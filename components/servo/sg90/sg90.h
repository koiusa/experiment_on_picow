#include <stdio.h>
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
#define DF_MOT_DUTY_N70_DEG     0.71F   //-70deg time of high level[ms]              
#define DF_MOT_DUTY_N65_DEG     0.76F   //-65deg time of high level[ms]              
#define DF_MOT_DUTY_N60_DEG     0.82F   //-60deg time of high level[ms]              
#define DF_MOT_DUTY_N30_DEG     1.13F   //-30deg time of high level[ms]              
#define DF_MOT_DUTY_0_DEG       1.45F   //  0deg time of high level[ms]              
#define DF_MOT_DUTY_P30_DEG     1.80F   //+30deg time of high level[ms]              
#define DF_MOT_DUTY_P60_DEG     2.10F   //+60deg time of high level[ms]              
#define DF_MOT_DUTY_P90_DEG     2.40F   //+90deg time of high level[ms]  

class sg90{
    public:
        sg90();
        ~sg90();
        uint begin(void);
        uint servomotor_sg90_init(uint8_t port);
    private:
        static bool set_pwm_50Hz(uint port_num);
        static uint16_t set_pwm_duty(uint16_t period_cycle,float cycletime,float hightime);
};
            
