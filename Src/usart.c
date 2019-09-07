/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdarg.h>
#include <string.h>
#include "tim.h"
#include "shell.h"

/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */
void uprintf(const char *fmt, ...) {
    char buffer[IO_BUFFER_SIZE] = {0};
    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end(args);
    HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), 0xffff);
}

char s[30]={'b','y',16,6};
void send_wave(float arg1,float arg2,float arg3,float arg4) {
    s[20]='\r';
    s[21]='\n';
    memcpy(s+4,&arg1,sizeof(arg1));
    memcpy(s+8,&arg2,sizeof(arg1));
    memcpy(s+12,&arg3,sizeof(arg1));
    memcpy(s+16,&arg4,sizeof(arg1));
    HAL_UART_Transmit(&huart1,(uint8_t *)s,sizeof(s),2000);
}

void send_wave6(float *arg1, float* arg2) {
  s[28] = '\r';
  s[29] = '\n';
  memcpy(s+4,arg1, 4);
  memcpy(s+8, arg2, 4);

  memcpy(s+12, arg1+1, 4);
  memcpy(s+16, arg2+1, 4);

  memcpy(s+20, arg1+2, 4);
  memcpy(s+24, arg2+2, 4);
  HAL_UART_Transmit(&huart1,(uint8_t *)s,sizeof(s),2000);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  
  if (huart->Instance == USART1) {
    extern char buffer_rx_temp;
    extern char UART_RX_Buffer[RX_BUFFER_SIZE];
    /**
    if (TIM1->DIER == 1){
      if (buffer_rx_temp == 's') {
          HAL_TIM_Base_Stop_IT(&htim1);
          //uprintf("\nstop TIM1\r\n");
          HAL_UART_Receive_IT(&huart1, (uint8_t *)&buffer_rx_temp, 1);
          memset(UART_RX_Buffer, 0, RX_BUFFER_SIZE);
          prompt();
      }
    } else {*/
      startShell();
      HAL_UART_Receive_IT(&huart1, (uint8_t *)&buffer_rx_temp, 1);
    //}
  } 
  /*
  if (huart->Instance == USART1) {
    startShell();
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&buffer_rx_temp, 1);
  }*/
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
