#include "servo_drive_tester.h"



void ServoDriveTester::update()
{
    if (debounce_reset.isValid(state->triangle != 0)) { // 現在の状態が前回の状態と異なる場合
        if (debounce_reset.isNotBounce()) { // 200msのデバウンス
            begin();
            debounce_reset.pressed(); // 最後に押された時間を更新
        }
    }
    
    float pad_y = logic::remap(state->r3_y, 1.0f, 255.0f, -10.0f, 10.0f) * -1.0f; // Map the right stick Y-axis to the servo angle
    drive(servo_y, pad_y, 0.0f, 90.0f); // Update the servo Y-axis

    float pad_x = logic::remap(state->r3_x, 1.0f, 255.0f, -10.0f, 10.0f) * 2.0f; // Map the right stick Y-axis to the servo angle
    drive(servo_z, pad_x,-90.0f, 90.0f); // Update the servo Z-axis
    
    printf("%s [R3] %f:%f\n", "SERVO" , servo_y.get_current_angle(), servo_z.get_current_angle());
    osc::bundle angle{osc::time()};
    angle << (osc::message{ "/servo" } << float(servo_y.get_current_angle()) << float(servo_z.get_current_angle()));
    udp->send_bundle(angle); // Add the message to the UDP queue

    debounce_reset.saveState(state->triangle != 0); // Save the state of the triangle button
};


