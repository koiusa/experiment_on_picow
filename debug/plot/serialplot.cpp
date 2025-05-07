#include "serialplot.h"

void serialplot::plot(DualShock4_state state) {
        // Plot a sinus
        printf(">l3_xy:%3d:%3d|xy\n",state.l3_x,state.l3_y);
        printf(">r2_value:%03d\n",state.r2_value);
        printf(">r1:%3d\n",state.r1);
        printf(">r2:%3d\n",state.r2);
        printf(">r3:%3d\n",state.r3);
        printf(">r3_xy:%3d:%3d|xy\n",state.r3_x,state.r3_y);
        printf(">l2_value:%03d\n",state.l2_value);
        printf(">l1:%3d\n",state.l1);
        printf(">l2:%3d\n",state.l2);
        printf(">l3:%3d\n",state.l3);

        printf(">hat:%3d\n",state.hat);
        printf(">share:%3d\n",state.share);
        printf(">options:%3d\n",state.options);
        printf(">ps:%3d\n",state.ps);
        printf(">cross:%3d\n",state.cross);
        printf(">circle:%3d\n",state.circle);
        printf(">square:%3d\n",state.square);
        printf(">triangle:%3d\n",state.triangle);
        printf(">touch:%3d\n",state.touch);
        printf(">mute:%3d\n",state.mute);
        printf(">fn1:%3d\n",state.fn1);
        printf(">fn2:%3d\n",state.fn2);

        printf(">touch_packet_size:%02d\n",state.touch_packet_size);
        printf(">touch_timestamp:%3d\n",state.touch_timestamp);

        printf(">touch_f1_active:%3d\n",state.touch_f1_active);
        printf(">touch_f1_counter:%3d\n",state.touch_f1_counter);
        printf(">touch_f1_xy:%04d:%03d|xy\n",state.touch_f1_x,state.touch_f1_y);
        printf(">touch_f2_active:%3d\n",state.touch_f2_active);
        printf(">touch_f2_counter:%3d\n",state.touch_f2_counter);
        printf(">touch_f2_xy:%04d:%03d|xy\n",state.touch_f2_x,state.touch_f2_y);

        printf(">battery_level:%02d\n", state.battery_level);
        printf(">battery:%03d\n", state.battery);

        printf(">3D|Gyro_xyz:S:cube:P:0:0:0:R:%6d:%6d:%6d:W:2:H:2:D:2:C:red\n", state.gyro_x, state.gyro_y, state.gyro_z);
        printf(">Gyro_x:%6d\n", state.gyro_x);
        printf(">Gyro_y:%6d\n", state.gyro_y);
        printf(">Gyro_z:%6d\n", state.gyro_z);
        printf(">3D|Accel:S:sphere:P:%6d:%6d:%6d:RA:0:C:red\n", state.accel_x, state.accel_y, state.accel_z);
        printf(">Accel_x:%6d\n", state.accel_x);
        printf(">Accel_y:%6d\n", state.accel_y);
        printf(">Accel_z:%6d\n", state.accel_z);
        
        printf(">Linked:%3d\n", state.linked);
        printf(">USB:%3d\n", state.connected_usb);
        printf(">MIC:%3d\n", state.connected_mic);
        printf(">Phone:%3d\n", state.connected_phone);
        printf(">ID:%02d", state.report_id);
        printf(">TS:%05d", state.timestamp);
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