#ifndef NAMMA_AUTOSAR_SPI_CFG_H
#define NAMMA_AUTOSAR_SPI_CFG_H

// This file is autogenerated, any hand modifications will be lost!

#include <Platform_Types.h>

#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>


#define SPI_CHAN_TYPE_IB        1
#define SPI_CHAN_TYPE_EB        2
#define SPI_CHAN_TYPE_IB_EB     3


typedef struct {
    uint8 spi_level_delivered;
    uint8 spi_chan_buff_allowed;
    boolean spi_intr_seq_allowed;
    boolean spi_hw_status_api;
    boolean spi_cancel_api;
    boolean spi_version_info_api;
    boolean spi_dev_error_detect;
    boolean spi_supp_conc_sync_tx;
    uint32 spi_main_func_period_ms;
} SpiGeneralCfgType;


typedef enum {
	SPI_EXT_DEV_spi0,
	SPI_EXT_DEV_MAX
} SpiExtDevID_Type;


typedef enum {
    SPI_EDGE_LEADING,
    SPI_EDGE_TRAILING
} SpiDataShiftEdgeType;


typedef enum {
    SPI_LEVEL_LOW,
    SPI_LEVEL_HIGH
} SpiLevelType;


typedef enum {
    CS_VIA_PERIPHERAL_ENGINE,
    CS_VIA_GPIO
} SpiCsSelectionType;


typedef enum {
    SPI_TX_START_MSB,
    SPI_TX_START_LSB
} SpiTransferStartType;


typedef enum {
    MOTOROLA_SPI,
    TI_SSI,
    NS_MICROWIRE
} SpiFrameFormatType;


typedef struct {
    SpiExtDevID_Type spi_hw_unit_id;
    uint32 spi_baudrate;
    SpiDataShiftEdgeType spi_data_shift_edge;
    SpiLevelType spi_shftclk_idle_level;
    boolean spi_enable_cs;
    char spi_cs_id[128];
    SpiCsSelectionType spi_cs_selection;
    sint16 spi_cs_dio;
    SpiLevelType spi_cs_polarity;
    uint32 spi_usec_clk_2_cs;
    uint32 spi_usec_cs_2_clk;
    uint32 spi_usec_cs_2_cs;
    uint8 spi_databits;
    SpiTransferStartType spi_tfr_type;
    SpiFrameFormatType spi_frame_fmt;
} SpiExternalDeviceType;


typedef struct {
    uint16 spi_chan_id;
    uint8 spi_chan_type;
    uint16 spi_data_width; /* width in bits */
    const uint8* spi_default_data;
    uint16 spi_default_data_len;
    uint16 spi_ib_num_buf;
    uint32 spi_ib_buf_len;   /* length of int. src & dst buffers */
    uint8* spi_ib_buf_s_ptr; /* int. src buffer (Tx buffer) ptr */
    uint8* spi_ib_buf_d_ptr; /* int. dst buffer (Rx buffer) ptr */
    uint16 spi_eb_max_len;
    uint16* spi_eb_buf_l_ptr; /* length of ext. src & dst buffers */
    uint8** spi_eb_buf_s_ptr; /* ext. src buffer (Tx buffer) ptr */
    uint8** spi_eb_buf_d_ptr; /* ext. dst buffer (Rx buffer) ptr */
    SpiTransferStartType spi_tx_start;
} SpiChannelCfgType;


typedef struct {
    uint16 spi_job_id;
    uint16 spi_job_priority;
    void (*job_end_notification_fn)(void);
    SpiExtDevID_Type spi_dev_assignment;
    uint16 spi_chan_list_size;
    const uint16* spi_chan_list;
} SpiJobCfgType;


typedef struct {
    uint16 spi_seq_id;
    boolean spi_seq_interruptible;
    void (*seq_end_notification_fn)(void);
    uint16 spi_job_list_size;
    const uint16* spi_job_list;
} SpiSequenceCfgType;



#define SPI_DRIVER_MAX_CHANNEL   (1)
#define SPI_DRIVER_MAX_JOB       (1)
#define SPI_DRIVER_MAX_SEQUENCE  (1)
#define SPI_DRIVER_MAX_HW_UNIT   (1)

typedef struct {
    const SpiGeneralCfgType general;
    const SpiExternalDeviceType* devices[SPI_DRIVER_MAX_HW_UNIT];
    const SpiChannelCfgType* channels[SPI_DRIVER_MAX_CHANNEL];
    const SpiJobCfgType* jobs[SPI_DRIVER_MAX_JOB];
    const SpiSequenceCfgType* sequences[SPI_DRIVER_MAX_SEQUENCE];
} Spi_ConfigType;


/* NammaAUTOSAR's custom enum, which will make code more readable */
typedef enum {
	SEQ_ETHERNET_BASIC_TX_RX,
	SEQ_ENUM_MAX
} Spi_SequenceEnumType;


extern const SpiGeneralCfgType SpiGeneralCfg;
extern const SpiExternalDeviceType SpiExternalDeviceCfg[SPI_DRIVER_MAX_HW_UNIT];
extern const SpiChannelCfgType SpiChannelCfg[SPI_DRIVER_MAX_CHANNEL];
extern const SpiJobCfgType SpiJobCfg[SPI_DRIVER_MAX_JOB];
extern const SpiSequenceCfgType SpiSequenceCfg[SPI_DRIVER_MAX_SEQUENCE];
extern const Spi_ConfigType SpiConfigs;


/* Zephyr SPI Device Constants Declarations */
extern const struct device *Spi_ZDevs[];
extern const struct spi_config Spi_ZCfgs[];

#endif
