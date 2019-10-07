/*******************************************************************************
 * Copyright:		BUPT
 * File Name:		nrf24l01.c
 * Description:		NRF Driver. Ref benjamin bldc code.
 * Author:			ZeroVoid
 * Version:			0.2
 * Data:			2019/10/07 Mon 18:33
 *******************************************************************************/

#include "nrf24l01.h"
#include <string.h>

/*******************************************************************************
 * NRF Private Macro
 *******************************************************************************/
#define NRF_PAYLOAD_SIZE					32 // NRF Payload Register Size(Bytes)

/*******************************************************************************
 * NRF Private Macro Functions
 *******************************************************************************/
#define NRF_SPI_TransmitReceive(pTxData, pRxData, len)   HAL_SPI_TransmitReceive(&NRF_SPI_Handle, pTxData, pRxData, len, 50)
#define NRF_SPI_Read(pRxData, len) HAL_SPI_Receive(&NRF_SPI_Handle, pRxData, len, 50)
#define NRF_SPI_Write(pTxData, len) HAL_SPI_Transmit(&NRF_SPI_Handle, pTxData, len, 50)

/*******************************************************************************
 * NRF Private Function Declaration
 *******************************************************************************/
/* NRF Register Write & Read -----------------------------------------------------*/
static void nrf_write_reg(uint8_t reg, const uint8_t *data, uint8_t len);
static void nrf_write_reg_byte(uint8_t reg, uint8_t value);
static void nrf_read_reg(uint8_t reg, uint8_t *data, uint8_t len);
static uint8_t nrf_read_reg_byte(uint8_t reg);

/* NRF SPI Communication Utils -----------------------------------------------------*/
static void nrf_spi_start(void);
static void nrf_spi_end(void);
static void nrf_spi_dealy(void);

/*******************************************************************************
 * NRF Val
 *******************************************************************************/
uint8_t nrf_rx_data[32];
uint8_t nrf_tx_data[32];

/*******************************************************************************
 * NRF Private Val
 *******************************************************************************/
//static uint8_t nrf_spi_rx;
static NRF_ConfigTypeDef nrf_config = {
    speed : NRF_SPEED_2M,
    power : NRF_POWER_UP,
    rf_power : NRF_RF_POWER_0DBM,
    crc_type : NRF_CRC_2B,
    retry_delay : NRF_RETR_DELAY_500US,
    retries : 5,
    channel : 0,
    address : {0x10, 0x11, 0x12, 0x11, 0x10},
    addr_len : NRF_AW_5,
    send_crc_ack : true
};
static NRF_AW nrf_addr_width;
static uint8_t nrf_tx_addr[5];
static uint8_t nrf_rx_addr[6][5];
static bool nrf_rx_addr_set[6];
static bool tx_pipe0_addr_eq;

/*******************************************************************************
 * NRF Mid Functions
 *******************************************************************************/
/**
 * @brief	Initialize the nrf24l01 driver
 * @param   config @see NRF_ConfigTypeDef
 */
void nrf_init(NRF_ConfigTypeDef *config) {
    _nrf_set_power(NRF_POWER_DOWN);
    // config 为NULL 则使用默认config
    if (config != NULL) {
        nrf_config = *config;
    }
    nrf_addr_width = nrf_config.addr_len;

	nrf_write_reg_byte(NRF_REG_EN_RXADDR, 0);
	nrf_write_reg_byte(NRF_REG_DYNPD, 0);

	_nrf_set_crc_type(nrf_config.crc_type);
	_nrf_set_retr_retries(nrf_config.retries);
	_nrf_set_retr_delay(nrf_config.retry_delay);
	_nrf_set_power(nrf_config.power);
	_nrf_set_speed(nrf_config.speed);
	_nrf_set_address_width(nrf_config.addr_len);
	_nrf_set_frequency(2400 + nrf_config.channel);
	_nrf_enable_features(NRF_FEATURE_DPL | NRF_FEATURE_DYN_ACK);

	_nrf_enable_pipe_autoack(NRF_MASK_PIPE0);
	_nrf_enable_pipe_address(NRF_MASK_PIPE0);
	_nrf_enable_pipe_dlp(NRF_MASK_PIPE0);

	memcpy(nrf_tx_addr, nrf_config.address, nrf_config.addr_len + 2);
	memcpy(nrf_rx_addr[0], nrf_config.address, nrf_config.addr_len + 2);
	tx_pipe0_addr_eq = true;

	_nrf_set_tx_addr(nrf_tx_addr, nrf_addr_width);
	_nrf_set_rx_addr(0, nrf_rx_addr[0], nrf_addr_width);

	if (nrf_config.power != NRF_POWER_DOWN) {
        _nrf_set_power(NRF_POWER_UP);
	}
    _nrf_flush_all();
    _nrf_clear_irq();
}

void nrf_stop(void) {
    _nrf_set_power(NRF_POWER_DOWN);
}

/**
 * @brief	Set Tx mode, send data, wait result, Set Rx mode
 * @param   data    Data to be sent
 * @param   len     Length of the data
 * @param   ack     Whether use the ack feature
 * @return	0       Send success
 * @return  1       Send failed after retries
 */
uint8_t nrf_send_data(uint8_t *data, int len, bool ack) {
	uint8_t retval = 0;

	rf_mode_tx();
	rf_clear_irq();
	rf_flush_all();

	// Pipe0-address and tx-address must be equal for ack to work.
	if (!tx_pipe0_addr_eq && ack) {
		_nrf_set_rx_addr(0, nrf_tx_addr, nrf_addr_width);
	}

	if (ack) {
		_nrf_write_tx_payload(data, len);
	} else {
		_nrf_write_tx_payload_no_ack(data, len);
	}

    uint8_t status;
    do
    {
        status = _nrf_get_status();
        if (NRF_STATUS_GET_MAX_RT(status)) {
            retval = 1;
            break;
        }
    } while ( !NRF_STATUS_GET_TX_DS(status) );
    
	// Restore pipe0 address
	if (!tx_pipe0_addr_eq && ack) {
		_nrf_set_rx_addr(0, nrf_rx_addr[0], nrf_addr_width);
	}
	rf_mode_rx();
    nrf_send_callback();

	return retval;
}

/**
 * @brief	NRF read data from rx fifo
 * @param   data    Pointer to the array in which to store the data
 * @param   len     Pointer to the pipe on which the data was received. Can be 0.
 * @return  1       Read ok. More data need to read.
 * @return	0       Read ok.
 * @return -1       No Rx data.
 * @return -2       Wrong data length. Something is likely wrong.
 */
uint8_t nrf_read_rx_data(uint8_t *data, uint8_t *len, NRF_PIPE *pipe) {
    int retval = -1;
	int status = _nrf_get_status();
	int pipe_n = NRF_STATUS_GET_RX_P_NO(status);

	if (pipe_n != NRF_RX_FIFO_EMPTY) {
		*len = _nrf_get_payload_width();
		if (pipe) {
			*pipe = pipe_n;
		}
		if (*len <= 32 && *len >= 0) {
			_nrf_read_rx_payload(data, *len);
			_nrf_clear_rx_irq();

			status = _nrf_get_status();
			if (NRF_STATUS_GET_RX_P_NO(status) == NRF_FIFO_RX_EMPTY) {
				retval = 0;
			} else {
				retval = 1;
			}
		} else {
			*len = 0;
			retval = -2;
		}
    }

    nrf_receive_callback(data, *len);
	return retval;
}

void nrf_set_tx_addr(const uint8_t *addr, uint8_t addr_len) {
    memcpy(nrf_tx_addr, addr, addr_len);
	nrf_addr_width = addr_len;
	tx_pipe0_addr_eq = memcmp(nrf_rx_addr[0], nrf_tx_addr, nrf_addr_width) == 0;
	_nrf_set_tx_addr(nrf_tx_addr, nrf_addr_width);
}

void nrf_set_rx_addr(NRF_PIPE pipe, const uint8_t *addr, uint8_t addr_len) {
    memcpy(nrf_rx_addr[pipe], addr, addr_len);
	nrf_addr_width = addr_len;
	tx_pipe0_addr_eq = memcmp(nrf_rx_addr[0], nrf_tx_addr, nrf_addr_width) == 0;
	nrf_rx_addr_set[pipe] = true;
	_nrf_set_rx_addr(pipe, addr, nrf_addr_width);
}


/*******************************************************************************
 * NRF Driver Functions
 *******************************************************************************/
/* 0x00 Configuration Register -----------------------------------------------------*/
// TODO: ZeroVoid	due:compelete	todo
void _nrf_enable_irq(uint8_t irq);

/**
 * @brief	设置CRC校验位个数
 * @param   crc_type @see NRF_CRC
 */
void _nrf_set_crc_type(NRF_CRC crc_type) {
    uint8_t reg_old = nrf_read_reg_byte(NRF_REG_CONFIG);
	uint8_t reg_new = reg_old;

	reg_new &= ~(NRF_CONFIG_CRCO | NRF_CONFIG_EN_CRC);

	switch (crc_type) {
	case NRF_CRC_DISABLED:
		break;

	case NRF_CRC_1B:
		reg_new |= NRF_CONFIG_EN_CRC;
		break;

	case NRF_CRC_2B:
		reg_new |= NRF_CONFIG_EN_CRC | NRF_CONFIG_CRCO;
		break;

	default:
		break;
	}

	if (reg_old != reg_new) {
		nrf_write_reg_byte(NRF_REG_CONFIG, reg_new);	// Update if we need
	}
}

/**
 * @brief   设置NRF是否工作
 * @param	power enum: NRF_POWER_UP or NRF_POWER_DOWN
 */
void _nrf_set_power(NRF_POWER power) {
    uint8_t config = nrf_read_reg_byte(NRF_REG_CONFIG);
    if ((config & NRF_CONFIG_PWR_UP) != power) {
        if (power) {
            config |= (NRF_CONFIG_PWR_UP);
        } else {
            config &= ~(NRF_CONFIG_PWR_UP);
        }
        nrf_write_reg_byte(NRF_REG_CONFIG, config);
    }
}

/**
 * @brief	Set work mode rx or tx
 * @param   mode: NRF_PTX or NRF_PRX
 */
void _nrf_set_mode(NRF_MODE mode) {
    uint8_t config = nrf_read_reg_byte(NRF_REG_CONFIG);
    if ((config & NRF_CONFIG_PRIM_RX) != mode) {
        if (mode) {
            config |= (NRF_CONFIG_PRIM_RX);
        } else {
            config &= ~(NRF_CONFIG_PRIM_RX);
        }
        nrf_write_reg_byte(NRF_REG_CONFIG, config);
    }
}

/* 0x01-x03 -----------------------------------------------------*/
/**
 * @brief	Enable autoack
 * @param   pipes: mask 6bit
 */
void _nrf_enable_pipe_autoack(uint8_t pipes) {
    uint8_t tmp = nrf_read_reg_byte(NRF_REG_EN_AA);
	if ((tmp & (pipes)) != (pipes)) {
		tmp |= (pipes);
		nrf_write_reg_byte(NRF_REG_EN_AA, tmp);	//Update if we need
	}
}

void _nrf_enable_pipe_address(uint8_t pipes) {
    uint8_t tmp = nrf_read_reg_byte(NRF_REG_EN_RXADDR);
	if ((tmp & (pipes)) != (pipes)) {
		tmp |= (pipes);
		nrf_write_reg_byte(NRF_REG_EN_RXADDR, tmp);	//Update if we need
	}
}

void _nrf_set_address_width(NRF_AW aw) {
    nrf_write_reg_byte(NRF_REG_SETUP_AW, aw);
}

NRF_AW _nrf_get_address_width(void) {
    return nrf_read_reg_byte(NRF_REG_SETUP_AW);
}

/* 0x04 -----------------------------------------------------*/
void _nrf_set_retr_delay(NRF_RETR_DELAY delay) {
    uint8_t reg_old = nrf_read_reg_byte(NRF_REG_SETUP_RETR);
	uint8_t reg_new = reg_old;

	reg_new &= ~NRF_SETUP_RETR_ARD;
	reg_new |= ((uint8_t)delay & 0xF) << 4;

	if (reg_old != reg_new) {
		nrf_write_reg_byte(NRF_REG_SETUP_RETR, reg_new);	// Update if we need
	}
}
void _nrf_set_retr_retries(uint8_t retries) {
    uint8_t reg_old = nrf_read_reg_byte(NRF_REG_SETUP_RETR);
	uint8_t reg_new = reg_old;

	reg_new &= ~NRF_SETUP_RETR_ARC;
	reg_new |= retries & 0xF;

	if (reg_old != reg_new) {
		nrf_write_reg_byte(NRF_REG_SETUP_RETR, reg_new);	// Update if we need
	}
}

/* 0x05 -----------------------------------------------------*/
/**
 * @brief	Set radio frequency in MHz (2400 to 2525 allowed)
 */
void _nrf_set_frequency(int freq) {
    nrf_write_reg_byte(NRF_REG_RF_CH, (freq - 2400) & 0x7F); // F = 2400 + RF_CH[MHz] [6:0]
}

int _nrf_get_frequency(void) {
    return nrf_read_reg_byte(NRF_REG_RF_CH) + 2400;
}

/* 0x06 -----------------------------------------------------*/
void _nrf_set_speed(NRF_SPEED speed) {
    uint8_t reg_old = nrf_read_reg_byte(NRF_REG_RF_SETUP);
	uint8_t reg_new = reg_old;

	reg_new &= ~(NRF_RF_SETUP_RF_DR_LOW | NRF_RF_SETUP_RF_DR_HIGH);

	switch (speed) {
	case NRF_SPEED_250K:
		reg_new |= NRF_RF_SETUP_RF_DR_LOW;
		break;

	case NRF_SPEED_1M:
		break;

	case NRF_SPEED_2M:
		reg_new |= NRF_RF_SETUP_RF_DR_HIGH;
		break;

	default:
		break;
	}

	if (reg_old != reg_new) {
		nrf_write_reg_byte(NRF_REG_RF_SETUP, reg_new);	// Update if we need
	}
}
void _nrf_set_rf_power(NRF_RF_POWER power) {
	uint8_t reg_old = nrf_read_reg_byte(NRF_REG_RF_SETUP);
	uint8_t reg_new = reg_old;

	reg_new &= ~(NRF_RF_SETUP_RF_PWR | 1);
	reg_new |= (uint8_t)power << 1;

	// In case this is a SI24R1 chip and the highest power is requested, set
	// the first bit to get 7dBm output.
	if (power == NRF_RF_POWER_0DBM) {
		reg_new |= 1;
	}

	if (reg_old != reg_new) {
		nrf_write_reg_byte(NRF_REG_RF_SETUP, reg_new);	// Update if we need
	}
}

/* 0x07 -----------------------------------------------------*/
uint8_t _nrf_get_status(void) {
    return nrf_read_reg_byte(NRF_REG_CONFIG);
}

void _nrf_clear_irq(void) {
    nrf_write_reg_byte(NRF_REG_STATUS, NRF_STATUS_IRQ);
}

void _nrf_clear_rx_irq(void) {
    nrf_write_reg_byte(NRF_REG_STATUS, NRF_STATUS_RX_DR);
}

void _nrf_clear_tx_irq(void) {
    nrf_write_reg_byte(NRF_REG_STATUS, NRF_STATUS_TX_DS);
}

void _nrf_clear_maxrt_irq(void) {
    nrf_write_reg_byte(NRF_REG_STATUS, NRF_STATUS_MAX_RT);
}

/* 0x08 Transmit Observe Register -----------------------------------------------------*/
uint8_t _nrf_get_plos_cnt(void) {
    uint8_t temp = nrf_read_reg_byte(NRF_REG_OBSERVE_TX);
    return (temp>>NRF_PLOS_CNT_POS);
}

uint8_t _nrf_get_arc_cnt(void) {
    uint8_t temp = nrf_read_reg_byte(NRF_REG_OBSERVE_TX);
    return (temp & NRF_ARC_CNT);
}

/* 0x09 -----------------------------------------------------*/
uint8_t _nrf_rx_power_detect(void) {
    return nrf_read_reg_byte(NRF_REG_RPD);
}

/* 0x0A-0x0F -----------------------------------------------------*/
void _nrf_set_rx_addr(NRF_PIPE pipe, const uint8_t *address, uint8_t addr_len) {
    nrf_write_reg(NRF_REG_RX_ADDR_P0 + pipe, address, addr_len);
}

/* 0x10 -----------------------------------------------------*/
void _nrf_set_tx_addr(const uint8_t *address, int addr_len) {
    nrf_write_reg(NRF_REG_TX_ADDR, address, addr_len);
}

/* 0x11-0x16 -----------------------------------------------------*/
uint8_t _nrf_get_payload_width(void) {
    uint8_t w;
	uint8_t cmd = NRF_CMD_READ_RX_PAYLOAD_WIDTH;
    nrf_spi_start();
    NRF_SPI_TransmitReceive(&cmd, 0, 1);
    NRF_SPI_Read(&w, 1);
    nrf_spi_end();
	return w;
}

uint8_t _nrf_get_payload_width_pipe(NRF_PIPE pipe) {
    return nrf_read_reg_byte(NRF_REG_RX_PW_P0 + pipe);
}

/* 0x17 -----------------------------------------------------*/
uint8_t _nrf_get_fifo_status(void) {
    return nrf_read_reg_byte(NRF_REG_FIFO_STATUS);
}

void _nrf_write_tx_payload(const uint8_t *data, uint8_t length) {
    uint8_t cmd = NRF_CMD_WRITE_TX_PAYLOAD;
    nrf_spi_start();
    NRF_SPI_TransmitReceive(&cmd, 0, 1);
    NRF_SPI_Write(data, length);
    nrf_spi_end();
}

void _nrf_write_tx_payload_no_ack(const uint8_t *data, uint8_t length) {
    uint8_t cmd = NRF_CMD_WRITE_TX_PAYLOAD_NO_ACK;
    nrf_spi_start();
    NRF_SPI_TransmitReceive(&cmd, 0, 1);
    NRF_SPI_Write(data, length);
    nrf_spi_end();
}

void _nrf_write_ack_payload(NRF_PIPE pipe, const uint8_t *data, uint8_t length) {
    uint8_t cmd = NRF_CMD_WRITE_ACK_PAYLOAD | (pipe & 0x7);
    nrf_spi_start();
    NRF_SPI_TransmitReceive(&cmd, 0, 1);
    NRF_SPI_Write(data, length);
    nrf_spi_end();
}

void _nrf_read_rx_payload(uint8_t *data, uint8_t length) {
    uint8_t cmd = NRF_CMD_READ_RX_PAYLOAD;
    nrf_spi_start();
    NRF_SPI_TransmitReceive(&cmd, 0, 1);
    NRF_SPI_Read(data, length);
    nrf_spi_end();
}

/* 0x1C-0x1D -----------------------------------------------------*/
/**
 * @brief	Enable Dynamic Payload Length
 */
void _nrf_enable_pipe_dlp(uint8_t pipes) {
    uint8_t tmp = rf_read_reg_byte(NRF_REG_DYNPD);
	if ((tmp & (pipes)) != (pipes)) {
		tmp |= (pipes);
		nrf_write_reg_byte(NRF_REG_DYNPD, tmp);	//Update if we need
	}
}

void _nrf_enable_features(uint8_t features) {
    uint8_t tmp = rf_read_reg_byte(NRF_REG_FEATURE);
	if ((tmp & (features)) != (features)) {
		tmp |= (features);
		nrf_write_reg_byte(NRF_REG_FEATURE, tmp);	//Update if we need
	}
}

/* Commands -----------------------------------------------------*/
void _nrf_flush_tx(void) {
    uint8_t cmd = NRF_CMD_FLUSH_TX;
    nrf_spi_start();
    NRF_SPI_Write(&cmd, 1);
    nrf_spi_end();
}

void _nrf_flush_rx(void) {
    uint8_t cmd = NRF_CMD_FLUSH_RX;
    nrf_spi_start();
    NRF_SPI_Write(&cmd, 1);
    nrf_spi_end();
}

void _nrf_flush_all(void) {
    _nrf_flush_rx();
    _nrf_flush_tx();
}


/*******************************************************************************
 * Private Function Definition
 *******************************************************************************/
/* NRF Register Write & Read -----------------------------------------------------*/
static void nrf_write_reg(uint8_t reg, const uint8_t *data, uint8_t len) {
    uint8_t cmd = NRF_CMD_WRITE_REGISTER | reg;

    nrf_spi_start();
    NRF_SPI_TransmitReceive(&cmd, 0, 1);
    NRF_SPI_Write(data, len);
    nrf_spi_end();
}

static void nrf_write_reg_byte(uint8_t reg, const uint8_t data) {
    nrf_write_reg(reg, &data, 1);
}

static void nrf_read_reg(uint8_t reg, uint8_t *data, uint8_t len) {
    uint8_t cmd = NRF_CMD_READ_REGISTER | reg;

    nrf_spi_start();
    NRF_SPI_TransmitReceive(&cmd, 0, 1);
    NRF_SPI_Read(data, len);
    nrf_spi_end();
    
}

static uint8_t nrf_read_reg_byte(uint8_t reg) {
    uint8_t data;
    nrf_read_reg(reg, &data, 1);
    return data;
}
/* NRF SPI Communication Utils -----------------------------------------------------*/
static void nrf_spi_start(void) {
    HAL_GPIO_WritePin(NRF_SPI_CSN_GPIO_PORT, NRF_SPI_CSN_PIN, 0);
    NRF_SPI_Delay();
}

void nrf_spi_end(void) {
    NRF_SPI_Delay();
    HAL_GPIO_WritePin(NRF_SPI_CSN_GPIO_PORT, NRF_SPI_CSN_PIN, 1);
}

void nrf_spi_delay(void) {
    for (volatile int i = 0; i<5; i++) {
        UNUSED(i);
    }
}
