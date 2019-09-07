#include "mtr.h"
#include "usart.h"
#include "tim.h"

struct pwm_conf MTR1_FORWARD;
struct pwm_conf MTR1_BACKWARD;
struct pwm_conf MTR2_FORWARD;
struct pwm_conf MTR2_BACKWARD;

void set_channel_speed(struct pwm_conf conf, int speed);

void pwm_conf_init(void) {
    MTR1_FORWARD.htim = htim3;
    MTR1_FORWARD.channel = TIM_CHANNEL_1;
    MTR1_BACKWARD.htim = htim4;
    MTR1_BACKWARD.channel = TIM_CHANNEL_1;

    MTR2_FORWARD.htim = htim3;
    MTR2_FORWARD.channel = TIM_CHANNEL_3;
    MTR2_BACKWARD.htim = htim3;
    MTR2_BACKWARD.channel = TIM_CHANNEL_4;
}

// 初始化P波 驱动为低电平有效
void pwm_init(void) {
    _set_duty(htim3, TIM_CHANNEL_1, 0);
    _set_duty(htim4, TIM_CHANNEL_1, 0);
    _set_duty(htim3, TIM_CHANNEL_3, 0);
    _set_duty(htim3, TIM_CHANNEL_4, 0);
    HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start_IT(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_4);
}

void set_speed(int MTR, int speed) {
    if (MTR == MTR_J1) {
        if (speed>=0) {
            if (speed>MAX_SPEED) {
                speed = MAX_SPEED;
            }
            set_channel_speed(MTR1_BACKWARD, 0);
            set_channel_speed(MTR1_FORWARD, speed);
        } else if (speed<0) {
            set_channel_speed(MTR1_FORWARD, 0);
            set_channel_speed(MTR1_BACKWARD, speed);
        }
    } else if (MTR == MTR_J2) {
        if (speed>=0) {
            if (speed>MAX_SPEED) {
                speed = MAX_SPEED;
            }
            set_channel_speed(MTR2_BACKWARD, 0);
            set_channel_speed(MTR2_FORWARD, speed);
        } else if (speed<0) {
            set_channel_speed(MTR2_FORWARD, 0);
            set_channel_speed(MTR2_BACKWARD, speed);
        }

    }
}

void set_channel_speed(struct pwm_conf conf, int speed) {
    _set_duty(conf.htim, conf.channel, 100-speed);
}

void set_freq_duty(struct pwm_conf conf, int freq, int duty) {
    _set_freq_duty(conf.htim, conf.channel, freq, duty);
}

/* TIM_OCPOLARITY_HIGH 0x00000000U
   TIM_OCPOLARITY_LOW  0x00000002U
   if low TIMx->CCER == 0x2*/
void _set_duty(TIM_HandleTypeDef htim, uint32_t channel, int duty) {
    if (duty < 0 || duty > 100) {
        uprintf("Error arg duty %d\r\n", duty);
        return;
    }
    uprintf("Change duty to %d\r\n", duty);
    TIM_TypeDef *TIM = htim.Instance;
    uint32_t tempccr = (uint32_t) (((TIM->CCER)? duty:(100-duty))*(TIM->ARR + 1)/100 - 1);
    if (channel != TIM_CHANNEL_ALL) {
        set_CCR(htim, channel, tempccr);
    }
}

void set_CCR(TIM_HandleTypeDef htim, uint32_t channel, uint32_t ccr) {
    *(&(htim.Instance->CCR1)+channel/4) = ccr;
    uprintf("set channel %d ccr %d\r\n", channel/4+1, *(&(htim.Instance->CCR1)+channel/4));
}

void _set_freq_duty(TIM_HandleTypeDef htim, uint32_t channel, int freq, int duty) {
    if (duty < 0 || duty > 100 || freq <= 0) {
        uprintf("Error arg\r\n");
    }
    uprintf("Change frequence to %.3f", freq);
    TIM_TypeDef *TIM = htim.Instance;
    TIM->ARR = (uint32_t) ((SYSCLK/(htim.Init.Prescaler+1))/freq - 1);
    uprintf("(%d)\r\n", TIM->ARR);
    _set_duty(htim, channel, duty);
}