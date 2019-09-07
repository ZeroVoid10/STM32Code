#ifndef __MTR_H__
#define __MTR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tim.h"

#define LOW_VALID 0
#define HIGH_vALID 1
#define MTR_J1 0
#define MTR_J2 1
#define MAX_SPEED 70

struct pwm_conf {
    TIM_HandleTypeDef htim;
    uint32_t channel;
};

extern struct pwm_conf MTR1_FORWARD;
extern struct pwm_conf MTR1_BACKWARD;
extern struct pwm_conf MTR2_FORWARD;
extern struct pwm_conf MTR2_FORWARD;

void pwm_init(void);
void pwm_conf_init(void);

void _set_duty(TIM_HandleTypeDef htim, uint32_t channel, int duty); 
void set_CCR(TIM_HandleTypeDef htim, uint32_t channel, uint32_t ccr);
void _set_freq_duty(TIM_HandleTypeDef htim, uint32_t channel, int freq, int duty);
void set_speed(int MTR, int speed);

#ifdef __cplusplus
}
#endif 

#endif /* __MTR_H__ */