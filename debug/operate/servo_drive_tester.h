#ifndef SERVO_DRIVE_TESTER_H
#define SERVO_DRIVE_TESTER_H

#include <algorithm>
#include <stdio.h>
#include "pico/stdlib.h"
#include "ds4_on_pico_w.hpp"
#include "sg90.h"

class ServoDriveTester
{
    public:
        ServoDriveTester() {
            servo.setup(DF_PWMSIG); // Initialize the servo on the specified pin
        }
        ~ServoDriveTester() = default;

        void set_State(const DualShock4_state& state) { this->state = state; }
        void update();
    private:
        DualShock4_state state = {0};
        sg90 servo; 
        void begin() { if (state.triangle != 0) { servo.reset(); servo.setup(DF_PWMSIG);} }
};

#endif // SERVO_DRIVE_TESTER_H
