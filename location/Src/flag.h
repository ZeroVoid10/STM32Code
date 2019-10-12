#ifndef __FLAG_H__
#define __FLAG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define EST_ANGLE_FLAG 0x00000001U
#define ACC_ANGLE_FLAG 0x00000002U
#define GYRO_ANGLE_FLAG 0x00000004U

extern int _5ms_flag;
extern uint32_t show_angle_flag;
extern int count;
extern int INT_GAP;
extern int run_shell_flag;

#ifdef __cplusplus
}
#endif 

#endif /* __FLAG_H__ */