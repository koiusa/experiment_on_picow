#include <stdio.h>
#include <algorithm>
#include "pico/stdlib.h"
#include "hardware/structs/pwm.h"
#include "hardware/pwm.h"

//-------------------------------------------------------------------------
//define
//-------------------------------------------------------------------------
#define DF_PWMSIG   2
//servo motor SG90 setting
#define DF_MOT_PERIOD_CYCLE     25000   //value of period cycles
#define DF_MOT_CYCLE_TIME      20.00F   //time of one cycle[ms]
#define DF_MOT_DUTY_N90_DEG     0.50F   //-90deg time of high level[ms]          
#define DF_MOT_DUTY_P90_DEG     2.40F   //+90deg time of high level[ms]  

class sg90{
    public:
        sg90() = default;
        ~sg90() = default;
        void setup(uint8_t port);
        uint begin(void);
        uint servomotor_sg90_init(uint8_t port);
        void change_cycle_time(uint16_t period_cycle);
        void drive_to_angle(float angle);
        float current_angle_ = 0.0f;
        void apply_angle(bool active);
        
        static float remap(float value, float istart, float istop, float ostart, float ostop) { return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));}
    private:
        pwm_config cfg;
        uint8_t port_ = 0;
        uint slice_ = 0;
        float cycle_ = 20.0f;
        bool is_pwm_init = false;
        void set_chan_level(uint16_t count);
        bool set_pwm_init(uint port_num);
        float angle_to_hightime(float angle);
        static uint16_t set_pwm_duty(uint16_t period_cycle,float cycletime,float hightime);

};
            
