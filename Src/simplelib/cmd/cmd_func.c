#include "cmd_func.h"
#include "can_utils.h"
#include "can_func.h"
#include "flags.h"
#include "mtr_driver.h"
#include <stdlib.h>

#ifdef SL_MOTOR_DRIVER
#ifdef EN_VESC_MOTOR_DRIVER
void cmd_vesc_set_duty(int argc, char *argv[]);
void cmd_vesc_set_rpm(int argc, char *argv[]);
void cmd_vesc_set_pos(int argc, char *argv[]);
void cmd_vesc_set_handbrake(int argc, char *argv[]);
void cmd_vesc_set_current(int argc, char *argv[]);
void cmd_vesc_set_current_brake(int argc, char *argv[]);

void cmd_vesc_set_duty(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        uprintf("id %d set duty %f\r\n", id, arg);
        vesc_set_duty(id, arg);
    } else {
        uprintf("Error Args");
    }
}

void cmd_vesc_set_rpm(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        uprintf("id %d set rpm %f\r\n", id, arg);
        vesc_set_rpm(id, arg);
    } else {
        uprintf("Error Args");
    }
}

void cmd_vesc_set_pos(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        uprintf("id %d set pos %f\r\n", id ,arg);
        vesc_set_pos(id, arg);
    } else {
        uprintf("Error Args");
    }
}

void cmd_vesc_set_handbrake(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        vesc_set_handbrake(id, arg);
    } else {
        uprintf("Error Args");
    }
}

void cmd_vesc_set_current(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        vesc_set_current(id, arg);
    } else {
        uprintf("Error Args");
    }
}

void cmd_vesc_set_current_brake(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        float arg = atof(argv[2]);
        vesc_set_current_brake(id, arg);
    } else {
        uprintf("Error Args");
    }
}
#endif // EN_VESC_MOTOR_DRIVER

#ifdef EN_MOTOR_DRIVER
void cmd_md_set_duty(int argc, char *argv[]);
void cmd_md_set_speed(int argc, char *argv[]);
void cmd_md_set_position(int argc, char *argv[]);

void cmd_md_set_duty(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        int arg = atoi(argv[2]);
        md_set_duty(id, arg);
        uprintf("id %d set duty %d\r\n", id, arg);
    } else {
        uprintf("Error Args\r\n");
    }
}

void cmd_md_set_speed(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        int arg = atoi(argv[2]);
        md_set_speed(id, arg);
        uprintf("id %d speed duty %d\r\n", id, arg);
    } else {
        uprintf("Error Args\r\n");
    }
}

void cmd_md_set_position(int argc, char *argv[]) {
    if (argc == 3) {
        uint8_t id = (uint8_t) atoi(argv[1]);
        int arg = atoi(argv[2]);
        md_set_position(id, arg);
        uprintf("id %d position duty %d\r\n", id, arg);
    } else {
        uprintf("Error Args\r\n");
    }
}

#endif // EN_MOTOR_DRIVER
#endif // SL_MOTOR_DRIVER

void cmd_hello_func(int argc, char *argv[]) {
    #ifdef SL_CMD
    uprintf("hello world\r\n");
    #endif // SL_CMD
}

#ifdef SL_DEBUG
void cmd_can_test(int argc, char *argv[]) {
    uprintf("can send test\r\n");
    can_send_test();
}

void cmd_show_rocker(int argc, char *argv[]) {
    uprintf("show rocker\r\n");
    can_data_show_flag = 1;
}

void cmd_stop_rocker(int argc, char *argv[]) {
    can_data_show_flag = 0;
    uprintf("sotp show rocker\r\n");
}

void cmd_wave_test(int argc, char *argv[]) {
    send_wave_flag ^= 1;
    uprintf(send_wave_flag? "Wave Start\r\n":"Wave Stop\r\n");
}
#endif // SL_DEBUG

void cmd_func_init(void) {

    #ifdef SL_MOTOR_DRIVER
    #ifdef EN_MOTOR_DRIVER
    cmd_add("m_duty", "", cmd_md_set_duty);
    cmd_add("m_speed", "", cmd_md_set_speed);
    cmd_add("m_pos", "", cmd_md_set_position);
    #endif // EN_MOTOR_DRIVER
    
    #ifdef EN_VESC_MOTOR_DRIVER
    cmd_add("v_duty", "vesc set duty", cmd_vesc_set_duty);
    cmd_add("v_rpm", "vesc set rpm", cmd_vesc_set_rpm);
    cmd_add("v_pos", "vesc set pos", cmd_vesc_set_pos);
    #endif // EN_VESC_MOTOR_DRIVER
    #endif // SL_MOTOR_DRIVER

    #ifdef SL_DEBUG
    cmd_add("hello", "just", cmd_hello_func);
    cmd_add("can_test", "test can", cmd_can_test);
    cmd_add("wave", "", cmd_wave_test);
    cmd_add("rocker", "rocker", cmd_show_rocker);
    cmd_add("can_stop", "s", cmd_stop_rocker);
    #endif
}