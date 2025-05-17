#include "sg90.h"

void sg90::setup(uint8_t port) {
    if (is_pwm_init) {
        printf("PWM is already initialized.\n");
        return;
    }
    // Set the PWM frequency to 50Hz
    set_pwm_init(port);
}

void sg90::change_cycle_time(uint16_t period_cycle) {
    // Set the period cycle and cycle time
    pwm_set_wrap(slice_, (period_cycle-1));
    pwm_set_clkdiv(slice_, 100.0f);
}

float sg90::angle_to_hightime(float angle) {
    return remap(angle, -90.0f, 90.0f, DF_MOT_DUTY_N90_DEG, DF_MOT_DUTY_P90_DEG);
}

void sg90::drive_to_angle(float angle) {
    // Set the duty cycle based on the angle
    float hightime = angle_to_hightime(angle);
    uint16_t count = set_pwm_duty(DF_MOT_PERIOD_CYCLE, DF_MOT_CYCLE_TIME, hightime);
    set_chan_level(count);
}

void sg90::apply_angle(bool active) {
    pwm_set_enabled(slice_, active);
}

//-------------------------------------------------------------------------
//function     : main
//return       : ---
//-------------------------------------------------------------------------
uint sg90::begin() {

    //set pwm duty(-70[deg])
    drive_to_angle(90.0f);
    // Set the PWM running
    pwm_set_enabled(slice_, true);
    //wait
    busy_wait_ms(180);
    // Set the PWM stop
    pwm_set_enabled(slice_, false);

    return(slice_);
};

//-------------------------------------------------------------------------
//function: pwm free count(50Hz cycle)setting
//port_num: using port number
//return  : true=success/false=fault
//-------------------------------------------------------------------------
bool sg90::set_pwm_init(uint port_num)
{
    if(port_num>=30)
    {
        printf("Error: port number is over 30\n");
        return(false);
    }

    // Tell GPIO port number this is allocated to the PWM
    gpio_set_function(port_num,GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to GPIO port number (it's slice 0)
    slice_ = pwm_gpio_to_slice_num(port_num);

    // get default pwm confg
    cfg = pwm_get_default_config();

    // set pwm config modified div mode and div int value
    pwm_config_set_clkdiv_mode(&cfg,PWM_DIV_FREE_RUNNING);
    pwm_config_set_clkdiv_int(&cfg,100);
    pwm_init(slice_,&cfg,false);

    drive_to_angle(0.0f);
    // Set the PWM running
    pwm_set_enabled(slice_, true);
    //wait 1[s]
    busy_wait_ms(1000);
    // Set the PWM stop
    pwm_set_enabled(slice_, false);
    is_pwm_init = true;

    return(true);

};

void sg90::set_chan_level(uint16_t count)
{
    // Set channel A or B output high for one cycle before dropping
    if(port_ % 2)
    {   //odd number
        pwm_set_chan_level(slice_, PWM_CHAN_B, count);
    }
    else
    {   //even number
        pwm_set_chan_level(slice_, PWM_CHAN_A, count);
    }
};

//-------------------------------------------------------------------------
//function     : pwm duty value
//period_cycle : value of period cycles
//cycletime    : time of one cycle [ms]
//hightime     : time of high level[ms]
//return       : level for the selected output
//-------------------------------------------------------------------------
uint16_t sg90::set_pwm_duty(uint16_t period_cycle,float cycletime,float hightime)
{
    float count_pms = (float)period_cycle / cycletime * hightime;
    return((uint16_t)count_pms);
};
