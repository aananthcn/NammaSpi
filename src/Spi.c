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
#include <Dio.h>

#include <stddef.h>


#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Spi, LOG_LEVEL_DBG);


// Globals
static Spi_StatusType Spi_State = SPI_UNINIT;
static Spi_AsyncModeType Spi_Mode = SPI_INTERRUPT_MODE;

static Spi_StatusType Spi_HWUnitStatus[SPI_DRIVER_MAX_HW_UNIT];
static Spi_JobResultType Spi_JobResult[SPI_DRIVER_MAX_JOB];
static Spi_SeqResultType Spi_SeqResult[SPI_DRIVER_MAX_SEQUENCE];


// Functions
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

	if (SrcDataBufferPtr != NULL) {
		*SpiChannelCfg[Channel].spi_eb_buf_s_ptr = (uint8*) SrcDataBufferPtr;
	}

	if (DesDataBufferPtr != NULL) {
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



#define BUF_COUNT (1)
Std_ReturnType Spi_SyncTransmit_Channel(Spi_ChannelType ch_id, SpiExtDevID_Type hwdev_id) {
	int zspi_rc;
	uint8 *src_ptr, *dst_ptr;
	struct spi_buf_set tx_bufset, rx_bufset;
	struct spi_buf tx_buf[BUF_COUNT], rx_buf[BUF_COUNT];
	struct device *spidev;
	uint16 buf_len;

	if ((ch_id >= SPI_DRIVER_MAX_CHANNEL) || (SPI_DRIVER_MAX_CHANNEL == 0)) {
		return E_NOT_OK;
	}

	if ((hwdev_id >= SPI_DRIVER_MAX_HW_UNIT) || (SPI_DRIVER_MAX_HW_UNIT == 0)) {
		return E_NOT_OK;
	}

	if (Spi_HWUnitStatus[hwdev_id] != SPI_IDLE) {
		LOG_ERR("SPI channel %d is not ready!", hwdev_id);
		return E_NOT_OK;
	}

	/* get the spidev generated by Car-OS */
	spidev = (struct device *) Spi_ZDevs[hwdev_id];
	if (spidev == NULL) {
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

	// initialize zephyr spi buffer structure for spi_transceive, else you get hardware fault!!
	tx_buf[0].buf = src_ptr;
	tx_buf[0].len = buf_len;
	tx_bufset.buffers = (const struct spi_buf *) &tx_buf;
	tx_bufset.count = BUF_COUNT;

	rx_buf[0].buf = dst_ptr;
	rx_buf[0].len = buf_len;
	rx_bufset.buffers = (const struct spi_buf *) &rx_buf;
	rx_bufset.count = BUF_COUNT;



	Spi_HWUnitStatus[hwdev_id] = SPI_BUSY;

	// Send & receive channel data corresponding to ch_id channel
	zspi_rc = spi_transceive(spidev, &Spi_ZCfgs[hwdev_id], &tx_bufset, &rx_bufset);

	Spi_HWUnitStatus[hwdev_id] = SPI_IDLE;

	if (zspi_rc) {
		LOG_ERR("spi_transceive() failure!");
		return E_NOT_OK;
	}

	return E_OK;
}


// move this function
Std_ReturnType Spi_SyncTransmit_Job(Spi_JobType job_id) {
	int i;
	sint16 cs_pin;
	Spi_ChannelType ch_id;
	SpiExtDevID_Type hwdev_id;

	if (job_id >= SPI_DRIVER_MAX_JOB) {
		return E_NOT_OK;
	}

	Spi_JobResult[job_id] = SPI_JOB_QUEUED;
	hwdev_id = SpiJobCfg[job_id].spi_dev_assignment;

	// chip select assert, if CS_VIA_GPIO is configured
	cs_pin = SpiExternalDeviceCfg[hwdev_id].spi_cs_dio;
	if (cs_pin >= 0) {
		Dio_WriteChannel(cs_pin, STD_LOW);
	}

	// Send all channel data corresponding to this job
	for (i = 0; i < SpiJobCfg[job_id].spi_chan_list_size; i++) {
		ch_id = SpiJobCfg[job_id].spi_chan_list[i];
		Spi_SyncTransmit_Channel(ch_id, hwdev_id);
	}

	// chip select de-assert, if CS_VIA_GPIO is configured
	if (cs_pin >= 0) {
		Dio_WriteChannel(cs_pin, STD_HIGH);
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
	for (i = 0; i < SpiSequenceCfg[Sequence].spi_job_list_size; i++) {
		job_id = SpiSequenceCfg[Sequence].spi_job_list[i];
		Spi_SyncTransmit_Job(job_id);
	}

	Spi_SeqResult[Sequence] = SPI_SEQ_OK;
	Spi_State = SPI_IDLE;

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

	// TODO: call bsp specific function to initiate a transfer
	// TODO: spi_transceive_cb()

	Spi_State = SPI_BUSY;
	Spi_SeqResult[Sequence] = SPI_SEQ_PENDING;
	// Spi_JobResult[??] = SPI_JOB_QUEUED;

	return E_OK;
}



Std_ReturnType Spi_DeInit(void) {
	// call bsp specific de-init function

	Spi_State = SPI_UNINIT;

	return E_OK;
}



void Spi_Init(const Spi_ConfigType* ConfigPtr) {
	int i;

	Spi_State = SPI_IDLE;

	// SPI device initialization
	if (SPI_DRIVER_MAX_HW_UNIT <= 0) {
		LOG_ERR("Spi is initialized with 0 device configured!");
		return;
	}
	else {
		for (i = 0; i < SPI_DRIVER_MAX_HW_UNIT; i++) {
			/* Check if all SPI devices configured in DTS file are initialized */
			if (!device_is_ready(Spi_ZDevs[i])) {
				LOG_ERR("%s: SPI device %d is not ready", __func__, i);
				return;
			}
			Spi_HWUnitStatus[i] = SPI_IDLE;
		}
	}

	// SPI jobs initialization
	if (SPI_DRIVER_MAX_JOB <= 0) {
		LOG_ERR("Spi is initialized with 0 jobs configured!");
		return;
	}
	else {
		for (i = 0; i < SPI_DRIVER_MAX_JOB; i++) {
			Spi_JobResult[i] = SPI_JOB_OK;
		}
	}

	// SPI sequence initialization
	if (SPI_DRIVER_MAX_SEQUENCE <= 0) {
		LOG_ERR("Spi is initialized with 0 sequences configured!");
		return;
	}
	else {
		for (i = 0; i < SPI_DRIVER_MAX_SEQUENCE; i++) {
			Spi_SeqResult[i] = SPI_SEQ_OK;
		}
	}

	LOG_DBG("Init complete!");
}
