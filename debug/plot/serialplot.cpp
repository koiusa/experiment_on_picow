#include "serialplot.h"

void serialplot::plot(DualShock4_state state) {
        // Plot a sinus
        printf(">ds4_trigger_l3_xy:%3d:%3d|xy,np\n",state.l3_x,state.l3_y);
        printf(">ds4_trigger_r2_value:%03d|np\n",state.r2_value);
        printf(">ds4_trigger_r1:%3d|np\n",state.r1);
        printf(">ds4_trigger_r2:%3d|np\n",state.r2);
        printf(">ds4_trigger_r3:%3d|np\n",state.r3);
        printf(">ds4_trigger_r3_xy:%3d:%3d|xy,np\n",state.r3_x,state.r3_y);
        printf(">ds4_trigger_l2_value:%03d|np\n",state.l2_value);
        printf(">ds4_trigger_l1:%3d|np\n",state.l1);
        printf(">ds4_trigger_l2:%3d|np\n",state.l2);
        printf(">ds4_trigger_l3:%3d|np\n",state.l3);

        printf(">ds4_button_hat:%3d|np\n",state.hat);
        printf(">ds4_button_share:%3d|np\n",state.share);
        printf(">ds4_button_options:%3d|np\n",state.options);
        printf(">ds4_button_ps:%3d|np\n",state.ps);
        printf(">ds4_button_cross:%3d|np\n",state.cross);
        printf(">ds4_button_circle:%3d|np\n",state.circle);
        printf(">ds4_button_square:%3d|np\n",state.square);
        printf(">ds4_button_triangle:%3d|np\n",state.triangle);
        printf(">ds4_touch:%3d|np\n",state.touch);
        printf(">ds4_othor_mute:%3d|np\n",state.mute);
        printf(">ds4_othor_fn1:%3d|np\n",state.fn1);
        printf(">ds4_othor_fn2:%3d|np\n",state.fn2);

        printf(">ds4_touch_packet_size:%02d|np\n",state.touch_packet_size);
        printf(">ds4_touch_timestamp:%3d|np\n",state.touch_timestamp);

        printf(">ds4_touch_f1_active:%3d|np\n",state.touch_f1_active);
        printf(">ds4_touch_f1_counter:%3d|np\n",state.touch_f1_counter);
        printf(">ds4_touch_f1_xy:%04d:%03d|xy,np\n",state.touch_f1_x,state.touch_f1_y);
        printf(">ds4_touch_f2_active:%3d|np\n",state.touch_f2_active);
        printf(">ds4_touch_f2_counter:%3d|np\n",state.touch_f2_counter);
        printf(">ds4_touch_f2_xy:%04d:%03d|xy,np\n",state.touch_f2_x,state.touch_f2_y);

        printf(">ds4_battery_level:%02d|np\n", state.battery_level);
        printf(">ds4_battery:%03d|np\n", state.battery);

        printf(">ds4_sensor_Gyro_x:%6d|np\n", state.gyro_x);
        printf(">ds4_sensor_Gyro_y:%6d|np\n", state.gyro_y);
        printf(">ds4_sensor_Gyro_z:%6d|np\n", state.gyro_z);
        printf(">ds4_sensor_Accel_x:%6d|np\n", state.accel_x);
        printf(">ds4_sensor_Accel_y:%6d|np\n", state.accel_y);
        printf(">ds4_sensor_Accel_z:%6d|np\n", state.accel_z);
        
        printf(">ds4_system_Linked:%3d|np\n", state.linked);
        printf(">ds4_system_USB:%3d|np\n", state.connected_usb);
        printf(">ds4_system_MIC:%3d|np\n", state.connected_mic);
        printf(">ds4_system_Phone:%3d|np\n", state.connected_phone);
        printf(">ds4_system_ID:%02d|np\n", state.report_id);
        printf(">ds4_system_TS:%05d|np\n", state.timestamp);
};

void serialplot::flush(DualShock4_state state) {
    printf("[%01d%s%s%s%s%s%s%s%s%s%s%s] ", //
        state.hat,
        state.share ? "S" : " ",
        state.options ? "O" : " ",
        state.ps ? "P" : " ",
        state.cross ? "A" : " ",
        state.circle ? "B" : " ",
        state.square ? "X" : " ",
        state.triangle ? "Y" : " ",
        state.touch ? "T" : " ",
        state.mute ? "M" : " ",
        state.fn1 ? "1" : " ",
        state.fn2 ? "2" : " ");

    printf("L[%s%s%s(%03d)(%03d,%03d)] ", //
        state.l1 ? "1" : " ",
        state.l2 ? "2" : " ",
        state.l3 ? "3" : " ",
        state.l2_value,
        state.l3_x,
        state.l3_y);
    printf("R[%s%s%s(%03d)(%03d,%03d)] ", //
        state.r1 ? "1" : " ",
        state.r2 ? "2" : " ",
        state.r3 ? "3" : " ",
        state.r2_value,
        state.r3_x,
        state.r3_y);

    printf("Touch[%02d/%3d] ", state.touch_packet_size, state.touch_timestamp);
    printf("T1[%s/%3d(%04d,%03d)] ", state.touch_f1_active ? "x" : " ", state.touch_f1_counter, state.touch_f1_x, state.touch_f1_y);
    printf("T2[%s/%3d(%04d,%03d)] ", state.touch_f2_active ? "x" : " ", state.touch_f2_counter, state.touch_f2_x, state.touch_f2_y);
    #if 0
    printf("T1B[%s/%3d(%04d,%03d)] ", state.touch_f1_spare_active ? "x" : " ", state.touch_f1_spare_counter, state.touch_f1_spare_x, state.touch_f1_spare_y);
    printf("T2B[%s/%3d(%04d,%03d)] ", state.touch_f2_spare_active ? "x" : " ", state.touch_f2_spare_counter, state.touch_f2_spare_x, state.touch_f2_spare_y);
    #endif
    printf("BtLv.%02d(%03d) ", state.battery_level, state.battery);
    printf("Gyro[%6d,%6d,%6d] ", state.gyro_x, state.gyro_y, state.gyro_z);
    printf("Accel[%6d,%6d,%6d] ", state.accel_x, state.accel_y, state.accel_z);
    printf("%s", state.linked ? " Linked " : "Unlinked");
    printf("in[%s/%s/%s] ", state.connected_usb ? "USB" : "   ", state.connected_mic ? "MIC" : "   ", state.connected_phone ? "Phone" : "     ");
    printf("ID.%02d ", state.report_id);
    printf("TS[%05d] ", state.timestamp);

    printf("\n");

};