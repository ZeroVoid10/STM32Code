#include "can_func.h"
#include "cmd.h"
#include "simplelib_cfg.h"

void can_func_init() {
    #ifdef SL_DEBUG
    can_callback_add(1, can_suc_rx);
    can_callback_add(325, can_show_button);
    can_callback_add(324, can_show_rocker);
    #endif
}

void can_suc_rx(can_msg *data) {
    uprintf("can rx ok\r\n");
}

void can_show_button(can_msg *data) {
    uprintf("%d %d %c\r\n", data->ui8[0], data->ui8[1], data->ui8[2]);
}

int can_data_show_flag = 0;
void can_show_rocker(can_msg *data) {
    if (can_data_show_flag) {
        uprintf("%4d %4d %4d %4d\r", (int16_t)data->ui16[0], 
                                (int16_t)data->ui16[1],
                                (int16_t)data->ui16[2],
                                (int16_t)data->ui16[3]);
    }
}