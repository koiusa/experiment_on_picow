#include "servo_drive_tester.h"



void ServoDriveTester::update()
{
    // begin(); // Initialize the servo if the triangle button is pressed
    float pad = logic::remap(state.r3_y, 1.0f, 255.0f, -10.0f, 10.0f) * -1.0f; // Map the right stick Y-axis to the servo angle
    float goal_angle = std::clamp(servo.get_current_angle() + pad, 0.0f, 90.0f); // Reduce the sensitivity of the left stick
    servo.drive_to_angle(goal_angle);
    if (pad != 0.0f) {
        servo.apply_angle(true);
    }else {
        servo.apply_angle(false);
    }
    printf("%s [R3] %f\n", "SERVO" , servo.get_current_angle());
    
    osc::bundle angle{osc::time()};
    angle << (osc::message{ "/servo" } << float(servo.get_current_angle()));
    udp->send_bundle(angle);

};
