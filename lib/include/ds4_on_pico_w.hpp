
#ifndef PICO_W_FOR_DS4_HPP
#define PICO_W_FOR_DS4_HPP
#include "struct_ds4_on_pico_w.hpp"

class DS4forPicoW {
public:
public:
    DS4forPicoW();
    ~DS4forPicoW();
    void setup(bool blink_led = true);
    bool scan(int timeout_ms = 3000);
    bool is_use();
    DualShock4_state get_state();

private:
    bool _flag_setup          = false;
    const int TIMEOUT_SPAN_MS = 100;
};

#endif
