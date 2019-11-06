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
 * @note    VSCode comment shortcut: ctrl + /
 *******************************************************************************/
// #define SL_OS                           // OS系统模块
#define SL_CAN                          // CAN通信模块
#define SL_CMD                          // 串口通信模块
#define SL_CMD_DMA                          // 串口通信DMA使用
#define SL_FLASH                        // Flash 模块
// #define SL_NRF                          // NRF通信模块
#define SL_MOTOR_DRIVER					// 驱动卡模块
#define SL_UTILS					    // 通用工具函封装

#define SL_DEBUG                        // Simplelib debug macro
#define SL_COMP_V01					    // 兼容V0.1版本

/* Motor Driver Configuration -----------------------------------------------------*/
#ifdef SL_MOTOR_DRIVER
#define EN_MOTOR_DRIVER                 // 使能HX 驱动卡
#define EN_VESC_MOTOR_DRIVER             // 使能Benjamin VESC驱动卡
#endif // SL_MOTOR_DRIVER

/* Command Configuration -----------------------------------------------------*/
#ifdef SL_CMD_DMA
#define CMD_MAX_LENGTH					100 // 指令最大长度
#endif // SL_CMD_DMA

/* Flash Configuration -----------------------------------------------------*/
#ifdef SL_FLASH
#define FLASH_SIZE                      10 // Flash float数组大小
#endif // SL_FLASH

/* NRF Configuration -----------------------------------------------------*/
#ifdef SL_NRF

#ifdef STM32F407xx 
#include "gpio.h"
#include "spi.h"
#define NRF_ADDR_COF					1 // 接收地址变化参数,方便测试
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