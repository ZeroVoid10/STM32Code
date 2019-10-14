/*******************************************************************************
 * Copyright:		BUPT
 * File Name:		simplelib_config.h
 * Description:		SimpleLib: 兼容STM32CubeMX生成项目的组件Lib
 * Version:			0.1.1
 * Author:			ZeroVoid
 * Data:			2019/09/23 Mon 14:30
 * Encoding:        UTF-8
 *******************************************************************************/
/*******************************************************************************
 * LOG
 * Last Modified:	2019/10/13 Sun 22:55
 * Author:			ZeroVoid
 *******************************************************************************/
#ifndef __SIMPLELIB_CONFIG_H
#define __SIMPLELIB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * SimpleLib Configuration
 * @note	SL Simplelib or Stm32Lib
 * @note    vscode comment shortcut: ctrl + /
 *******************************************************************************/
#define SL_CAN                          // CAN通信模块
#define SL_CMD_USART                    // 串口通信模块
#define SL_CMD_USART_DMA                // 串口通信模块，使用DMA传输
// #define SL_NRF                          // NRF通信模块

#define SL_DEBUG                        // Simplelib debug macro

/* NRF Configuration -----------------------------------------------------*/
#ifdef SL_NRF
// TODO: ZeroVoid	due:10/14	模块化NRF模块

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