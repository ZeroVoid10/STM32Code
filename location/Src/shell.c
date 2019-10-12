#include "shell.h"
#include "cmd_func.h"
#include "gyro_acc.h"
#include "flag.h"

#include "tim.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//TODO: need to move to proper file or delete

/* Remeber to add , in pre line */
static struct cmd_struct cmd_list[] = {
    ADD_CMD("help", "Help", cmd_help),
    ADD_CMD("pwm", "-f/d <Param>: set frequence or duty", cmd_pwm),
    ADD_CMD("s", "stop wave", cmd_stop_wave),
    ADD_CMD("speed", "set mtr speed", cmd_set_speed),
    ADD_CMD("f", "", cmd_p),
    ADD_CMD("angle", "get angle", cmd_angle)
    //ADD_CMD("load", "load <Param>: move steering", cmd_load)
};

const char UART_RX_Alert[] = "\r\nCommand to long!\r\n";
const char *promptStr = "[zerovoid@STM32F103C]# ";
const char *delim = " \0";

//TODO: change UART_RX_Buffer to cmd_line
char buffer_rx_temp;
char UART_RX_Buffer[RX_BUFFER_SIZE] = {0};
int UART_RX_Count = 0;

char *cmd_argv[MAX_ARGC] = {0};

// shell.c private 
void shellBackspace(void);
void readCmd(char *cmd);

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
        #ifdef ECHO 
        uprintf(&buffer_rx_temp);
        #endif
    }
}

void prompt(void) {
    uprintf(promptStr);
}

void shell_exe(void) {
    int argc;
    #ifdef ECHO
    uprintf("\n");
    #endif
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
    #ifdef ECHO
    prompt();
    #endif
}

void shell_check(void) {
    echoInput();
    if (UART_RX_Buffer[UART_RX_Count-1] == '\r') {
        run_shell_flag = 1;
    }
}

void shellBackspace() {
    if(UART_RX_Count != 0)
    {
        UART_RX_Count--;
        #ifdef ECHO
        uprintf("\b \b");
        #endif
    }

}

void shell_init(void) {
    #ifdef ECHO
    prompt();
    #endif
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