
#include "ds4_on_pico_w.hpp"
#include "pico/stdlib.h"

#include "serialplot.h"

#include <algorithm>
#include <stdio.h>

#define LOG_HEADER "[main]"

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
    // controller.setup((DS4forPicoW::config){ .mac_address = "00:00:00:00:00:00" });
    controller.setup();

    while (1) {
        loop_contents = false;
        do {
            DualShock4_state state = controller.get_state();
            if (true == state.linked) {
                loop_contents = true;
                printf("\n%s [Linked] %s\n", LOG_HEADER, controller.get_mac_address());
                break;
            } else {
                printf(".");
            }
            sleep_ms(250);
        } while (false == loop_contents);

        ////////////////////////////////////////////
        // LOOP
        ////////////////////////////////////////////
        DualShock4_state state;
        printf("%s [LOOP]\n", LOG_HEADER);
        while (loop_contents) {
            tight_loop_contents();
            state = controller.get_state();
            if (true == state.linked) {
                
                serialplot::flush(state); // Flush the state to serial port
                serialplot::plot(state); // Plot a sinus
            }
            sleep_ms(20);
        }
        printf("%s [Closing]\n", LOG_HEADER);
    }

    ////////////////////////////////////////////
    // CLOSING
    ////////////////////////////////////////////
    printf("%s [CLOSING] DS4 on PicoW\n", LOG_HEADER);
    stdio_deinit_all();
    return 0;
}
