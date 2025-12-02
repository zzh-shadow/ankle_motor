#include "open_loop.h"

void open_loop_start(struct OPEN_LOOP_s *open_loop ,struct FOC_s *foc, struct PWM_GEN_s *pwm_gen ,struct USR_CONFIG_s *usr_config)
{
    open_loop->voltage = usr_config->calib_voltage;
    usr_config->encoder_dir = 1;

}

void open_loop_end(struct OPEN_LOOP_s *open_loop , struct FOC_s *foc , struct PWM_GEN_s *pwm_gen)
{

}

void open_loop_loop(struct MOTOR_s *motor)
{
    motor->open_loop.phase_set += M_2PI * CURRENT_MEASURE_PERIOD * 16;
    if(motor->open_loop.phase_set >= M_2PI)
    {
        motor->open_loop.phase_set -= M_2PI;
    }

    motor->foc.voltage(&motor->foc,motor->open_loop.voltage,0,motor->open_loop.phase_set,&motor->pwm_gen);
}
