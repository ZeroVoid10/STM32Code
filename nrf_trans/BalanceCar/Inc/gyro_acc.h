#ifndef __GYRO_ACC_H__
#define __GYRO_ACC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "icm20600.h"

#define X 0
#define Y 1
#define Z 2

struct zero_bias {
    float zero;
    float pos_bias;
    float neg_bias;
};

extern float last_angle[3];
extern float ac_ang[3];

void int_init(void);

void acc_show_angle(void);
void gyro_show_angle_speed(void);
void gyro_show_acc(void);
void wave_stop(void);

void get_acc_angle(float *angle);
void get_gyro_angle(float *angle);
float _get_angle(float *angle, float *acc_angle);
void get_2angle_speed(float *angle, float *aspeed, float *acc_angle);

void show_acc_angle(void);
void show_gyro_angle(void);
void show_gyro_angle_speed(void);
void show_angle(void);
void show_angle(void);

void rect(float *val, struct zero_bias *bias);

#ifdef __cplusplus
}
#endif 

#endif /* __GYRO_ACC_H__ */