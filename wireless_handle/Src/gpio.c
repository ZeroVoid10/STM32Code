/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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
#include "gpio.h"
/* USER CODE BEGIN 0 */
#include "nrf24l01.h"

#define BUTTON_STATE_POS 11
#define BUTTON_DOWN 0
#define BUTTON_UP 1
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
//�����ɿ�����־λ
GPIO_PinState lastbutton_1 = GPIO_PIN_SET;
GPIO_PinState lastbutton_2 = GPIO_PIN_SET;
GPIO_PinState lastbutton_3 = GPIO_PIN_SET;
GPIO_PinState lastbutton_4 = GPIO_PIN_SET;
GPIO_PinState lastbutton_5 = GPIO_PIN_SET;
GPIO_PinState lastbutton_6 = GPIO_PIN_SET;
GPIO_PinState lastbutton_7 = GPIO_PIN_SET;
GPIO_PinState lastbutton_8 = GPIO_PIN_SET;
GPIO_PinState lastbutton_9 = GPIO_PIN_SET;
GPIO_PinState lastbutton_10 = GPIO_PIN_SET;

int exit_state = 0;
int dial_state = 0;
/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|NRF_CE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(NRF_CS_GPIO_Port, NRF_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC0 PC6 PC7 PC9 
                           PC10 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9 
                          |GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC1 PC2 PC3 PC4 
                           PC5 PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4 
                          |GPIO_PIN_5|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PBPin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|NRF_CE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = NRF_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(NRF_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = NRF_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(NRF_IRQ_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */
//������ס�������ƺ���
//���������½��ص��жϻ���Ϊ������������ʧ�����⣬��˱�����ʹ�ñ�־λ
//��ס��һ�ε�pin״̬���ֶ��ж������غ��½��أ�ȷ��������ȫ��
uint16_t button_state = 0;
void gpio_delayed_button_ctrl(GPIO_PinState* last, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, int num) {

    uint8_t temp[10] = {0};
    if (*last == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET) {
        /*
          str[0] = '0' + dial_state;
          str[1] = '0' + num;
          str[2] = 'd';
          */
        button_state |= 1 << num;
        button_state |= BUTTON_DOWN << BUTTON_STATE_POS;
        //can_send_msg(325, str,3);
        *last = GPIO_PIN_SET;
        memcpy(temp + 8, &button_state, 2);
        //NRF_Install_TX_Data(temp, 10);
        //NRF_Send_Message_IT();
        nrf_send_data(temp, 10);
        //nrf_send_data(temp, 10, false);
        button_state = 0;
    } else if (*last == GPIO_PIN_SET && HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET) {
        /*
          str[0] = '0' + dial_state;
          str[1] = '0' + num;
          str[2] = 'u';
          uprintf("%s\n",str);
          */
        button_state |= 1 << num;
        button_state |= BUTTON_UP << BUTTON_STATE_POS;
        //can_send_msg(325, str,3);
        *last = GPIO_PIN_RESET;
        memcpy(temp + 8, &button_state, 2);
        //NRF_Install_TX_Data(temp, 10);
        //NRF_Send_Message_IT();
        nrf_send_data(temp, 10);
        //nrf_send_data(temp, 10, false);
        button_state = 0;
    }
}

void gpio_read_dial() {
    int tmp = 0;
    if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_3) == GPIO_PIN_SET)
        button_state |= 1 << 15;
    //tmp += 1;
    if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_4) == GPIO_PIN_SET)
        button_state |= 1 << 14;
    //tmp += 2;
    dial_state = tmp;


}

/*
            6              0
         8     7        1     2
            9              3
               4        5
*/
//���Ӱ����������ƺ��������������main�����
void gpio_delayed_button() {
    gpio_read_dial();
    gpio_delayed_button_ctrl(&lastbutton_1, GPIOC, GPIO_PIN_0, 0);   //����1
    gpio_delayed_button_ctrl(&lastbutton_2, GPIOC, GPIO_PIN_1, 1);   //����2
    gpio_delayed_button_ctrl(&lastbutton_3, GPIOC, GPIO_PIN_2, 2);   //����3
    gpio_delayed_button_ctrl(&lastbutton_4, GPIOC, GPIO_PIN_3, 3);   //����4
    gpio_delayed_button_ctrl(&lastbutton_5, GPIOC, GPIO_PIN_4, 4);   //����3
    gpio_delayed_button_ctrl(&lastbutton_6, GPIOC, GPIO_PIN_5, 5);   //����4
    gpio_delayed_button_ctrl(&lastbutton_7, GPIOC, GPIO_PIN_6, 6);   //����5
    gpio_delayed_button_ctrl(&lastbutton_8, GPIOC, GPIO_PIN_7, 7);   //����6
    gpio_delayed_button_ctrl(&lastbutton_9, GPIOC, GPIO_PIN_8, 8);   //����6
    gpio_delayed_button_ctrl(&lastbutton_10, GPIOC, GPIO_PIN_9, 9);  //����6
}

//void exit_button()
//{
//    switch(exit_state)
//    {
//    case 1:
//        //nrf_send("0",1);
//        can_send_msg(325,"0",1);
//        exit_state = 0;
//        break;
//    case 2:
//        //nrf_send("1",1);
//        can_send_msg(325,"1",1);
//        exit_state = 0;
//        break;
//    case 3:
//        //nrf_send("4",1);
//        can_send_msg(325,"4",1);
//        exit_state = 0;
//        break;
//    case 4:
//        //nrf_send("3",1);
//        can_send_msg(325,"3",1);
//        exit_state = 0;
//        break;
//    case 5:
//        //nrf_send("2",1);
//        can_send_msg(325,"2",1);
//        exit_state = 0;
//        break;
//
//    }
//}

//�ⲿ�жϵ�callback��PIN_x��Ӧÿһ����������������
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//  if(GPIO_Pin == GPIO_PIN_1)
//  {
//      //can_send_msg(325, "0",1);
//      exit_state = 1;
//  }
//  else if(GPIO_Pin == GPIO_PIN_2)
//  {
//     // can_send_msg(325, "1",1);
//        exit_state = 2;
//  }
//  else if(GPIO_Pin == GPIO_PIN_3)
//  {
//     // can_send_msg(325, "4",1);
//    exit_state = 3;
//  }
//  else if(GPIO_Pin == GPIO_PIN_4)
//  {
//      //can_send_msg(325, "3",1);
//    //exit_state = 4;
//  }
//  else if(GPIO_Pin == GPIO_PIN_5)
//  {
//      //can_send_msg(325, "2",1);
//    //exit_state = 5;
//  }
//  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
//}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
