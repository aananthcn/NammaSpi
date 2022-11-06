/*
 * Created on Sat Oct 01 2022 8:08:30 PM
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
#ifndef NAMMA_AUTOSAR_SPI_H
#define NAMMA_AUTOSAR_SPI_H


#include <Spi_cfg.h> // auto-generated file, contains structre & macros related to Spi_ConfigType
#include <Platform_Types.h>
#include <Std_Types.h>
#include <Spi_cfg.h>


typedef enum {
        SPI_UNINIT,
        SPI_IDLE,
        SPI_BUSY
} Spi_StatusType;


/* A Job shall contain at least one Channel */
typedef enum {
        SPI_JOB_OK,
        SPI_JOB_PENDING, /* == SPI_BUSY */
        SPI_JOB_FAILED,
        SPI_JOB_QUEUED
} Spi_JobResultType;
typedef uint16 Spi_JobType;


/* A sequence shall contain at least one Job */
typedef enum {
        SPI_SEQ_OK,
        SPI_SEQ_PENDING,
        SPI_SEQ_FAILED,
        SPI_SEQ_CANCELED
} Spi_SeqResultType;
typedef uint8 Spi_SequenceType;


typedef uint8 Spi_DataBufferType;
typedef uint16 Spi_NumberOfDataType;

/* Each Channel shall have an associated index which is used for
   specifying the order of the Channel within the Job */
typedef uint8 Spi_ChannelType;


typedef uint8 Spi_HWUnitType;

typedef enum {
        SPI_POLLING_MODE,
        SPI_INTERRUPT_MODE,
        SPI_MODE_MAX
} Spi_AsyncModeType;


/* Function Declaration */
void Spi_Init(const Spi_ConfigType* ConfigPtr);
Std_ReturnType Spi_DeInit(void);

/* Service for writing one or more data to an IB SPI Handler/Driver Channel specified by parameter */
Std_ReturnType Spi_WriteIB(Spi_ChannelType Channel, const Spi_DataBufferType* DataBufferPtr);

/* Service to transmit data on the SPI bus */
Std_ReturnType Spi_AsyncTransmit(Spi_SequenceType Sequence);

/* Service for reading synchronously one or more data from an IB SPI Handler/Driver Channel specified by parameter */
Std_ReturnType Spi_ReadIB(Spi_ChannelType Channel, Spi_DataBufferType* DataBufferPointer);

/* Service to setup the buffers and the length of data for the EB SPI Handler/Driver Channel specified */
Std_ReturnType Spi_SetupEB(Spi_ChannelType Channel, const Spi_DataBufferType* SrcDataBufferPtr,
                           Spi_DataBufferType* DesDataBufferPtr, Spi_NumberOfDataType Length);

/* Service to transmit data on the SPI bus */
Std_ReturnType Spi_SyncTransmit(Spi_SequenceType Sequence);

Spi_StatusType Spi_GetStatus(void);
Spi_JobResultType Spi_GetJobResult(Spi_JobType Job);
Spi_SeqResultType Spi_GetSequenceResult(Spi_SequenceType Sequence);
void Spi_GetVersionInfo(Std_VersionInfoType* versioninfo);


Spi_StatusType Spi_GetHWUnitStatus(Spi_HWUnitType HWUnit);
void Spi_Cancel(Spi_SequenceType Sequence);
Std_ReturnType Spi_SetAsyncMode(Spi_AsyncModeType Mode);



#endif