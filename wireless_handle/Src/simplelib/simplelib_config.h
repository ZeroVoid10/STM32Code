/*******************************************************************************
 * Copyright:		BUPT
 * File Name:		simplelib_config.h
 * Description:		配置文件
 * Author:			ZeroVoid
 * Version:			0.1
 * Data:			2019/09/23 Mon 14:30
 *******************************************************************************/
#ifndef __SIMPLELIB_CONFIG_H
#define __SIMPLELIB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// SL SimpleLib/Stm32Lib
#define SL_DEBUG
#define SL_CAN
#define SL_USART
#define SL_USART_USE_DMA // Unused
#define SL_NRF

#ifdef SL_NRF
#ifdef STM32F407xx 
#include "gpio.h"
#include "spi.h"

#define NRF_ADDR_COF					1 // 变化参数
#define NRF_SPI_Handle                  hspi3
#define NRF_SPI_CSN_PIN                 GPIO_PIN_15
#define NRF_SPI_CSN_GPIO_PORT           GPIOA
#define NRF_SPI_IRQ_PIN                 GPIO_PIN_8
#define NRF_SPI_IRQ_GPIO_PORT           GPIOB
#define NRF_SPI_CE_PIN                  GPIO_PIN_9
#define NRF_SPI_CE_GPIO_PORT            GPIOB
#endif // STM32F407xx

#endif // SL_NRF

#ifdef __cplusplus
}
#endif

#endif /* __SIMPLELIB_CONFIG_H */