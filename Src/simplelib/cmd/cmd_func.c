#include "cmd_func.h"
#include "can_utils.h"
#include "can_func.h"
#include "flags.h"
#include "kick.h"
#include "mtr_driver.h"
#include <stdlib.h>


void cmd_magnet_toggle(int argc, char *argv[]);
void cmd_magnet_set(int argc, char *argv[]);
void cmd_magnet_free(int argc, char *argv[]);

void cmd_kick_test_reset(int argc, char *argv[]);
void cmd_kick_stata_show(int argc, char *argv[]);
void cmd_mag_mtr_free(int argc, char *argv[]);
//void cmd_mag_mtr_up_duty(int argc, char *argv[]);
void cmd_mag_mtr_up_speed(int argc, char *argv[]);
void cmd_mag_mtr_up_pos(int argc, char *argv[]);
void cmd_mag_mtr_up_lp(int argc, char *argv[]);
void cmd_spr_mtr_back(int argc, char *argv[]);
void cmd_kick_spr_free(int argc, char *argv[]);
void cmd_cal_spr_duty(int argc, char *argv[]);
void cmd_cal_mag_speed(int argc, char *argv[]);

void cmd_kick_test_reset(int argc, char *argv[]) {
    uprintf("reset kick test\r\n");
    kick_test_reset(&kick_ctrl);
}

void cmd_kick_stata_show(int argc, char *argv[]) {
    uprintf("\nstate %d\r\nmag_mtr pos %f\r\nsrp mtr pos: %f\r\nmag_mtr_state %d\r\nmag_state %d\r\n", 
            kick_ctrl.state, kick_ctrl.mag_mtr_can_state.position, kick_ctrl.spr_mtr_pos, kick_ctrl.mag_mtr_state, kick_ctrl.magnet_state);
    uprintf("spr back duty %d\r\nspr free duty %d\r\n", kick_ctrl.spr_mtr_back_duty, kick_ctrl.spr_mtr_free_duty);
    uprintf("up time %d\r\nmag delay time %d\r\n spr back time %d\r\nspr trans time %d\r\n", kick_ctrl.mag_mtr_up_time, kick_ctrl.mag_delay_time, kick_ctrl.spr_mtr_back_time, kick_ctrl.spr_trans_time);
}

void cmd_kick_spr_free(int argc, char *argv[]) {
    uprintf("spring free\r\n");
    md_set_duty(MAG_MTR_SID, 0);
    kick_ctrl.mag_mtr_state = MAG_MTR_FREE;
    kick_test_flag = 0;
    kick_test_flag |= KICK_SPR_FREE_MSK;
}

/*
void cmd_mag_mtr_up_duty(int argc, char *argv[]) {
    if (argc == 2) {
        int arg = atoi(argv[1]);
        uprintf("mag mtr up test: duty %d\r\n", arg);
        kick_ctrl.mag_mtr_up_arg = arg;
        kick_ctrl.mag_mtr_mode = MMTR_DUTY;
        kick_test_flag &= MAG_MTR_UP_DUTY_MSK;
    }
}*/

void cmd_mag_mtr_up_speed(int argc, char *argv[]) {
    if (argc == 3) {
        int arg = atoi(argv[1]);
        int pos = atoi(argv[2]);
        uprintf("mag mtr up test: speed %d\r\n", arg);
        kick_ctrl.mag_mtr_up_arg = arg;
        kick_ctrl.mag_mtr_up_pos = pos;
        kick_ctrl.mag_mtr_mode = MMTR_SPEED;
        kick_ctrl.state = KICK_READY;
        kick_ctrl.mag_mtr_state = MAG_MTR_READY;
        kick_test_flag = 0;
        kick_test_flag |= MAG_MTR_UP_SPEED_MSK;
    }
}

void cmd_mag_mtr_up_lp(int argc, char *argv[]) {
    if (argc == 2) {
        int arg = atoi(argv[1]);
        uprintf("mag mtr up test: pos %d\r\n", arg);
        //kick_ctrl.mag_mtr_up_arg = arg;
        kick_ctrl.mag_mtr_up_pos = arg;
        kick_ctrl.mag_mtr_mode = MMTR_LIN_SPEED;
        kick_ctrl.state = KICK_READY;
        kick_ctrl.mag_mtr_state = MAG_MTR_READY;
        kick_test_flag = 0;
        kick_test_flag |= MAG_MTR_UP_POSITION_MSK;
    }
}

void cmd_mag_mtr_up_pos(int argc, char *argv[]) {
    if (argc == 2) {
        int arg = atoi(argv[1]);
        uprintf("mag mtr up test: pos %d\r\n", arg);
        kick_ctrl.mag_mtr_up_arg = arg;
        kick_ctrl.mag_mtr_mode = MMTR_POS;
        kick_ctrl.state = KICK_READY;
        kick_ctrl.mag_mtr_state = MAG_MTR_READY;
        kick_test_flag = 0;
        kick_test_flag |= MAG_MTR_UP_POSITION_MSK;
    }
}

void cmd_mag_mtr_free(int argc, char *argv[]) {
    md_set_duty(MAG_MTR_SID, 0);
    magnet_free();
    uprintf("mag mtr free\r\n");
}

void cmd_spr_mtr_back(int argc, char *argv[]) {
    if (argc == 2) {
        float arg = (uint32_t)atof(argv[1]);
        uprintf("spring mtr back pos: %f\r\n", arg);
        kick_ctrl.spr_mtr_kick_pos = arg;
        kick_ctrl.state = KICK_MAG_CONNECTED;
        kick_test_flag = 0;
        kick_test_flag |= SPR_MTR_BACK_MSK;
    }
}

void cmd_cal_spr_duty(int argc, char *argv[]) {
    if (argc == 3) {
        float speed = atof(argv[1]);
        float pos = atof(argv[2]);
        struct kick_controller test = {0};
        test.mag_mtr_can_state.speed = speed;
        test.mag_mtr_can_state.position = pos;
        uprintf("cal duty: %f\r\n", set_spr_mtr_duty(&test));
    }
}

void cmd_cal_mag_speed(int argc, char *argv[]) {
    if (argc == 3) {
        float free_duty = atof(argv[1]);
        float pos = atof(argv[2]);
        struct kick_controller test = {0};
        test.spr_mtr_free_duty = free_duty;
        test.mag_mtr_can_state.position = pos;
        uprintf("cal speed: %f\r\n", set_mag_mtr_speed(&test));
    }
}

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
    uprintf("hello world\r\n");
}

void cmd_magnet_toggle(int argc, char *argv[]) {
    MAGNET_STATE state = magnet_toggle();
    uprintf("magnet state: %d\r\n", state);
}

void cmd_magnet_set(int argc, char *argv[]) {
    MAGNET_STATE state = magnet_set();
    uprintf("magnet state: %d\r\n", state);
}

void cmd_magnet_free(int argc, char *argv[]) {
    MAGNET_STATE state = magnet_free();
    uprintf("magnet state: %d\r\n", state);
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
    cmd_add("hello", "just", cmd_hello_func);

    cmd_add("magtog", "magnet toggle", cmd_magnet_toggle);
    cmd_add("magset", "magnet set", cmd_magnet_set);
    cmd_add("magfree", "magnet free", cmd_magnet_free);

    //cmd_add("mmup_duty", "mag mtr up duty", cmd_mag_mtr_up_duty);
    cmd_add("mmup_speed", "", cmd_mag_mtr_up_speed);
    cmd_add("mmup_pos", "", cmd_mag_mtr_up_pos);
    cmd_add("smback", "spring mtr back", cmd_spr_mtr_back);
    cmd_add("mmup_free", "", cmd_mag_mtr_free);
    cmd_add("spr_free", "kick spring free", cmd_kick_spr_free);
    cmd_add("kr", "kick reset", cmd_kick_test_reset);
    cmd_add("ks", "kick state", cmd_kick_stata_show);
    cmd_add("spr_duty", "call spr mtr duty", cmd_cal_spr_duty);
    cmd_add("mag_speed", "", cmd_cal_mag_speed);
    cmd_add("mmup_lp", "", cmd_mag_mtr_up_lp);

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
    cmd_add("can_test", "test can", cmd_can_test);
    cmd_add("wave", "", cmd_wave_test);
    cmd_add("rocker", "rocker", cmd_show_rocker);
    cmd_add("can_stop", "s", cmd_stop_rocker);
    #endif
}