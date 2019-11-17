/*******************************************************************************
 * Copyright:		BUPT
 * File Name:		nrf_comm.c
 * Description:		NRF
 * Author:			ZeroVoid
 * Version:			0.1
 * Data:			2019/11/08 Fri 21:06
 *******************************************************************************/
#include "nrf_comm.h"
#ifdef SL_NRF_COMM

#ifdef SL_CMD
#include "cmd.h"
#endif // SL_CMD

#include <string.h>

uint8_t nrf_tx_buffer[99] = {0};
uint8_t nrf_tx_cnt = 0;
uint8_t nrf_tx_remainder = 0;
static uint8_t cnt = 0;

void _nrf_receive_callback(uint8_t *data, int len);
void _nrf_send_callback(void);
void _nrf_max_rt_callback(void);
void nrf_comm_cmd(NRF_Handle *handle);

/**
 * @brief	NRF 基于协议发送函数
 * @param   data        发送数据
 * @param   len         数据长度
 * @param   data_deal   NRF_UART / NRF_CAN / NRF_SPI 组合
 *          e.g. NRF_UART | NRF_CAN
 * @note    使用前设置to 和 from
 */
void nrf_comm_send(uint8_t *data, int len) {
    // nrf_handle.nrf_data_from = NRF_SPI;
    // nrf_handle.nrf_data_to = data_deal;
    nrf_tx_cnt = len/(32 - NRF_PCK_HEADER_SIZE) + 1;
    nrf_tx_data[1] = nrf_tx_cnt;
    nrf_tx_remainder = len%(32 - NRF_PCK_HEADER_SIZE);
    for (cnt = 0; cnt < nrf_tx_cnt - 1; cnt++) {
        nrf_tx_data_lock();
        memcpy(nrf_tx_data + NRF_PCK_HEADER_SIZE, nrf_tx_buffer + NRF_PCK_SIZE*cnt, NRF_PCK_SIZE);
        _nrf_comm_send(nrf_tx_data, 32);
        while(tx_locked);
    }
    nrf_tx_data_lock();
    memcpy(nrf_tx_data + NRF_PCK_HEADER_SIZE, nrf_tx_buffer + NRF_PCK_SIZE*cnt, nrf_tx_remainder);
    _nrf_comm_send(nrf_tx_data, NRF_PCK_HEADER_SIZE + nrf_tx_remainder);
    while(tx_locked);
}

void nrf_main(void) {
    switch(nrf_flow_state) {
    case NRF_IRQ:
        // nrf_irq_handle();
        break;

    case NRF_COMM_SEND:
        // _nrf_comm_send(nrf_handle.tx_data, nrf_handle.tx_len);
        nrf_comm_send(nrf_tx_buffer, nrf_handle.tx_len);
        nrf_flow_state = NRF_IDLE;
        break;

    case NRF_RX_CALLBACK:
        _nrf_receive_callback(nrf_handle.rx_data, nrf_handle.rx_len);
        nrf_flow_state = NRF_IDLE;
        break;
    case NRF_TX_CALLBACK:
        _nrf_send_callback();
        memset(nrf_tx_data, 0, 32);
        nrf_flow_state = NRF_IDLE;
        break;
    case NRF_MAX_RT_CALLBACK:
        _nrf_max_rt_callback();
        memset(nrf_tx_data, 0, 32);
        nrf_flow_state = NRF_IDLE;
        break;
    
    default:
        break;
    }
}


void _nrf_comm_send(uint8_t *data, int len) {
    data[0] = (nrf_handle.nrf_data_from << 4) | nrf_handle.nrf_data_to;
    nrf_handle.tx_data = data;
    nrf_handle.tx_len = len;
    nrf_send_data(data, len);
}

void _nrf_receive_callback(uint8_t *data, int len) {
    // uint8_t cnt = (data[0])
    uint8_t deal_method = (data[0] & 0x0F);
    if (deal_method & NRF_UART) {
        #ifdef SL_CMD
        uprintf_to(&huart1, (char*)(nrf_rx_data + NRF_PCK_HEADER_SIZE));
        // uprintf((char*)(nrf_rx_data_buffer + NRF_PCK_HEADER_SIZE));
        // pCMD_USART->gState = HAL_UART_STATE_READY;
        // HAL_UART_Transmit_DMA(pCMD_USART, nrf_rx_data + NRF_PCK_HEADER_SIZE, len);
        // while(pCMD_USART->hdmatx->State != HAL_DMA_STATE_READY);
        #endif // SL_CMD
        nrf_uart_receive_callback(data + NRF_PCK_HEADER_SIZE, len - NRF_PCK_HEADER_SIZE);
        HAL_GPIO_TogglePin(IND_LED_GPIO_Port, IND_LED_Pin);
    }
    if (deal_method & NRF_CAN) {
        nrf_can_receive_callback(data + NRF_PCK_HEADER_SIZE, len - NRF_PCK_HEADER_SIZE);
    }
    if (deal_method & NRF_SPI) {
        nrf_comm_cmd(&nrf_handle);
        nrf_spi_receive_callback(data + NRF_PCK_HEADER_SIZE, len - NRF_PCK_HEADER_SIZE);
    }
    #ifdef SL_NRF_DEBUG
    // HAL_GPIO_TogglePin(IND_LED_GPIO_Port, IND_LED_Pin);
    // HAL_GPIO_WritePin(IND_LED_GPIO_Port, IND_LED_Pin, GPIO_PIN_RESET);
    #endif // SL_NRF_DEBUG
    nrf_receive_callback(nrf_handle.rx_data, nrf_handle.rx_len);
}
void nrf_comm_cmd(NRF_Handle *handle) {
    uint8_t arg = handle->rx_data[NRF_PCK_HEADER_SIZE]&0x0F;
    uint8_t cmd = handle->rx_data[NRF_PCK_HEADER_SIZE] >> 4;
    switch (cmd) {
    case NRF_COMM_CMD_ALL_CAN:
        nrf_all_can_send = arg;
        break;
    
    default:
        break;
    }
}

void _nrf_send_callback(void) {
    #ifdef SL_NRF_DEBUG
    // HAL_GPIO_TogglePin(IND_LED_GPIO_Port, IND_LED_Pin);
    // HAL_GPIO_WritePin(IND_LED_GPIO_Port, IND_LED_Pin, GPIO_PIN_SET);
    #endif // SL_NRF_DEBUG
    nrf_send_callback(nrf_handle.tx_data, nrf_handle.tx_len);
}

void _nrf_max_rt_callback(void) {
    #ifdef SL_NRF_DEBUG_
    HAL_GPIO_TogglePin(IND_LED_GPIO_Port, IND_LED_Pin);
    #endif // SL_NRF_DEBUG
    nrf_max_rt_callback(nrf_handle.tx_data, nrf_handle.tx_len);
}

__weak void nrf_spi_receive_callback(uint8_t *data, int len) {}
__weak void nrf_can_receive_callback(uint8_t *data, int len) {}
__weak void nrf_uart_receive_callback(uint8_t *data, int len) {}
__weak void nrf_receive_callback(uint8_t *data, int len) {}
__weak void nrf_send_callback(uint8_t *data, int len) {}
__weak void nrf_max_rt_callback(uint8_t *data, int len) {}

#ifdef SL_NRF_HW_CAN

void _can_rx_nrf_callback(uint32_t *id, can_msg *data) {
    nrf_handle.nrf_data_from = NRF_CAN;
    nrf_handle.nrf_data_to = NRF_UART | NRF_SPI;
    nrf_handle.tx_len = 9 + NRF_PCK_HEADER_SIZE;
    strncpy((char*)(nrf_handle.tx_data + NRF_PCK_HEADER_SIZE), (char*)id, 4);
    strncpy((char*)(nrf_handle.tx_data + NRF_PCK_HEADER_SIZE + 4), (char*)data, 8);
    _nrf_comm_send(nrf_handle.tx_data, nrf_handle.tx_len);
}

#endif // SL_NRF_HW_CAN

#endif // SL_NRF_COMM