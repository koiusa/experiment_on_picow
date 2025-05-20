#ifndef SERVO_DRIVE_TESTER_H
#define SERVO_DRIVE_TESTER_H

#include <algorithm>
#include <stdio.h>
#include "pico/stdlib.h"
#include "ds4_on_pico_w.hpp"
#include "sg90.h"
#include "picow_udp.h"
#include "Ids4state.h"

class ServoDriveTester : public PicowUDP::IUdpListener, public IDs4state
{
    public:
        ServoDriveTester() {
            servo.setup(DF_PWMSIG); // Initialize the servo on the specified pin
        }
        ~ServoDriveTester() = default;

        void update();
        void attach(PicowUDP* udp) override { this->udp = udp; };
        void attach(const DualShock4_state* state) override { this->state = state; };
    private:
        sg90 servo; 
        void begin() { if (state->triangle != 0) { servo.reset(); servo.setup(DF_PWMSIG);} }
};

#endif // SERVO_DRIVE_TESTER_H
