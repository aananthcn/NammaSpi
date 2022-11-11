/*
 * Created on Tue Nov 08 2022 10:49:33 PM
 *
 * The MIT License (MIT)
 * Copyright (c) 2022 Aananth C N
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <rp2040.h>
#include <Spi_cfg.h>
#include <stddef.h>

#include <bsp_spi.h>
#include <platform.h>
#include <os_api.h>



int bsp_spi_init(u8 spi_id, u32 baudrate, u8 tfr_type, u8 cpol, u8 cspol, u8 databits) {
	u32 lsh4_ps;
	u32 spibase = SPI0_BASE;
	u8 roundup4 = 0;

	if (spi_id >= 2) {
		pr_log("Spi Error: Invalid spi_id for RPi Pico\n");
		return -1;
	}
	else if (spi_id == 1) {
		spibase = SPI1_BASE;
	}

	/* reset the SPI block */
	RESET_CTRL |= (SS_BIT_SPI0 << spi_id);
	RESET_CTRL &= ~(SS_BIT_SPI0 << spi_id);

	/* Set the baudrate -- clock source SSPCLK = clk_peri = 125 MHz clk_sys.
	   Prescale divisor shall be an even number between 2 and 254. So the 
	   logic used below is to round up the prescale by 2 in case the decimal
	   part is greater than 1/16th */
	lsh4_ps = ((125000000 << 4) / baudrate);
	if (lsh4_ps & 0xF) {
		roundup4 = 2;
	}
	SSPCPSR(spibase) = (u8) (((lsh4_ps >> 4) + roundup4) & 0xFE);

	/* Control register 0 & 1 */
	if (tfr_type) {
		/* LSB First is not supported by RPi Pico (PL022) */
		pr_log("Spi Error: LSB First is not suppored by RPi Pico\n");
		return -1;
	}
	if (cspol) {
		/* Chip Select active HIGH is not supported by RPi Pico (PL022) */
		pr_log("Spi Error: Chip Select active HIGH is not suppored by RPi Pico\n");
		return -1;
	}
	if (cpol) {
		cpol = 1 << 6; // SPO
	}
	if ((databits < 4) || (databits > 16)) {
		pr_log("Spi Error: Invalid databits argument passed\n");
	}
	else {
		databits = databits - 1;
	}
	SSPCR0(spibase) = cpol | databits;
	SSPCR1(spibase) = 1 << 1; // SSE enabled

	return 0;
}


int bsp_spi_exit(u8 spi_id) {
	u32 spibase = SPI0_BASE;

	if (spi_id >= 2) {
		pr_log("Spi Error: Invalid spi_id for RPi Pico\n");
		return -1;
	}
	else if (spi_id == 1) {
		spibase = SPI1_BASE;
	}

	SSPCR1(spibase) &= ~(1 << 1); // SSE disabled
	SSPDMACR(spibase) &= ~(0x3);  // Tx and Rx DMA disabled

	return 0;
}



static inline boolean is_tx_fifo_full(u32 spibase) {
	if (SSPSR(spibase) & SPI_TNF) {
		return FALSE;
	}

	return TRUE;
}

static inline boolean is_rx_fifo_empty(u32 spibase) {
	if (SSPSR(spibase) & SPI_RNE) {
		return FALSE;
	}

	return TRUE;
}

int bsp_spi_tranceive_8bit(u8 spi_id, u8 *tx_buf, u8 *rx_buf, u16 len) {
	u32 spibase = SPI0_BASE;
	int i;

	if (spi_id >= 2) {
		pr_log("Spi Error: Invalid spi_id for RPi Pico\n");
		return -1;
	}
	else if (spi_id == 1) {
		spibase = SPI1_BASE;
	}

	if ((tx_buf == NULL) || (rx_buf == NULL)) {
		pr_log("Spi Error: NULL pointer passed as Tx or Rx buffer\n");
		return -1;
	}

	for (i = 0; i < len; i++) {
		/* wait if Tx FIFO is full */
		while(is_tx_fifo_full(spibase)); // this line is dangerous, will add timed loop exit later
		SSPDR(spibase) = tx_buf[i];

		/* wait if Rx FIFO is empty*/
		while(is_rx_fifo_empty(spibase)); // this line is dangerous, will add timed loop exit later
		rx_buf[i] = SSPDR(spibase);
	}

	return 0;
}


int bsp_spi_tranceive_16bit(u8 spi_id, u16 *tx_buf, u16 *rx_buf, u16 len) {
	u32 spibase = SPI0_BASE;
	int i;

	if (spi_id >= 2) {
		pr_log("Spi Error: Invalid spi_id for RPi Pico\n");
		return -1;
	}
	else if (spi_id == 1) {
		spibase = SPI1_BASE;
	}

	if ((tx_buf == NULL) || (rx_buf == NULL)) {
		pr_log("Spi Error: NULL pointer passed as Tx or Rx buffer\n");
		return -1;
	}

	for (i = 0; i < len; i++) {
		/* wait if Tx FIFO is full */
		while(is_tx_fifo_full(spibase)); // this line is dangerous, will add timed loop exit later
		SSPDR(spibase) = tx_buf[i];

		/* wait if Rx FIFO is empty*/
		while(is_rx_fifo_empty(spibase)); // this line is dangerous, will add timed loop exit later
		rx_buf[i] = SSPDR(spibase);
	}

	return 0;
}