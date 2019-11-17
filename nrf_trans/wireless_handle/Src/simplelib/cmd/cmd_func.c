#include "cmd_func.h"
#include "can_utils.h"
#include "can_func.h"
#include "simplelib_cfg.h"
#include "flags.h"


void cmd_func_init(void) {
    cmd_add("hello", "just", cmd_hello_func);
    cmd_add("rocker", "rocker", cmd_show_rocker);
    cmd_add("can_stop", "s", cmd_stop_rocker);

    #ifdef DEBUG
    cmd_add("can_test", "test can", cmd_can_test);
    cmd_add("test_wave", "", cmd_wave_test);
    #endif
}

void cmd_hello_func(int argc, char *argv[]) {
    uprintf("hello world\r\n");
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