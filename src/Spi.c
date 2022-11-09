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

#include <bsp_spi.h>


// Globals
static Spi_StatusType Spi_State = SPI_UNINIT;
static Spi_AsyncModeType Spi_Mode = SPI_INTERRUPT_MODE;

static Spi_StatusType Spi_HWUnitStatus[SPI_DRIVER_MAX_HW_UNIT];
static Spi_JobResultType Spi_JobResult[SPI_DRIVER_MAX_JOB];
static Spi_SeqResultType Spi_SeqResult[SPI_DRIVER_MAX_SEQUENCE];


// Functions
void Spi_Init(const Spi_ConfigType* ConfigPtr) {
	int i;
	uint32 baudrate;
	uint8 tfr_type;
	uint8 clk_pol;
	uint8 cs_pol;
	uint8 databits;

	Spi_State = SPI_IDLE;
	for (i = 0; i < SPI_DRIVER_MAX_HW_UNIT; i++) {
		/* collect data from Spi_cfg.c */
		baudrate = SpiExternalDeviceCfg[i].spi_baudrate;
		tfr_type = SpiExternalDeviceCfg[i].spi_tfr_type;
		clk_pol  = SpiExternalDeviceCfg[i].spi_shftclk_idle_level;
		cs_pol   = SpiExternalDeviceCfg[i].spi_cs_polarity;
		databits = SpiExternalDeviceCfg[i].spi_databits;
		/* initialize board and driver */
		bsp_spi_init(0, baudrate, tfr_type, clk_pol, cs_pol, databits);
		Spi_HWUnitStatus[i] = SPI_IDLE;
	}
	for (i = 0; i < SPI_DRIVER_MAX_JOB; i++) {
		Spi_JobResult[i] = SPI_JOB_OK;
	}
	for (i = 0; i < SPI_DRIVER_MAX_SEQUENCE; i++) {
		Spi_SeqResult[i] = SPI_SEQ_OK;
	}
}


Std_ReturnType Spi_DeInit(void) {
	// call bsp specific de-init function

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
			SpiChannelCfg[Channel].spi_ib_buf_s_ptr[i] = SpiChannelCfg[Channel].spi_default_data[i];
		}
	}
	else {
		/* Here the complete buffer is copied irrespective of real length of the message */
		for (i = 0; i < SpiChannelCfg[Channel].spi_ib_buf_len; i++) {
			SpiChannelCfg[Channel].spi_ib_buf_s_ptr[i] = DataBufferPtr[i];
		}
	}

	return E_OK;
}


/* Service for reading synchronously one or more data from an IB SPI Handler/Driver Channel specified by parameter */
Std_ReturnType Spi_ReadIB(Spi_ChannelType Channel, Spi_DataBufferType* DataBufferPointer) {
	int i;

	/* Check for valid channel number */
	if (Channel >= SPI_DRIVER_MAX_CHANNEL) {
		return E_NOT_OK;
	}

	if (DataBufferPointer == NULL) {
		return E_NOT_OK;
	}

	/* Here the complete buffer is copied irrespective of real length of the message */
	for (i = 0; i < SpiChannelCfg[Channel].spi_ib_buf_len; i++) {
		DataBufferPointer[i] = SpiChannelCfg[Channel].spi_ib_buf_d_ptr[i];
	}

	return E_OK;
}



/* Service to setup the buffers and the length of data for the EB SPI Handler/Driver Channel specified */
Std_ReturnType Spi_SetupEB(Spi_ChannelType Channel, const Spi_DataBufferType* SrcDataBufferPtr,
                           Spi_DataBufferType* DesDataBufferPtr, Spi_NumberOfDataType Length) {
	/* Check for valid channel number */
	if (Channel >= SPI_DRIVER_MAX_CHANNEL) {
		return E_NOT_OK;
	}

	*SpiChannelCfg[Channel].spi_eb_buf_l_ptr = Length;

	if (SrcDataBufferPtr == NULL) {
		*SpiChannelCfg[Channel].spi_eb_buf_s_ptr = (uint8*) SrcDataBufferPtr;
	}

	if (DesDataBufferPtr == NULL) {
		*SpiChannelCfg[Channel].spi_eb_buf_d_ptr = (uint8*) DesDataBufferPtr;
	}

	return E_OK;
}



Spi_StatusType Spi_GetStatus(void) {
	return Spi_State;
}

Spi_JobResultType Spi_GetJobResult(Spi_JobType Job) {
	if (Job >= SPI_DRIVER_MAX_JOB) {
		return SPI_JOB_FAILED;
	}

	return Spi_JobResult[Job];
}

Spi_SeqResultType Spi_GetSequenceResult(Spi_SequenceType Sequence) {
	if (Sequence >= SPI_DRIVER_MAX_SEQUENCE) {
		return SPI_SEQ_FAILED;
	}

	return Spi_SeqResult[Sequence];
}


void Spi_GetVersionInfo(Std_VersionInfoType* versioninfo) {
	if (versioninfo == NULL) {
		return;
	}

	versioninfo->vendorID = 0x0000;
	versioninfo->moduleID = 0x0000;
	versioninfo->sw_major_version = 20;
	versioninfo->sw_minor_version = 11;
	versioninfo->sw_patch_version = 0;
}


Spi_StatusType Spi_GetHWUnitStatus(Spi_HWUnitType HWUnit) {
	if (HWUnit >= SPI_DRIVER_MAX_HW_UNIT) {
		return SPI_UNINIT;
	}

	return Spi_HWUnitStatus[HWUnit];
}


/* Service cancels the specified on-going sequence transmission. */
void Spi_Cancel(Spi_SequenceType Sequence) {
	if (Sequence >= SPI_DRIVER_MAX_SEQUENCE) {
		return;
	}

	Spi_SeqResult[Sequence] = SPI_SEQ_CANCELED;
}


/* Service to set the asynchronous mechanism mode for SPI busses handled asynchronously */
Std_ReturnType Spi_SetAsyncMode(Spi_AsyncModeType Mode) {
	if (Mode >= SPI_MODE_MAX) {
		return E_NOT_OK;
	}

	Spi_Mode = Mode;
	return E_OK;
}


// move this function
Std_ReturnType Spi_SyncTransmit_Channel(Spi_ChannelType ch_id, SpiExtDevID_Type hwdev) {
	int i;
	uint8 *src_ptr, *dst_ptr;
	uint16 buf_len;

	if (ch_id >= SPI_DRIVER_MAX_CHANNEL) {
		return E_NOT_OK;
	}

	if (hwdev >= SPI_DRIVER_MAX_HW_UNIT) {
		return E_NOT_OK;
	}

	// find out source and destination pointer
	if (SpiChannelCfg[ch_id].spi_chan_type == SPI_CHAN_TYPE_IB) {
		src_ptr = SpiChannelCfg[ch_id].spi_ib_buf_s_ptr;
		dst_ptr = SpiChannelCfg[ch_id].spi_ib_buf_d_ptr;
		buf_len = SpiChannelCfg[ch_id].spi_ib_buf_len;
	}
	else {
		src_ptr = *SpiChannelCfg[ch_id].spi_eb_buf_s_ptr;
		dst_ptr = *SpiChannelCfg[ch_id].spi_eb_buf_d_ptr;
		buf_len = *SpiChannelCfg[ch_id].spi_eb_buf_l_ptr;
	}

	Spi_HWUnitStatus[hwdev] = SPI_BUSY;

	// Send all channel data corresponding to this channel

	return E_OK;
}


// move this function
Std_ReturnType Spi_SyncTransmit_Job(Spi_JobType job_id) {
	int i;
	Spi_ChannelType ch_id;

	if (job_id >= SPI_DRIVER_MAX_JOB) {
		return E_NOT_OK;
	}

	Spi_JobResult[job_id] = SPI_JOB_QUEUED;

	// Send all channel data corresponding to this job
	for (int i; i < SpiJobCfg[job_id].spi_chan_list_size; i++) {
		ch_id = SpiJobCfg[job_id].spi_chan_list[i];
		Spi_SyncTransmit_Channel(ch_id, SpiJobCfg[job_id].spi_dev_assignment);
	}

	return E_OK;
}



/* Service to transmit data on the SPI bus */
Std_ReturnType Spi_SyncTransmit(Spi_SequenceType Sequence) {
	int i;
	Spi_JobType job_id;

	if (Sequence >= SPI_DRIVER_MAX_SEQUENCE) {
		return SPI_SEQ_FAILED;
	}

	if (Spi_SeqResult[Sequence] == SPI_SEQ_PENDING) {
		return E_NOT_OK;
	}

	Spi_State = SPI_BUSY;
	Spi_SeqResult[Sequence] = SPI_SEQ_PENDING;

	// Execute Jobs of this Sequence
	for (int i; i < SpiSequenceCfg[Sequence].spi_job_list_size; i++) {
		job_id = SpiSequenceCfg[Sequence].spi_job_list[i];
		Spi_SyncTransmit_Job(job_id);
	}

	return E_OK;
}


/* Service to transmit data on the SPI bus */
Std_ReturnType Spi_AsyncTransmit(Spi_SequenceType Sequence) {
	if (Sequence > SPI_DRIVER_MAX_SEQUENCE) {
		return E_NOT_OK;
	}

	if (Spi_SeqResult[Sequence] == SPI_SEQ_PENDING) {
		return E_NOT_OK;
	}

	// call bsp specific function to initiate a transfer

	Spi_State = SPI_BUSY;
	Spi_SeqResult[Sequence] = SPI_SEQ_PENDING;
	// Spi_JobResult[??] = SPI_JOB_QUEUED;

	return E_OK;
}

