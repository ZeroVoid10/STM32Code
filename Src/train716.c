/**
 * 2019/07/19 机器人对培训作业代码
 */
#include "main.h"
#include "usart.h"
#include "gpio.h"

#include "train716.h"
#include <string.h>
#include <stdarg.h>

void lightLED(void)
{
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
  HAL_Delay(1000);
}

void keyLightLED(void)
{
  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET)
  {
    HAL_Delay(20);
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET)
    {
      while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET);
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      uprintf("Hello, world!\n");
      uprintf("%d %d\n", 5, 10);
    }
  }
}

void transmitHello(void)
{
  uint8_t data[] = "Hello world!";
  HAL_UART_Transmit(&huart1, data, strlen((char*)data), 0xffff);
  HAL_Delay(1000);

}

void keyTransmitHello(void)
{
  char data[] = "Hello world.\n";
  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET)
  {
    HAL_Delay(20);
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET)
    {
      while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET);
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      HAL_UART_Transmit(&huart1, (uint8_t *)data, sizeof(data), 0xffff);
    }
  }
}

void uprintf(char *fmt, ...)
{
  char buffe[256];
  va_list args;
  va_start(args, fmt);
  vsprintf(buffe, fmt, args);
  va_end(args);
  HAL_UART_Transmit(&huart1, (uint8_t *)buffe, strlen(buffe), 0xffff);
}

void uprintfTest(void)
{
  if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET)
    {
      HAL_Delay(20);
      if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET)
      {
        while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET);
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        uprintf("Hello, world!\n");
        uprintf("%d %d\n", 5, 10);
      }
    }
}