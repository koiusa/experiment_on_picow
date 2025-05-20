
#include "ds4_on_pico_w.hpp"
#include "pico/stdlib.h"

#include "ds4_imu_tester.h"
#include "servo_drive_tester.h"

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
    
    DS4ImuTester ds4_imu_tester;
    ServoDriveTester servo_drive_tester;  
    PicowUDP udp;
    udp.set_wifi_config({ WIFI_SSID, WIFI_PASSWORD });
    udp.set_udp_config({ UDP_TARGET, UDP_PORT });
    ds4_imu_tester.attach(&udp);
    servo_drive_tester.attach(&udp);

    
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
            udp.try_wifi_connect(); // Try to connect to Wi-Fi
            state = controller.get_state();
            ds4_imu_tester.attach(&state);
            servo_drive_tester.attach(&state);

            if (true == state.linked) {
                ds4_imu_tester.update(); // Update the dummy input
                servo_drive_tester.update(); // Update the servo drive tester
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
