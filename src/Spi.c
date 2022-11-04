/*
 * Created on Sun Oct 30 2022 10:29:02 AM
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

#include <Spi.h>
#include <Spi_cfg.h>

#include <stddef.h>

// Global
Spi_StatusType Spi_State = SPI_UNINIT;
Spi_JobResultType Spi_JobResult;
Spi_SeqResultType Spi_SeqResult;


// Functions
void Spi_Init(const Spi_ConfigType* ConfigPtr) {
	//call board specific init function

	Spi_State = SPI_IDLE;
	Spi_JobResult = SPI_JOB_OK;
	Spi_SeqResult = SPI_SEQ_OK;
}


Std_ReturnType Spi_DeInit(void) {
	// call board specific de-init function

	Spi_State = SPI_UNINIT;

	return E_OK;
}


/* Service for writing one or more data to an IB SPI Handler/Driver Channel specified by parameter */
Std_ReturnType Spi_WriteIB(Spi_ChannelType Channel, const Spi_DataBufferType* DataBufferPtr) {
	int i;

	/* Check for valid channel number */
	if (Channel >= SPI_DRIVER_MAX_CHANNEL) {
		return E_NOT_OK;
	}

	/* reject request if the channel is not configured as Internal Buffer */
	if (SpiChannelCfg[Channel].spi_chan_type != SPI_CHAN_TYPE_IB) {
		return E_NOT_OK;
	}

	if (DataBufferPtr == NULL) {
		/* Copy the default data to buffer if incoming data pointer is NULL */
		for (i = 0; i < SpiChannelCfg[Channel].spi_default_data_len; i++) {
			SpiChannelCfg[Channel].spi_ib_buf_ptr[i] = SpiChannelCfg[Channel].spi_default_data[i];
		}
	}
	else {
		/* Here the complete buffer is copied irrespective of real length of the message */
		for (i = 0; i < SpiChannelCfg[Channel].spi_ch_buf_len; i++) {
			SpiChannelCfg[Channel].spi_ib_buf_ptr[i] = DataBufferPtr[i];
		}
	}
}
