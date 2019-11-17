#include "shell.h"
#include <string.h>
#include "train716.h"
#include "usart.h"
#include "tim.h"
#include <stdlib.h>
#include <ctype.h>

#define RX_BUFFER_SIZE 256
#define NUM_CMD 4
#define CMD_SIZE 16
#define PRSCRALER 7200

char buffer_rx_temp;
char UART_RX_Buffer[RX_BUFFER_SIZE] = {0};
int UART_RX_Count = 0;
char UART_RX_Alert[] = "\r\nCommand to long!\r\n";
char pref[] = "\nReturn message:";
char promptStr[] = "[zerovoid@STM32F103C]# ";
char delim[] = " \0";
char cmdList[NUM_CMD][CMD_SIZE] = {
    "help",
    "LED",
    "stopLED",
    "pwm"
};

void shellBackspace(void);
void pwmControl(void);

/* Read input & echo input to screen. */
void echoInput(void)
{
    if (UART_RX_Count == RX_BUFFER_SIZE)
    {
        memset(UART_RX_Buffer, 0, RX_BUFFER_SIZE);
        UART_RX_Count = 0;
        uprintf(UART_RX_Alert);
    } else if (buffer_rx_temp == '\b'){
        shellBackspace();
    } else{
        UART_RX_Buffer[UART_RX_Count++] = buffer_rx_temp;
        uprintf(&buffer_rx_temp);
    }
}

void prompt(void)
{
    uprintf(promptStr);
}

void TwLED()
{
    char *hz = strtok(NULL, delim);
    //uprintf("hz %s\r\n", hz);  
    if (hz == NULL)
    {
        //uprintf("hz is NULL\r\n");
        HAL_TIM_Base_Start_IT(&htim2);
    }
    float hzlf = 0.0F;
    if (isdigit(hz[0]) && (hzlf = atof(hz)) != 0)
    {
        uprintf("hzlf %f\r\n", hzlf);
        TIM2->ARR = (uint16_t)(PRSCRALER/hzlf-1);
        HAL_TIM_Base_Start_IT(&htim2);
    } else {
        uprintf("Error args\r\n");
    }
}

float freq = 1000;
int duty = 50;
void pwmControl(void)
{
    char *arg = strtok(NULL, delim);
    char *num;
    while(arg)
    {
        if (strcmp(arg, "-f") == 0)
        {
            num = strtok(NULL, delim);
            if (num && isdigit(num[0])) {
                freq = atof(num);
                uprintf("Change frequence to %f", freq);
                TIM3->ARR = (uint16_t) (100000/freq-1);
                uprintf("(arr %d)\r\n", TIM3->ARR);
                TIM3->CCR1 = (uint16_t) (duty*(TIM3->ARR + 1)/100);
            }
        } else if (strcmp(arg, "-d") == 0) {
            num = strtok(NULL, delim);
            if (num && isdigit(num[0]) && atoi(num) < 100) {
                duty = atoi(num);
                uprintf("Change duty to %d", duty);
                TIM3->CCR1 = (uint16_t) (duty*(TIM3->ARR + 1)/100);
                uprintf("(CCR1 %d)\r\n", TIM3->CCR1);
            }
        } else if (strcmp(arg, "-stop") == 0) {
            uprintf("pwm stop\r\n");
            HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
            return;
        } else if (strcmp(arg, "-s") == 0) {
            uprintf("freq = %f, duty = %d\r\n", freq, duty);
        }
        arg = strtok(NULL, delim);
    }
    uprintf("strat pwm\r\n");
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void startShell(void)
{
    echoInput();
    if (UART_RX_Buffer[UART_RX_Count-1] == '\r')
    {
        uprintf("\n");
        UART_RX_Buffer[UART_RX_Count-1] = '\0';
        readCmd(UART_RX_Buffer);
        memset(UART_RX_Buffer, 0, RX_BUFFER_SIZE);
        UART_RX_Count = 0;
        prompt();
    }
}

void shellBackspace()
{
    if(UART_RX_Count != 0)
    {
        UART_RX_Count--;
        uprintf("\b \b");
    }

}

void shellInit(void)
{
    prompt();
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&buffer_rx_temp, 1);
}

void readCmd(char *cmd)
{
    char *token = strtok(cmd, delim);
    int i = 0;
    int cmdNum = 0;
    for(i=0; i<NUM_CMD; i++)
    {
        if (strcmp(cmdList[i], token) == 0)
        {
            cmdNum = i;
            break;
        }
    }
    if (i==NUM_CMD)
        uprintf("Command not find.\r\n");
    switch(cmdNum)
    {
        case 0:
        uprintf("Help: \r\n");
        //uprintf("%d", cmd[0]);
        break;

        case 1:
        uprintf("Light LED\r\n");
        TwLED();
        break;

        case 2:
        uprintf("Stop LED\r\n");
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        HAL_TIM_Base_Stop_IT(&htim2);
        break;

        case 3:
        uprintf("pwm test\r\n");
        pwmControl();
        break;
    }
}