#ifndef SERVO_DRIVE_TESTER_H
#define SERVO_DRIVE_TESTER_H

#include <algorithm>
#include <stdio.h>
#include "pico/stdlib.h"
#include "ds4_on_pico_w.hpp"
#include "sg90.h"
#include "picow_udp.h"
#include "Ids4state.h"
#include "debounce.h"

#define DF_PWMSIG_YAW   1
#define DF_PWMSIG_PITCH   2
class ServoDriveTester : public PicowUDP::IUdpListener, public IDs4state
{
    public:
        ServoDriveTester() {
            servo_y.setup(DF_PWMSIG_PITCH); // Initialize the servo on the specified pin
            servo_z.setup(DF_PWMSIG_YAW); // Initialize the servo on the specified pin
        }
        ~ServoDriveTester() = default;

        void update();
        void attach(PicowUDP* udp) override { this->udp = udp; };
        void attach(const DualShock4_state* state) override { this->state = state; };
    private:
        sg90 servo_y;
        sg90 servo_z;  
        debounce debounce_reset;
        void begin() { 
            if (state->triangle != 0) {
                servo_y.setup(DF_PWMSIG_PITCH);
                servo_z.setup(DF_PWMSIG_YAW);}
            }
        void drive(sg90& servo ,float input_value, float min_angle = -90.0f, float max_angle = 90.0f) {
            float goal_angle = std::clamp(servo.get_current_angle() + input_value, min_angle, max_angle); // Reduce the sensitivity of the left stick
            servo.drive_to_angle(goal_angle);
            if (input_value != 0.0f) {
                servo.apply_angle(true);
            }else {
                servo.apply_angle(false);
            }    
        }
};

#endif // SERVO_DRIVE_TESTER_H
