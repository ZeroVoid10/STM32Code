#include "mtr.h"
#include "usart.h"
#include "tim.h"
#include "gyro_acc.h"
#include "pid.h"

struct pwm_conf MTR1_FORWARD;
struct pwm_conf MTR1_BACKWARD;
struct pwm_conf MTR2_FORWARD;
struct pwm_conf MTR2_BACKWARD;
uint32_t MAX_SPEED_CCR = 1199; //3999 ARR 70%
uint32_t ZERO_SPEED_CCR = 3999; 

void set_channel_speed(struct pwm_conf conf, int speed);
int last_speed = 0;
float target_angle = -50;
//float target_angle = -89;
//float target_angle = -80;

void control_angle() {
    float angle[3], acc_angle[3], pid_rel;
    _get_angle(angle, acc_angle);
    pid_rel = pd_release(&angle_pd, target_angle, angle[X]);
    int speed = last_speed + (int)pid_rel;
    Limit(speed, 2800);
    set_speed(MTR_J1, speed);
    set_speed(MTR_J2, speed);
    last_speed = speed;
    send_wave(angle[X], (pid_rel+1)/40, (speed+1)/40, acc_angle[X]);
}

//float k_angle = 180, k_aspeed = 8; // a plan speed limit 2000
//float k_angle = 220, k_aspeed = 14;   // b plan speed limit 1400
//float k_angle = 250, k_aspeed = 8;  //c plan speed limit 1400
//float k_angle = 215, k_aspeed = 12.5; // limit 1400
float k_angle = 220, k_aspeed = 7;
void control_angle_2(void) {
    float angle[3], aspeed[3], acc_angle[3];
    get_2angle_speed(angle, aspeed, acc_angle);
    int speed = (int)(k_angle*(target_angle-angle[X]) - k_aspeed*aspeed[X]);
    Limit(speed, 1400);
    send_wave(angle[X], 0, (speed+1)/40, acc_angle[X]);
    set_speed(MTR_J1, speed);
    set_speed(MTR_J2, speed);
    last_speed = speed;
}

float K_aspedd_hv = 18;
void control_angle_3(void) {
    float angle[3], aspeed[3], acc_angle[3];
    get_2angle_speed(angle, aspeed, acc_angle);
    float kd = (angle[X] > -85)? K_aspedd_hv:k_aspeed;
    int speed = (int)(k_angle*(target_angle-angle[X]) - kd*aspeed[X]);
    Limit(speed, 2500);
    send_wave(angle[X], 0, (speed+1)/40, acc_angle[X]);
    set_speed(MTR_J1, speed);
    set_speed(MTR_J2, speed);
    last_speed = speed;
}

void show_pid_state(void) {
}

void pwm_conf_init(void) {
    MTR1_FORWARD.htim = htim4;
    MTR1_FORWARD.channel = TIM_CHANNEL_1;
    MTR1_BACKWARD.htim = htim3;
    MTR1_BACKWARD.channel = TIM_CHANNEL_1;

    MTR2_FORWARD.htim = htim3;
    MTR2_FORWARD.channel = TIM_CHANNEL_4;
    MTR2_BACKWARD.htim = htim3;
    MTR2_BACKWARD.channel = TIM_CHANNEL_3;
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

int left_start_ccr = 3859;
int right_start_ccr = 3699;
void set_speed(int MTR, int speed) {
    uint32_t tempccr = ZERO_SPEED_CCR;
    if (speed >= 3199) {
        speed = 0;
    }
    if (MTR == MTR_J1) {
        if (speed> 0) {
            tempccr = left_start_ccr - speed;
            set_CCR(MTR1_BACKWARD, ZERO_SPEED_CCR);
            set_CCR(MTR1_FORWARD, (tempccr<MAX_SPEED_CCR)? MAX_SPEED_CCR: tempccr);
        } else if (speed<0) {
            tempccr = left_start_ccr + speed;
            set_CCR(MTR1_FORWARD, ZERO_SPEED_CCR);
            set_CCR(MTR1_BACKWARD, (tempccr<MAX_SPEED_CCR)? MAX_SPEED_CCR:tempccr);
        } else if (speed == 0) {
            set_CCR(MTR1_FORWARD, ZERO_SPEED_CCR);
            set_CCR(MTR1_BACKWARD, ZERO_SPEED_CCR);
        }
    } else if (MTR == MTR_J2) {
        if (speed>0) {
            tempccr = right_start_ccr - speed;
            set_CCR(MTR2_BACKWARD, ZERO_SPEED_CCR);
            set_CCR(MTR2_FORWARD, (tempccr<MAX_SPEED_CCR)? MAX_SPEED_CCR: tempccr);
        } else if (speed<0) {
            tempccr = right_start_ccr + speed;
            set_CCR(MTR2_FORWARD, ZERO_SPEED_CCR);
            set_CCR(MTR2_BACKWARD, (tempccr<MAX_SPEED_CCR)? MAX_SPEED_CCR:tempccr);
        } else if( speed == 0) {
            set_CCR(MTR2_FORWARD, ZERO_SPEED_CCR);
            set_CCR(MTR2_BACKWARD, ZERO_SPEED_CCR);
        }
    }
}

void set_speed_duty(int MTR, int speed) {
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
        _set_CCR(htim, channel, tempccr);
    }
}

void _set_CCR(TIM_HandleTypeDef htim, uint32_t channel, uint32_t ccr) {
    *(&(htim.Instance->CCR1)+channel/4) = ccr;
    uprintf("set channel %d ccr %d\r\n", channel/4+1, *(&(htim.Instance->CCR1)+channel/4));
}

void set_CCR(struct pwm_conf conf, uint32_t ccr) {
    _set_CCR(conf.htim, conf.channel, ccr);
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