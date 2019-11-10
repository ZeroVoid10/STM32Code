#include "cmd_func.h"
#include "can_utils.h"
#include "can_func.h"
#include "flags.h"
#include <stdlib.h>

#ifdef SL_NRF_COMM
#include "nrf24l01.h"
void cmd_nrf_set_tx_addr(int argc, char *argv[]) {
    if (argc < 4) {
        cmd_err_arg_default_handle(NULL);
        return ;
    }
    uint8_t addr[argc-1];
    uprintf("set nrf addr width %d\r\nset addr: ", argc - 1);
    nrf_set_addr_width(argc - 1);
    for (int i = 1; i < argc; i++) {
        addr[i-1] = (uint8_t) atoi(argv[i]);
        uprintf("%3d ", addr[i-1]);
    }
    uprintf("\r\n");
    nrf_set_tx_addr(addr, argc-3);
} 

void cmd_nrf_get_tx_addr(int argc, char *argv[]) {
    uint8_t *addr = NULL, len;
    nrf_get_tx_addr(&addr, &len);
    uprintf("addr len: %d\r\naddr: ", len);
    for (int i = 0; i<len; i++) {
        uprintf("%3d ", addr[i]);
    }
    uprintf("\r\n");
}

void cmd_nrf_set_rx_addr(int argc, char *argv[]) {
    if (argc < 2) {
        cmd_err_arg_default_handle(NULL);
        return ;
    }
    NRF_PIPE pipe = atoi(argv[1]);
    uint8_t addr[argc - 2];
    nrf_set_addr_width(argc - 1);
    for (int i = 2; i < argc; i++) {
        addr[i-1] = (uint8_t) atoi(argv[i]);
    }
    nrf_set_rx_addr(pipe, addr, argc-2);
}

void cmd_nrf_get_rx_addr(int argc, char *argv[]) {
    if (argc == 2) {
        uint8_t *addr = NULL, len;
        NRF_PIPE pipe = atoi(argv[1]);
        nrf_get_rx_addr(pipe, &addr, &len);
        uprintf("len %d\r\n", len);
        for (int i = 0; i<len; i++) {
            uprintf("%3d ", addr[i]);
        }
        uprintf("\r\n");
    } else {
        cmd_err_arg_default_handle(NULL);
    }
}
#endif // SL_NRF_COMM

#ifdef SL_MOTOR_DRIVER
#include "mtr_driver.h"
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


#ifdef SL_DEBUG
void cmd_hello_func(int argc, char *argv[]) {
    #ifdef SL_CMD
    uprintf("hello world\r\n");
    #endif // SL_CMD
}

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

    #ifdef SL_NRF_COMM
    cmd_add("nrf_set_tx", "set tx addr", cmd_nrf_set_tx_addr);
    cmd_add("nrf_get_tx", "get tx addr", cmd_nrf_get_tx_addr);
    cmd_add("nrf_set_rx", "set rx addr", cmd_nrf_set_rx_addr);
    cmd_add("nrf_get_rx", "get rx addr", cmd_nrf_get_rx_addr);
    #endif // SL_NRF_COMM

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