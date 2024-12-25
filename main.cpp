
#include "ds4_on_pico_w.hpp"
#include "pico/stdlib.h"

#include <algorithm>
#include <stdio.h>

int main()
{
    DS4forPicoW controller;
    bool loop_contents = true;
    ////////////////////////////////////////////
    // SETUP
    ////////////////////////////////////////////
    stdio_init_all();
    sleep_ms(5000);
    printf("======================\n[SETUP] DS4 on PicoW\n======================\n");

    controller.setup();
    while (1) {
        do {
            printf("Starting inquiry scan..\n");
            loop_contents = controller.scan(3000);
        } while (false == loop_contents);

        ////////////////////////////////////////////
        // LOOP
        ////////////////////////////////////////////
        bool flag_can_use = false;
        if (controller.is_use()) {
            flag_can_use = true;
        }
        while (loop_contents) {
            // tight_loop_contents();
            if (controller.is_use()) {
                DualShock4_state state = controller.get_state();
                printf("HAT[%01d] ", state.hat);
                printf("SHA[%s] ", state.share ? "x" : " ");
                printf("OPT[%s] ", state.options ? "x" : " ");
                printf("PS[%s] ", state.ps ? "x" : " ");
                printf("A[%s] ", state.cross ? "x" : " ");
                printf("B[%s] ", state.circle ? "x" : " ");
                printf("X[%s] ", state.square ? "x" : " ");
                printf("Y[%s] ", state.triangle ? "x" : " ");
                printf("Left[%s/%s(%03d)/%s(%03d,%03d)] ", state.l1 ? "x" : " ", state.l2 ? "x" : " ", state.l2_value, state.l3 ? "x" : " ", state.l3_x, state.l3_y);
                printf("Right[%s/%s(%03d)/%s(%03d,%03d)] ", state.r1 ? "x" : " ", state.r2 ? "x" : " ", state.r2_value, state.r3 ? "x" : " ", state.r3_x, state.r3_y);
                printf("Touch[%s(%03d,%03d)] ", state.touch ? "x" : " ", state.touch_x, state.touch_y);
                printf("Gyro[%06d,%06d,%06d] ", state.gyro_x, state.gyro_y, state.gyro_z);
                printf("Accel[%06d,%06d,%06d] ", state.accel_x, state.accel_y, state.accel_z);
                printf("Templ[%06d] ", state.temperature);
                printf("Battery[%03d] ", state.battery);
                printf("Timestamp[%06d] ", state.timestamp);
                printf("Status[%s] ", state.connected ? "connected" : "Searching");

                printf("\n");

            } else {
                if (true == flag_can_use) {
                    loop_contents = false;
                    break;
                }
            }
            sleep_ms(250);
        }
    }

    ////////////////////////////////////////////
    // CLOSING
    ////////////////////////////////////////////
    printf("[CLOSING] DS4 on PicoW\n");
    stdio_deinit_all();
    return 0;
}
