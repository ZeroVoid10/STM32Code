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

void rx_date_process(void) {
    
    nrf_send_data(nrf_tx_data, 32);
}