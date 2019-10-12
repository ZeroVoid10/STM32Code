#include "cmd_func.h"
#include "tim.h"
#include "mtr.h"
#include "usart.h"
#include "flag.h"
#include "gyro_acc.h"

#include <string.h>
#include <stdlib.h>

void cmd_set_speed(int argc, char *argv[]) {
    if (argc != 3) {
        uprintf("! args\r\n");
        return;
    }
    int speed = 0;
    if (strcmp(argv[1], "1") == 0) {
        speed = atoi(argv[2]);
        set_speed(MTR_J1, speed);
    } else if(strcmp(argv[1], "2") == 0) {
        speed = atoi(argv[2]);
        set_speed(MTR_J2, speed);
    }
}

void cmd_pwm(int argc, char *argv[]) {
    int index;
    uint32_t channel;
    TIM_HandleTypeDef htim = htim3;
    int freq = 16000;
    int duty = 0;

    if (strcmp(argv[1], "1") == 0) {
        channel = TIM_CHANNEL_1;
    } else if (strcmp(argv[1], "2") == 0) {
        channel = TIM_CHANNEL_1;
        htim = htim4;
    } else if (strcmp(argv[1], "3") == 0) {
        channel = TIM_CHANNEL_3;
    } else if (strcmp(argv[1], "4") == 0) {
        channel = TIM_CHANNEL_4;
    } else {
        uprintf("Error: channel choosed\r\n");
        return;
    }
    for (index = 2; index < argc; index++) {
        if (strcmp(argv[index], "-f") == 0) {
            freq = atoi(argv[++index]);
        } else 
        if (strcmp(argv[index], "-d") == 0) {
            duty = atoi(argv[++index]);
        } else
        if (strcmp(argv[index], "-s") == 0) {
            HAL_TIM_PWM_Stop(&htim, channel);
            return;
        }
    }
    _set_freq_duty(htim, channel, freq, duty);
    HAL_TIM_PWM_Start(&htim, channel);
}

void cmd_stop_wave(int argc, char *argv[]) {
    show_angle_flag = 0x00000000U;
    uprintf("stop\r\n");
}

void cmd_angle(int argc, char *argv[]) {
    if (argc == 1) {
        get_acc_angle(last_angle);
        show_angle_flag = EST_ANGLE_FLAG;
    }
    
    if (strcmp(argv[1], "a") == 0) {
        show_angle_flag = ACC_ANGLE_FLAG;
    } else if (strcmp(argv[1], "g") == 0) {
        get_acc_angle(last_angle);
        show_angle_flag = GYRO_ANGLE_FLAG;
    }
    uprintf("ok\r\n");
}

void cmd_init_icm(int argc, char *argv[]) {
    //ICM_Init();
}

void cmd_show_acc(int argc, char *argv[]) {

}

void cmd_show_angle_speed(int argc, char *argv[]) {
}