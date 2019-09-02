#include "shell.h"
#include "zerostm32.h"

#include "usart.h"
#include "tim.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pid.h"
#include "can.h"
#include "cmd_func.h"

//TODO: need to move to proper file or delete
#define NUM_CMD 8
#define CMD_SIZE 16
#define PRSCRALER 7200
#define MAX_CMD_NUM 10

/* Remeber to add , in pre line */
static struct cmd_struct cmd_list[] = {
    ADD_CMD("help", "Help", cmd_help),
    ADD_CMD("move", "-f/b/gl/gr/tl/tr <Param> or x/y/z <Param>", cmd_move),
    ADD_CMD("mtr", "Set motor duty or speed: <motor_ID> -d/s <Param>", cmd_move_mtr_set),
    ADD_CMD("s", "Stop all motor", cmd_stop),
    ADD_CMD("ss", "stop set duty 0", cmd_stop_duty),
    ADD_CMD("set", "Set max_speed/max_spin_speed/spin_coe/x/y/s/h or -s show befor variables' value", cmd_set),
    ADD_CMD("rocker", "Use serial port replace rocker CAN communicate", cmd_rocker),
    ADD_CMD("can", "Set can mode -n/l normal/loopback", cmd_set_can_mode),
    ADD_CMD("pwm", "-f/d <Param>: set frequence or duty", cmd_pwm),
    ADD_CMD("load", "load <Param>: move steering", cmd_load),
    //ADD_CMD("shoot", "", cmd_shoot),
    ADD_CMD("brushless", "", cmd_brushless)
};

const char UART_RX_Alert[] = "\r\nCommand to long!\r\n";
const char *promptStr = "[zerovoid@STM32F103C]# ";
const char *delim = " \0";

//TODO: change UART_RX_Buffer to cmd_line
char buffer_rx_temp;
char UART_RX_Buffer[RX_BUFFER_SIZE] = {0};
int UART_RX_Count = 0;

char *cmd_argv[MAX_ARGC] = {0};

// shell.c private 函数声明
void shellBackspace(void);
void readCmd(char *cmd);

/*TODO: 移到集中放置命令函数的文件内 */
void TwLED(void);
void pwmControl(TIM_TypeDef * TIM, TIM_HandleTypeDef htim, uint32_t channel, uint32_t ocmode);
char cmdList[NUM_CMD][CMD_SIZE] = {
    "help",
    "LED",
    "stopLED",
    "pwm",
    "pid",
    "mtrspeed ",
    "can",
    "speed"
};

void TwLED(void) {
    char *hz = strtok(NULL, delim);
    //uprintf("hz %s\r\n", hz);  
    if (hz == NULL)
    {
        //uprintf("hz is NULL\r\n");
        //HAL_TIM_Base_Start_IT(&htim2);
    }
    float hzlf = 0.0F;
    if (isdigit(hz[0]) && (hzlf = atof(hz)) != 0)
    {
        uprintf("hzlf %f\r\n", hzlf);
        //TIM2->ARR = (uint16_t)(PRSCRALER/hzlf-1);
        //HAL_TIM_Base_Start_IT(&htim2);
    } else {
        uprintf("Error args\r\n");
    }
}

/*
int freq = 1000;
int duty = 50;
void pwmControl(TIM_TypeDef * TIM, TIM_HandleTypeDef htim, uint32_t channel, uint32_t ocmode) {
    char *arg = strtok(NULL, delim);
    char *num;
    uint32_t ccrtemp;
    while(arg)
    {
        if (strcmp(arg, "-f") == 0)
        {
            num = strtok(NULL, delim);
            if (num && isdigit(num[0])) {
                freq = atoi(num);
                
                TIM->ARR = (uint32_t) (1000000/freq-1);
                uprintf("(arr %d)\r\n", TIM->ARR);
                ccrtemp = (uint32_t) (((ocmode == TIM_OCPOLARITY_HIGH)? duty:(100-duty))*(TIM->ARR + 1)/100);
                if (channel == TIM_CHANNEL_1)
                    TIM->CCR1 = ccrtemp;
                else if (channel == TIM_CHANNEL_2)
                    TIM->CCR2 = ccrtemp;
                
                setFreq(htim, channel, freq, duty);
            }
        } else if (strcmp(arg, "-d") == 0) {
            num = strtok(NULL, delim);
            if (num && isdigit(num[0]) && atoi(num) < 100) {
                duty = atoi(num);
                uprintf("Change duty to %d", duty);
                ccrtemp = (uint32_t) (((ocmode == TIM_OCPOLARITY_HIGH)? duty:(100-duty))*(TIM->ARR + 1)/100 - 1);
                if (channel == TIM_CHANNEL_1) {
                    TIM->CCR1 = ccrtemp;
                    uprintf("(CCR1 %d)\r\n", TIM->CCR1);
                }
                else if (channel == TIM_CHANNEL_2) {
                    TIM->CCR2 = ccrtemp;
                    uprintf("(CCR2 %d)\r\n", TIM->CCR2);
                }
            }
        } else if (strcmp(arg, "-stop") == 0) {
            uprintf("pwm stop\r\n");
            HAL_TIM_PWM_Stop(&htim, channel);
            return;
        } else if (strcmp(arg, "-s") == 0) {
            uprintf("freq = %f, duty = %d\r\n", freq, duty);
        }
        arg = strtok(NULL, delim);
    }
    uprintf("strat pwm\r\n");
    HAL_TIM_PWM_Start(&htim, channel);
}
*/
/* Shell relative code */
/* Read input & echo input to screen. */
void echoInput(void) {
    if (UART_RX_Count == RX_BUFFER_SIZE)
    {
        memset(UART_RX_Buffer, 0, RX_BUFFER_SIZE);
        UART_RX_Count = 0;
        uprintf(UART_RX_Alert);
    } else if (buffer_rx_temp == '\b'){
        shellBackspace();
    } else{
        UART_RX_Buffer[UART_RX_Count++] = buffer_rx_temp;
        //#ifdef OPEN_ECHO
        uprintf(&buffer_rx_temp);
        //#endif
    }
}

void prompt(void) {
    uprintf(promptStr);
}

void startShell(void) {
    int argc;
    echoInput();
    if (UART_RX_Buffer[UART_RX_Count-1] == '\r')
    {
        uprintf("\n");
        UART_RX_Buffer[UART_RX_Count-1] = '\0';
        //readCmd(UART_RX_Buffer);
        cmd_parse(UART_RX_Buffer, &argc, cmd_argv);
        if (argc != 0) {
            cmd_exec(argc, cmd_argv);
        } else {
            memset(UART_RX_Buffer, 0, RX_BUFFER_SIZE);
            UART_RX_Count = 0;
        }
        //memset(UART_RX_Buffer, 0, RX_BUFFER_SIZE);
        //UART_RX_Count = 0;
        prompt();
    }
}

void shellBackspace() {
    if(UART_RX_Count != 0)
    {
        UART_RX_Count--;
        uprintf("\b \b");
    }

}

void shell_init(void) {
    prompt();
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&buffer_rx_temp, 1);
}

void cmd_parse(char *cmd_line, int *argc, char *argv[]) {
    char *token = strtok(cmd_line, delim);
    int arg_index = 0;

    while(token) {
        argv[arg_index++] = token;
        token = strtok(NULL, delim);
    }
    *argc = arg_index;
}

int cmd_exec(int argc, char *argv[]) {
    int cmd_index = 0;
    int cmd_num = (int)(sizeof(cmd_list)/sizeof(cmd_list[0]));

    for (cmd_index = 0; cmd_index < cmd_num; cmd_index++) {
        if (strcmp(cmd_list[cmd_index].cmd_name, argv[0]) == 0) {
            uprintf("*****\r\n");
            cmd_list[cmd_index].cmd_func(argc, argv);
            UART_RX_Count = 0;
            memset(UART_RX_Buffer, 0, RX_BUFFER_SIZE);
            return 0;
        }
    }
    uprintf("Command %s not found.\r\n", argv[0]);
    memset(UART_RX_Buffer, 0, RX_BUFFER_SIZE);
    UART_RX_Count = 0;
    return -1;
}

void cmd_help(int argc, char *argv[]) {
    uprintf("Help: \r\n");
    int i, cmd_num = sizeof(cmd_list)/sizeof(cmd_list[0]);
    for (i = 0; i < cmd_num; i++) {
        uprintf("cmd:%s\r\n", cmd_list[i].cmd_name);
        uprintf("usage:%s\r\n", cmd_list[i].cmd_usage);
    }
}

void readCmd(char *cmd) {
    char *token = strtok(cmd, delim);
    int i = 0;
    int cmdNum = 0;
    char *tim_arg;
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
        //HAL_TIM_Base_Stop_IT(&htim2);
        break;

        case 3:
        uprintf("pwm test\r\n");
        tim_arg = strtok(NULL, delim);
        if (tim_arg) {
            if (strcmp(tim_arg, "3") == 0) {
                uprintf("TIM3 Channel 1\r\n");
                pwmControl(TIM3, htim3, TIM_CHANNEL_1, TIM_OCPOLARITY_LOW);
            } else if (strcmp(tim_arg, "4") == 0) {
                uprintf("TIM4 Channel 2\r\n");
                pwmControl(TIM4, htim4, TIM_CHANNEL_2, TIM_OCPOLARITY_HIGH);
            } else {
                uprintf("error TIM choose.\r\n");
            }
        }
        break;

        case 4:
        uprintf("pid control\r\n");
        PID_Control();
        break;

        case 5:
        detectSpeed();
        break;

        case 6:
        canSendTest();
        break;
        
        case 7:
        detectSpeed();
    }
}