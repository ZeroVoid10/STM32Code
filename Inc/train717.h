#ifndef __TRAIN717_H__
#define __TRAIN717_H__

#ifdef __cplusplus
extern "C" {
#endif

char buffer_rx_temp;
char UART_RX_Buffer[256] = {0};
int UART_RX_Count = 0;
char UART_RX_Alert[] = "Alert";
char pref[] = "\nReturn message:";

void interupterTest(void);
void toggleLED(void);
void GPIO_Interrupt_Test(uint16_t GPIO_Pin);
void USART_Interrupt_Test(UART_HandleTypeDef *huart);
  
#ifdef __cplusplus
}
#endif

#endif /* __TRAIN717_H__ */
