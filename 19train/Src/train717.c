#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "stm32f1xx_it.h"

#include "train717.h"
#include "train716.h"
#include <string.h>

void toggleLED(void) 
{
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

void GPIO_Interrupt_Test(uint16_t GPIO_Pin) 
{
    //HAL_GPIO_EXTI_Callback function
    if (GPIO_Pin == EXTI_13_Pin)
    {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }
    else if (GPIO_Pin == EXTI_14_Pin)
    {
        uint8_t data[] = "Key checked!\r\n";
        HAL_UART_Transmit(&huart1, data, strlen((char*)data), 0xffff);
    }
}


extern char buffer_rx_temp;
extern char UART_RX_Buffer[256];
extern int UART_RX_Count;
extern char UART_RX_Alert[];
extern char pref[];
extern UART_HandleTypeDef huart1;
void USART_Interrupter_Test(UART_HandleTypeDef *huart) 
{
      if (huart->Instance == USART1) 
      {
        if (UART_RX_Count == 32) {
        memset(UART_RX_Buffer, 0, 32);
        UART_RX_Count = 0;
        uprintf(UART_RX_Alert);
        } else {
        UART_RX_Buffer[UART_RX_Count++] = buffer_rx_temp;
        uprintf(&buffer_rx_temp);
        if (UART_RX_Buffer[UART_RX_Count-1] == '\r') {
            uprintf(pref);
            uprintf(UART_RX_Buffer);
            uprintf("\n");
            HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
            memset(UART_RX_Buffer, 0, 32);
            UART_RX_Count = 0;
            }   
        }
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&buffer_rx_temp, 1);
    }
}