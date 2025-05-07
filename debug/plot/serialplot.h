#include "ds4_on_pico_w.hpp"

#include <stdio.h>

class serialplot{
    public:
        serialplot();
        ~serialplot();
        static void plot(const DualShock4_state state);
        static void flush(const DualShock4_state state);
};