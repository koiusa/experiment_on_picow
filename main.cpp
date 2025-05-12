
#include "ds4_on_pico_w.hpp"
#include "pico/stdlib.h"

#include "dummyinput.h"

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
    sleep_ms(3000);
    printf("======================\n[SETUP] DS4 on PicoW\n======================\n");
    // controller.setup((DS4forPicoW::config){ .mac_address = "00:00:00:00:00:00" });
    controller.setup();
    
    Dummyinput dummyinput;
    
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
            dummyinput.wifi_connect(); // Try to connect to Wi-Fi
            state = controller.get_state();

            if (true == state.linked) {
                dummyinput.set_State(state); // Set the state to the dummy input
                dummyinput.update(); // Update the dummy input
            }
            sleep_ms(10);
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
