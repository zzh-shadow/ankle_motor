#include "zero_cailbration.h"


void zero_cailbration_start(struct ZERO_CAILBRATION_s *zero_cail,struct USR_CONFIG_s *usr_config)
{
    zero_cail->current_limit_count = 0;
    usr_config->control_mode = CONTROL_MODE_VELOCITY_RAMP;
    usr_config->zero_calib_valid = false;   
}

void zero_cailbration_loop(struct MOTOR_s *motor)
{
    motor->controller.input_velocity_buffer = 3;
    motor->zero_cailbration.cail_time_count++;
    if((motor->zero_cailbration.cail_time_count * motor->encoder.time_diff) > 3) {
        motor->task.statusword_new.errors.zero_cailbration = true;
    }

    if(motor->foc.i_sq > 2.0f && ABS(0 - motor->encoder.vel) < motor->usrconfig.target_velcity_window) {
        motor->zero_cailbration.current_limit_count++;
        if(motor->zero_cailbration.current_limit_count > 50) {
            motor->usrconfig.zero_calib_valid = true;
            motor->task.statecmd = IDLE;
            motor->task.set_state(&motor->task,&motor->enable,&motor->usrconfig);
        }
    }
    else if(motor->zero_cailbration.current_limit_count > 0){
        motor->zero_cailbration.current_limit_count--;
    }

    motor->controller.sync_callback(&motor->controller,&motor->usrconfig,&motor->task);
}

void zero_cailbration_stop(struct ZERO_CAILBRATION_s *zero_cail,struct CONTROLLER_s *controller,struct USR_CONFIG_s *usr_config,struct ENCODER_s* encoder)
{
    controller->input_velocity_buffer = 0;
    if(usr_config->zero_calib_valid) {
        encoder->shadow_count = 1 * ENCODER_CPR;
    }
}

