#include "pid.h"
#include "gyro_acc.h"
#include "mtr.h"

//PID_struct angle_pd = {0, 0, 0, 0, 0, 5000, 0, 0.005};
PID_struct angle_pd = {1, 0, 0, 0, 0, 5000, 0, 0.005};

float pid_release(PID_struct *pid, float target, float now)
{    
  float err;
  float err_dt;
  float result;
  
  err = target-now; 
  err_dt = err-pid->last_err; 
  
  /*
  err_dt*=0.384f;
  err_dt+=pid->last_d*0.615f;
  */
  
  pid->last_err=err;
  
  pid->i+=err*pid->I_TIME;
  
  Limit(pid->i,pid->i_max);
  pid->last_d=err_dt;
  
  result = err * pid->kp +   err_dt * pid->kd + pid->i * pid->ki;
  return result;
}

float pd_release(PID_struct *pid, float target, float now) {
    float err, err_dt, res;

    err = target-now;
    err_dt = err-pid->last_err;

    pid->last_err = err;
    pid->last_d = err_dt;
    res = err*pid->kp + err_dt*pid->kd;
    return res;
}

void pid_set(PID_struct *pid, float kp, float kd, float ki) {
    pid->kp = kp;
    pid->kd = kd;
    pid->ki = ki;
}

/*
void PD_Cal(Angle_PD_TypeDef *pd) {
    float angle[3];
    _get_angle(angle);
    float error = pd->target - angle[X];
    uint32_t pout = (uint32_t)pd->kp*error;
    uint32_t dout = (uint32_t)pd->kd*(error - pd->lastError);
    uint32_t speed = last_speed + pout + dout;
    pd->lastError = error;
    //uprintf("error %d, pout %d, dout %d\r\n", error, pout, dout);
    //uprintf("arr %d set ccr %d\r\n", TIM4->ARR, tempccr);
    set_speed(MTR_J1, speed);
    set_speed(MTR_J2, speed);
    last_speed = speed;
}
*/

void reset_PID(PID_struct * pid) {
    pid->i = 0;
    pid->last_err = 0;
    pid->last_d = 0;
}

void PID_init() {
    reset_PID(&angle_pd);
}