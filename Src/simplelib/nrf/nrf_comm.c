/*******************************************************************************
 * Copyright:		BUPT
 * File Name:		nrf_comm.c
 * Description:		NRF
 * Author:			ZeroVoid
 * Version:			0.1
 * Data:			2019/11/08 Fri 21:06
 *******************************************************************************/
#include "nrf_comm.h"
#include "nrf24l01.h"
#include "cmd.h"

void nrf_rx_date_process(void) {
    
    uprintf_to(&huart1, (char*)nrf_rx_data);
    nrf_send_data(nrf_tx_data, 32);
}