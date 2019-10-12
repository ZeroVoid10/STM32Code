#ifndef __CMD_FUNC_H__
#define __STDHEADER_H__

#ifdef __cplusplus
extern "C" {
#endif

void cmd_pwm(int argc, char *argv[]);
void cmd_set_speed(int argc, char *argv[]);
void cmd_stop_wave(int argc, char *argv[]);
void cmd_angle(int argc, char *argv[]);

void cmd_show_acc(int argc, char *argv[]);
void cmd_show_angle_speed(int argc, char *argv[]);

void cmd_p(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif 

#endif /* __CMD_FUNC_H__ */