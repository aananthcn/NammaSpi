#include <stddef.h>
#include <Spi_cfg.h>


// This file is autogenerated, any hand modifications will be lost!


const SpiGeneralCfgType SpiGeneralCfg = {
	.spi_level_delivered     = 1,
	.spi_chan_buff_allowed   = SPI_CHAN_TYPE_IB,
	.spi_intr_seq_allowed    = FALSE,
	.spi_hw_status_api       = FALSE,
	.spi_cancel_api          = FALSE,
	.spi_version_info_api    = FALSE,
	.spi_dev_error_detect    = FALSE,
	.spi_supp_conc_sync_tx   = FALSE,
	.spi_main_func_period_ms = 10
};


const SpiExternalDeviceType SpiExternalDeviceCfg[] = {
	{
		.spi_hw_unit_id = SPI_EXT_DEV_CSIB0,
		.spi_baudrate = 1000000, /* bps or Hz */
		.spi_data_shift_edge = SPI_EDGE_LEADING,
		.spi_shftclk_idle_level = SPI_LEVEL_LOW,
		.spi_enable_cs = FALSE,
		.spi_cs_id = "CS_Ethernet",
		.spi_cs_selection = CS_VIA_PERIPHERAL_ENGINE,
		.spi_cs_polarity = SPI_LEVEL_LOW,
		.spi_usec_clk_2_cs = 0,
		.spi_usec_cs_2_clk = 1,
		.spi_usec_cs_2_cs = 1
	}
};


const uint8 SpiDefaultData_0[] = {
	0xAA, 0x55, 0x12, 0x34, 0x45, 0x8a, 0x45, 0xb3, 0x87, 0x61, 0x90, 
};

const uint8 SpiDefaultData_1[] = {
	0xAA, 0x55, 0x12, 0x34, 
};

static uint8 SpiIB_BufferChn_0[1500];
static uint16 SpiEB_BufferLen_Chn_1;
static uint8* SpiEB_BufferSrc_Chn_1;
static uint8* SpiEB_BufferDst_Chn_1;

const SpiChannelCfgType SpiChannelCfg[] = {
	{
		.spi_chan_id = 0,
		.spi_chan_type = SPI_CHAN_TYPE_IB,
		.spi_data_width = 1,
		.spi_default_data = SpiDefaultData_0,
		.spi_default_data_len = 11,
		.spi_eb_max_len = 0,
		.spi_ib_num_buf = 1500,
		.spi_ib_buf_len = 1500,
		.spi_ib_buf_ptr = SpiIB_BufferChn_0,
		.spi_eb_buf_l_ptr = NULL,
		.spi_eb_buf_s_ptr = NULL,
		.spi_eb_buf_d_ptr = NULL,
		.spi_tx_start = SPI_TX_START_MSB
	},
	{
		.spi_chan_id = 1,
		.spi_chan_type = SPI_CHAN_TYPE_EB,
		.spi_data_width = 4,
		.spi_default_data = SpiDefaultData_1,
		.spi_default_data_len = 4,
		.spi_eb_max_len = 0,
		.spi_ib_num_buf = 0,
		.spi_ib_buf_len = 0,
		.spi_ib_buf_ptr = NULL,
		.spi_eb_buf_l_ptr = &SpiEB_BufferLen_Chn_1,
		.spi_eb_buf_s_ptr = &SpiEB_BufferSrc_Chn_1,
		.spi_eb_buf_d_ptr = &SpiEB_BufferDst_Chn_1,
		.spi_tx_start = SPI_TX_START_MSB
	}
};


const uint16 SpiChannelList_0[] = {
	0, 
};

const SpiJobCfgType SpiJobCfg[] = {
	{
		.spi_job_id = 0,
		.spi_job_priority = 0,
		.job_end_notification_fn = NULL,
		.spi_chan_list_size = 1,
		.spi_chan_list = SpiChannelList_0,
	}
};


const uint16 SpiJobAssignment_0[] = {
	0, 
};

const SpiSequenceCfgType SpiSequenceCfg[] = {
	{
		.spi_seq_id = 0,
		.spi_seq_interruptible = FALSE,
		.seq_end_notification_fn = NULL,
		.spi_job_list_size = 1,
		.spi_job_list = SpiJobAssignment_0,
	}
};


const Spi_ConfigType SpiCfg = {
	.general   = SpiGeneralCfg,
	.devices   = SpiExternalDeviceCfg,
	.channels  = SpiChannelCfg,
	.jobs      = SpiJobCfg,
	.sequences = SpiSequenceCfg
};

