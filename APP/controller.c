#include "controller.h"

int controller_set_home(struct CONTROLLER_s* controller,struct ENCODER_s* encoder,struct TASK_s* task,struct TRAJ_s* traj)
{
    int ret = 0;

    if(task->fsm.state == IDLE){
        encoder->shadow_count = 0;
        encoder->pos = 0;
    }
    else if(task->fsm.state == RUN && ABS(encoder->vel) < 0.5f){
        __disable_irq();
        controller->input_position = 0;
        controller->input_position_buffer = 0;
        controller->pos_setpoint = 0;
        __enable_irq();
    }
    else{
        ret = -1;
    }
    return ret;
}


void controller_sync_callback(struct CONTROLLER_s* controller,struct USR_CONFIG_s* usr_config,struct TASK_s* task)
{
    if(task->fsm.state != RUN && task->fsm.state != ZERO_CAILBRATION){
        return;
    }

    switch(usr_config->control_mode){
        case CONTROL_MODE_TORQUE_RAMP:
        {
            controller->input_torque = controller->input_torque_buffer;
            break;
        }
        case CONTROL_MODE_VELOCITY_RAMP:
        {
            controller->input_velocity = controller->input_velocity_buffer;
            break;
        }
        case CONTROL_MODE_POSITION_FILTER:
        {
            controller->input_position = controller->input_position_buffer;
            break;
        }
        case CONTROL_MODE_POSITION_PROFILE:
        {
            if(controller->input_position != controller->input_position_buffer){
                controller->input_updated = true;
                controller->input_position = controller->input_position_buffer;
                controller->input_velocity = ABS(controller->input_velocity_buffer);
                controller->input_time = controller->input_time_buffer;
            }
            break;
        }
        default:
            break;
    }
    task->statusword_new.status.target_reached = 0;
    task->statusword_old.status.target_reached = 0;
}




void controller_update_input_pos_filter_gain(struct CONTROLLER_s* controller,float bw)
{
    float bandwidth = MIN(bw, 0.25f * PWM_FREQUENCY);
    controller->input_pos_filter_ki = 0.5f * bandwidth;
    controller->input_pos_filter_kp = 10.0 * bandwidth;
}


void controller_init(struct CONTROLLER_s* controller,struct USR_CONFIG_s* usr_config)
{
    controller_update_input_pos_filter_gain(controller,usr_config->position_filter_bw);
}


void controller_reset(struct CONTROLLER_s* controller,struct ENCODER_s* encoder,struct TRAJ_s* traj,struct USR_CONFIG_s* usr_config)
{
    float pos_meas = 0;

    pos_meas = encoder->pos;

    controller->input_position = pos_meas;
    controller->input_velocity = 0.0f;
    controller->input_torque = 0.0f;

    controller->input_position_buffer = pos_meas;
    controller->input_velocity_buffer = 0.0f;
    controller->input_torque_buffer = 0.0f;

    controller->pos_setpoint = pos_meas;
    controller->vel_setpoint = 0.0f;
    controller->torque_setpoint = 0.0f;

    controller->vel_des = 0;

    controller->vel_integrator_torque = 0;
    controller->torque_limit = usr_config->current_limit * usr_config->torque_constant;

    controller->input_updated = false;

    traj->stop_flg = true;
    traj->input_pos = pos_meas;
    traj->vel_set = 0;


}

void controller_loop(struct MOTOR_s* motor)
{
    motor->controller.pos_meas = motor->encoder.pos / motor->usrconfig.encoder_gr;
    motor->task.statusword_new.status.target_reached = 0;
    
    if(motor->task.fsm.state == RUN || motor->task.fsm.state == ZERO_CAILBRATION){
        switch(motor->usrconfig.control_mode){
            case CONTROL_MODE_TORQUE_RAMP:
            {
                float max_step_size = ABS(LOW_TASK_TIME_DIFF * motor->usrconfig.torque_ramp_rate);
                float full_step = motor->controller.input_torque - motor->controller.torque_setpoint;
                float step = CLAMP(full_step , -max_step_size, max_step_size);
                motor->controller.torque_setpoint += step;
                break;
            }

            case CONTROL_MODE_VELOCITY_RAMP:
            {
                float max_step_size = ABS(LOW_TASK_TIME_DIFF * motor->usrconfig.velocity_ramp_rate);
                if(motor->task.fsm.state == RUN) {
                    if(motor->usrconfig.invert_motor_dir == 1){
                        if(((motor->encoder.pos + motor->usrconfig.position_limit ) <= 0.1) && (motor->controller.input_velocity < 0 )){
                            motor->controller.input_velocity = 0;
                        }
                        else if(((motor->encoder.pos + motor->usrconfig.position_limit ) >= 12.4) && (motor->controller.input_velocity > 0 )) {
                            motor->controller.input_velocity = 0;
                        }
                    }
                    else if(motor->usrconfig.invert_motor_dir == 0) {
                        if(((motor->encoder.pos - motor->usrconfig.position_limit ) <= 0.1) && (motor->controller.input_velocity > 0 )){
                            motor->controller.input_velocity = 0;
                        }
                        else if(((motor->encoder.pos - motor->usrconfig.position_limit ) >= 12.4) && (motor->controller.input_velocity < 0 )) {
                            motor->controller.input_velocity = 0;
                        }
                    }
                }
                float full_step = motor->controller.input_velocity - motor->controller.vel_setpoint;
                float step = CLAMP(full_step , -max_step_size, max_step_size);
                motor->controller.vel_setpoint += step;
                motor->controller.torque_setpoint = 0;
                if(motor->task.statusword_new.status.target_reached == 0 
                    && ABS(motor->controller.input_velocity - motor->encoder.vel) < motor->usrconfig.target_velcity_window){
                    motor->task.statusword_new.status.target_reached = 1;
                }
                motor->controller.vel_des = motor->controller.vel_setpoint;
                break;
            }

            case CONTROL_MODE_POSITION_FILTER:
            {
                motor->controller.input_velocity = 0;
                float delta_pos = motor->controller.input_position - motor->controller.pos_setpoint;
                float delta_vel = motor->controller.input_velocity - motor->controller.vel_setpoint;
                float accel = motor->controller.input_pos_filter_kp * delta_pos + motor->controller.input_pos_filter_ki * delta_vel;

                motor->controller.vel_setpoint += LOW_TASK_TIME_DIFF * accel;
                motor->controller.pos_setpoint += LOW_TASK_TIME_DIFF * motor->controller.vel_setpoint;
                
                if(motor->task.statusword_new.status.target_reached == 0) {
                    if(ABS(motor->controller.input_position - motor->controller.pos_meas) < motor->usrconfig.target_position_window){
                        motor->task.statusword_new.status.target_reached = 1;
                    }
                }
                float pos_err = motor->controller.pos_setpoint - motor->controller.pos_meas;
                if(motor->task.statusword_new.status.target_reached == 1) {
                    motor->controller.vel_des = 0;
                }
                else {
                    motor->controller.vel_des = motor->usrconfig.pos_gain * pos_err;
                }
                break;
            }

            case CONTROL_MODE_POSITION_PROFILE:
            {
                if(motor->controller.input_updated) {
                    if(motor->traj.plan(&motor->traj,motor->controller.input_position,motor->encoder.pos,motor->controller.input_velocity,motor->encoder.vel,motor->controller.input_time
                        ,motor->usrconfig.profile_accel,motor->usrconfig.profile_decel,motor->usrconfig.profile_velocity) == 0) {
                            motor->controller.input_updated = false;
                            motor->task.statusword_new.status.traj_status = 1;
                        }
                    else {
                        motor->controller.input_updated = false; //to do error
                        motor->task.statusword_new.status.traj_status = 0;
                    }
                }
                else {
                    motor->traj.eval(&motor->traj,&motor->encoder,&motor->controller,&motor->usrconfig);
                }

                if(motor->task.statusword_new.status.target_reached == 0) {
                    if(ABS(motor->traj.input_pos - motor->controller.pos_meas) < motor->usrconfig.target_position_window){
                        motor->task.statusword_new.status.target_reached = 1;
                        motor->controller.vel_des = 0;
                    }
                }
                break;
            }

            default:
                break;
        }

    }

    motor->controller.vel_des = CLAMP(motor->controller.vel_des, -motor->usrconfig.velocity_limit , +motor->usrconfig.velocity_limit);

    motor->controller.torque = motor->controller.torque_setpoint;

    if(motor->usrconfig.control_mode >= CONTROL_MODE_VELOCITY_RAMP) {
        motor->controller.v_err = motor->controller.vel_des - motor->encoder.vel;
        motor->controller.torque += motor->usrconfig.vel_gain * motor->controller.v_err;
        
        // Velocity integral action before limiting
        motor->controller.torque += motor->controller.vel_integrator_torque;
    }
    
    if(motor->usrconfig.control_mode < CONTROL_MODE_VELOCITY_RAMP){
        motor->controller.vel_des = CLAMP(motor->controller.vel_des, -30 ,30);
        float Tmax = (+30 - motor->encoder.vel) * motor->usrconfig.vel_gain;
        float Tmin = (-30 - motor->encoder.vel) * motor->usrconfig.vel_gain;
        motor->controller.torque = CLAMP(motor->controller.torque,Tmin,Tmax);
    }
    if(motor->foc.i_sq < (SQ(motor->controller.torque_limit) - 1)) {
        motor->controller.torque_limit = motor->usrconfig.protect_over_current * motor->usrconfig.torque_constant;
    }
    else {
        motor->controller.torque_limit = motor->usrconfig.current_limit * motor->usrconfig.torque_constant;
    }
    if (ABS(motor->controller.torque) > motor->controller.torque_limit){
        motor->controller.torque = CLAMP(motor->controller.torque,-motor->controller.torque_limit,motor->controller.torque_limit);
        motor->task.statusword_new.status.current_limit_active = 1;
    }
    else if ((ABS(motor->controller.torque) < (0.9f * motor->usrconfig.current_limit * motor->usrconfig.torque_constant))){
        motor->task.statusword_new.status.current_limit_active = 0;
    }


    if(motor->usrconfig.control_mode < CONTROL_MODE_VELOCITY_RAMP){
        motor->controller.vel_integrator_torque = 0.0f;
    }
    else{
        if(motor->task.statusword_new.status.current_limit_active){
            motor->controller.vel_integrator_torque *= 0.99f;
        }
        else{
            motor->controller.vel_integrator_torque += (motor->usrconfig.vel_integrator_gain * LOW_TASK_TIME_DIFF) * motor->controller.v_err;
        }
    }

    motor->controller.i_q_set = motor->controller.torque / motor->usrconfig.torque_constant;
    
    // motor->foc.current(&motor->foc,0, motor->controller.i_q_set, motor->encoder.phase, motor->encoder.phase_vel,&motor->pwm_gen,&motor->task,&motor->usrconfig,&motor->encoder);
}
