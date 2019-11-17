#include "gyro_acc.h"
#include "flag.h"
#include "usart.h"
#include "math.h"
#include "icm20600.h"

#define PI (acos(-1))

// 角速度零点误差
struct zero_bias angle_speed_bias[3] = {
    {-11.2, 1.2, -1.3},
    {-0.7, 1.2, -1.2},
    {0, 1, -1}
};

// Private function for gyro_acc
void read_raw_ac(int16_t *ac);
void read_raw_gy(int16_t *gy);
void get_acc(float* acc); 
void get_angle_speed(float *angle_speed);

void get_acc_gyro(float *acc, float *gyro) {
    int16_t ac[3], gy[3];
    ICM_Read_Raw(ac, gy);
    Accraw_to_Acceleration(ac, acc);
    Gyroraw_to_Angle_Speed(gy, gyro);
}

// 积分计算初始化函数
void int_init(void) {
    get_acc_angle(last_angle);
}

void show_angle(void) {
    float angle[3], acc_angle[3];
    float accx_angle = _get_angle(angle, acc_angle);
    send_wave(angle[X], angle[Y], angle[Z], accx_angle);
}

float k = 27;
//float k = 0.15;
float last_angle[3], ac_ang[3];
float _get_angle(float *angle, float *acc_angle) {
    float angle_speed[3], acc[3];
    int16_t gy[3], ac[3];
    ICM_Read_Raw(ac, gy);

    Accraw_to_Acceleration(ac, acc);
    Gyroraw_to_Angle_Speed(gy, angle_speed);
    rect(angle_speed, angle_speed_bias);

    acc_angle[X] = atan2(acc[Y], acc[Z])*180/PI;
    acc_angle[Y] = atan2(acc[X], acc[Z])*180/PI;
    acc_angle[Z] = atan2(acc[X], acc[Y])*180/PI;

    angle[X] = last_angle[X] + (angle_speed[X]+(acc_angle[X]-last_angle[X])*k)*INT_GAP/1000;
    angle[Y] = last_angle[Y] + (angle_speed[Y]+(acc_angle[Y]-last_angle[Y])*k)*INT_GAP/1000;
    angle[Z] = last_angle[Z] + (angle_speed[Z]+(acc_angle[Z]-last_angle[Z])*k)*INT_GAP/1000;

    last_angle[X] = angle[X];
    last_angle[Y] = angle[Y];
    last_angle[Z] = angle[Z];
    return acc_angle[X];
}

void get_2angle_speed(float *angle, float *aspeed, float *acc_angle) {
    float acc[3];
    int16_t gy[3], ac[3];
    ICM_Read_Raw(ac, gy);

    Accraw_to_Acceleration(ac, acc);
    Gyroraw_to_Angle_Speed(gy, aspeed);
    rect(aspeed, angle_speed_bias);

    acc_angle[X] = atan2(acc[Y], acc[Z])*180/PI;
    acc_angle[Y] = atan2(acc[X], acc[Z])*180/PI;
    acc_angle[Z] = atan2(acc[X], acc[Y])*180/PI;

    angle[X] = last_angle[X] + (aspeed[X]+(acc_angle[X]-last_angle[X])*k)*INT_GAP/1000;
    angle[Y] = last_angle[Y] + (aspeed[Y]+(acc_angle[Y]-last_angle[Y])*k)*INT_GAP/1000;
    angle[Z] = last_angle[Z] + (aspeed[Z]+(acc_angle[Z]-last_angle[Z])*k)*INT_GAP/1000;

    last_angle[X] = angle[X];
    last_angle[Y] = angle[Y];
    last_angle[Z] = angle[Z];
}

void show_angle_both(void) {
    float acc_angle[3], gyro_angle[3];
    get_acc_angle(acc_angle);
    get_gyro_angle(gyro_angle);
    send_wave6(acc_angle, gyro_angle);
}
void show_gyro_angle(void) {
    float angle[3];
    get_gyro_angle(angle);
    send_wave(angle[X], angle[Y], angle[Z], 0);
}

void show_acc_angle(void) {
    float angle[3];
    get_acc_angle(angle);
    send_wave(angle[X], angle[Y], angle[Z], 0);
}

// 从acc计算角度, X: x为轴旋转度数
void get_acc_angle(float* angle) {
    float acc[3];
    get_acc(acc);
    angle[X] = atan2(acc[Y], acc[Z])*180/PI;
    angle[Y] = atan2(acc[X], acc[Z])*180/PI;
    angle[Z] = atan2(acc[X], acc[Y])*180/PI;
}

void get_gyro_angle(float* angle) {
    float angle_speed[3];
    get_angle_speed(angle_speed);
    angle[X] = last_angle[X] + angle_speed[X]*INT_GAP/1000;
    angle[Y] = last_angle[Y] + angle_speed[Y]*INT_GAP/1000;
    angle[Z] = last_angle[Z] + angle_speed[Z]*INT_GAP/1000;
    last_angle[X] = angle[X];
    last_angle[Y] = angle[Y];
    last_angle[Z] = angle[Z];
}

// 示波从陀螺仪计算出的角速度
void show_gyro_angle_speed(void) {
    float angle_speed[3];
    get_angle_speed(angle_speed);
    send_wave(angle_speed[0], angle_speed[1], angle_speed[2], 0);
}

// 示波从加速度计算出加速度
void gyro_show_acc(void) {
    float acc[3];
    get_acc(acc);
    send_wave(acc[0], acc[1], acc[2], 0);
}

void read_raw_ac(int16_t *ac) {
    int16_t gy[3];
    ICM_Read_Raw(ac, gy);
}

void read_raw_gy(int16_t *gy) {
    int16_t ac[3];
    ICM_Read_Raw(ac, gy);
}

// 从加速度计获取加速度
void get_acc(float *acc) {
    int16_t ac[3];
    read_raw_ac(ac);
    Accraw_to_Acceleration(ac, acc);
}

// 从陀螺仪获取角速度
void get_angle_speed(float *angle_speed) {
    int16_t gy[3];
    read_raw_gy(gy);
    Gyroraw_to_Angle_Speed(gy, angle_speed);
    rect(angle_speed, angle_speed_bias);
}

void rect(float *val, struct zero_bias *bias) {
    int i;
    for (i=0; i<3; i++) {
        struct zero_bias temp = bias[i];
        if (val[i] > temp.zero + temp.pos_bias) {
            val[i] -= (temp.zero + temp.pos_bias);
        } else if (val[i] < temp.zero+temp.neg_bias) {
            val[i] -= temp.zero+temp.neg_bias;
        } else {
            val[i] = 0;
        }
    }
}