#ifndef __PID_H__
#define __PID_H__ 

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define Limit(value,max)     if(value>max)value=max;else if(value<-max)value=-max
 
// copy from driver code
typedef struct {
    float kp;
    float kd;
    float ki;
    float i;
    float last_err;
    float i_max;
    float last_d;
    float I_TIME;
} PID_struct;

extern PID_struct angle_pd;
float pid_release(PID_struct *pid,float target,float now);
float pd_release(PID_struct *pid, float target, float now);
void reset_PID(PID_struct * s);
void PID_init();

#ifdef __cplusplus
}
#endif 

#endif /* _PID_H__ */