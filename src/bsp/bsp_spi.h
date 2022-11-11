/*
 * Created on Tue Nov 08 2022 10:49:41 PM
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
#ifndef NASR_BRD_SPI_H
#define NASR_BRD_SPI_H

#include <base_types.h>

int bsp_spi_init(u8 spi_id, u32 baudrate, u8 tfr_type, u8 cpol, u8 cspol, u8 databits);
int bsp_spi_exit(u8 spi_id);

int bsp_spi_tranceive_8bit(u8 spi_id, u8 *tx_buf, u8 *rx_buf, u16 len);
int bsp_spi_tranceive_16bit(u8 spi_id, u16 *tx_buf, u16 *rx_buf, u16 len);

#endif