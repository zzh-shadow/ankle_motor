#include "task.h"


void task_init(struct TASK_s *task)
{
    task->fsm.state = BOOT_UP;
    task->fsm.state_next = BOOT_UP;
    task->fsm.state_next_ready = 0;

    task->statusword_new.status.status_code = 0;
    task->statusword_new.errors.errors_code = 0;
    task->statusword_old = task->statusword_new;

}

void task_reset_error(struct TASK_s *task)
{
    task->statusword_new.errors.errors_code &= 0;
    task->statusword_old.errors.errors_code &= 0;
}

int task_set_state(struct TASK_s *task,struct ENABLE_s *enable,struct USR_CONFIG_s *usrconfig)
{
    int ret = 0;

    if(task->fsm.state_next == task->statecmd && task->charge_boot_cat_delay != 0){
        return ret;
    }

    switch (task->fsm.state){
        case BOOT_UP:{
            if(task->statecmd == IDLE){
                task->fsm.state_next = IDLE;
            }
            else{
                ret = -1;
            }
            break;
        }
        case IDLE:{
            switch (task->statecmd){
                case IDLE:{
                    task->charge_boot_cat_delay = 0;
                    task->fsm.state_next = IDLE;
                    break;
                }
                case RUN:{
                    if(task->statusword_new.errors.errors_code != 0){
                        ret = -2;
                    }
                    else if(!usrconfig->calib_valid){
                        ret = -3;
                    }
                    else if(!usrconfig->zero_calib_valid){
                        ret = -4;
                    }
                    else{
                        task->fsm.state_next = RUN;
                        task->charge_boot_cat_delay = CHARGE_BOOT_CAP_TICKS;
                        enable->boost_enable(enable,true);
                    }
                    break;
                }
                case CALIBRATION:{
                    if(task->statusword_new.errors.errors_code != 0){
                        ret = -2;
                    }
                    else{
                        task->fsm.state_next = CALIBRATION;
                        task->charge_boot_cat_delay = CHARGE_BOOT_CAP_TICKS;
                        enable->boost_enable(enable,true);
                    }
                    break;
                }
                case ZERO_CAILBRATION:{
                    if(task->statusword_new.errors.errors_code != 0){
                        ret = -2;
                    }
                    else if(!usrconfig->calib_valid) {
                        ret = -3;
                    }
                    else{
                        task->fsm.state_next = ZERO_CAILBRATION;
                        task->charge_boot_cat_delay = CHARGE_BOOT_CAP_TICKS;
                        enable->boost_enable(enable,true);
                    }
                    break;
                }
                case ERRORCLEAR:{
                    if(task->statusword_new.errors.errors_code == 0){
                        ret = -1;
                    }
                    else{
                        task->charge_boot_cat_delay = 0;
                        task->fsm.state_next = ERRORCLEAR;
                    }
                    break;
                }
                case OPEN_LOOP_RUN:{
                    if(task->statusword_new.errors.errors_code != 0){
                        ret = -1;
                    }
                    else{
                        task->fsm.state_next = OPEN_LOOP_RUN;
                        enable->boost_enable(enable,true);
                        task->charge_boot_cat_delay = CHARGE_MOTOR_ENABLE_CAP_TICKS;
                    }
                    break;
                }

                default:{
                    ret = -1;
                    break;
                }
            }
            break;
        }
        default:{
            if(task->statecmd == IDLE){
                task->charge_boot_cat_delay = 0;
                task->fsm.state_next = IDLE;
            }
            else{
                ret = -1;
            }
            break;
        }
    }
    task->fsm.state_next_ready = 0;

    return ret;
}


void task_enter_state(struct MOTOR_s *motor)
{
    switch(motor->task.fsm.state_next){
        case BOOT_UP:{
            break;
        }
        case IDLE:{
            break;
        }
        case RUN:{
            motor->foc.arm(&motor->foc,&motor->enable,&motor->pwm_gen);
            motor->controller.reset(&motor->controller,&motor->encoder,&motor->traj,&motor->usrconfig);
            motor->task.statusword_new.status.operation_enabled = 1;
            motor->task.statusword_new.status.current_limit_active = 0;
            motor->task.statusword_old.status = motor->task.statusword_new.status;

            break;
        }
        case OPEN_LOOP_RUN:{
            motor->foc.arm(&motor->foc,&motor->enable,&motor->pwm_gen);
            motor->open_loop.start(&motor->open_loop,&motor->foc , &motor->pwm_gen ,&motor->usrconfig);
            break;
        }
        case CALIBRATION:{
            motor->foc.arm(&motor->foc,&motor->enable,&motor->pwm_gen);
            motor->calibration.calibration_start(&motor->calibration,&motor->usrconfig);
            break;
        }
        case ZERO_CAILBRATION:{
            motor->foc.arm(&motor->foc,&motor->enable,&motor->pwm_gen);
            motor->zero_cailbration.start(&motor->zero_cailbration,&motor->usrconfig);
            motor->controller.reset(&motor->controller,&motor->encoder,&motor->traj,&motor->usrconfig);
            motor->task.statusword_new.status.operation_enabled = 1;
            motor->task.statusword_new.status.current_limit_active = 0;
            motor->task.statusword_old.status = motor->task.statusword_new.status;
            break;
        }
        case ERRORCLEAR:{
            motor->task.reset_error(&motor->task);
            break;
        }
        default:
            break;
    }
}

void task_exit_state(struct MOTOR_s *motor)
{
    switch(motor->task.fsm.state)
    {
        case BOOT_UP:
        {
            motor->task.fsm.state_next_ready = 1;
            break;
        }
        case IDLE:
        {
            if(motor->task.charge_boot_cat_delay){
                motor->task.charge_boot_cat_delay--;
            }
            else{
                motor->task.fsm.state_next_ready = 1;
                motor->foc.disarm(&motor->foc,&motor->enable,&motor->pwm_gen);
            }
            break;
        }
        case RUN:
        {
            motor->foc.disarm(&motor->foc,&motor->enable,&motor->pwm_gen);
            motor->task.statusword_new.status.operation_enabled = 0;
            motor->task.statusword_new.status.target_reached = 0;
            motor->task.statusword_new.status.current_limit_active = 0;
            motor->task.statusword_old.status = motor->task.statusword_new.status;
            motor->task.fsm.state_next_ready = 1;
            break;
        }
        case OPEN_LOOP_RUN:
        {
            motor->foc.disarm(&motor->foc,&motor->enable,&motor->pwm_gen);
            motor->open_loop.end(&motor->open_loop,&motor->foc,&motor->pwm_gen);
            motor->task.fsm.state_next_ready = 1;
            break;
        }
        case CALIBRATION:
        {
            motor->foc.disarm(&motor->foc,&motor->enable,&motor->pwm_gen);
            motor->calibration.calibration_end(&motor->calibration,&motor->foc,&motor->pwm_gen);
            motor->task.fsm.state_next_ready = 1;
            break;
        }
        case ZERO_CAILBRATION:
        {
            motor->foc.disarm(&motor->foc,&motor->enable,&motor->pwm_gen);
            motor->zero_cailbration.end(&motor->zero_cailbration,&motor->controller,&motor->usrconfig,&motor->encoder);
            motor->task.fsm.state_next_ready = 1;
            break;
        }
        case ERRORCLEAR:
        {
            motor->task.fsm.state_next_ready = 1;
            break;
        }
    }
}

void task_safety_task(struct TASK_s *task , struct USR_CONFIG_s *usr_config,struct FOC_s *foc)
{
    if(task->fsm.state != BOOT_UP){
        if(foc->v_bus > usr_config->protect_over_voltage){
            task->statusword_new.errors.over_voltage = 1;
        }

        if(foc->v_bus < usr_config->protect_under_voltage){
            task->statusword_new.errors.under_voltage = 1;
        }
    }
}

void task_low_priority_task(struct MOTOR_s *motor)
{
    if(motor->task.statusword_old.status.status_code != motor->task.statusword_new.status.status_code){
        motor->task.statusword_old.status = motor->task.statusword_new.status;
    }

    if(motor->task.statusword_old.errors.errors_code != motor->task.statusword_new.errors.errors_code){
        if(motor->task.statusword_new.errors.errors_code == 0){
            motor->foc.disarm(&motor->foc,&motor->enable,&motor->pwm_gen);
            motor->task.statecmd = IDLE;
            motor->task.set_state(&motor->task,&motor->enable,&motor->usrconfig);
        }
        motor->task.statusword_old.errors.errors_code = motor->task.statusword_new.errors.errors_code;
    }

    if(motor->task.fsm.state_next != motor->task.fsm.state){
        motor->task.exit_state(motor);
        if(motor->task.fsm.state_next_ready){
            motor->task.fsm.state = motor->task.fsm.state_next;
            motor->task.enter_state(motor);
        }
    }

    if(motor->task.fsm.state == RUN || motor->task.fsm.state == ZERO_CAILBRATION) {
        motor->controller.loop(motor);
    }
    if(motor->task.fsm.state != BOOT_UP) {
        motor->foc.i_sq = 0.6666667 * (SQ(motor->foc.i_a) + SQ(motor->foc.i_b) + SQ(motor->foc.i_c));
        if(motor->foc.i_sq > SQ(motor->usrconfig.protect_over_current)) {
            motor->foc.over_current_time++;
            if(motor->foc.over_current_time >= 10) {
                motor->foc.disarm(&motor->foc,&motor->enable,&motor->pwm_gen);
                motor->task.statecmd = IDLE;
                motor->task.statusword_new.errors.over_phasecurrent = 1;

            }
        }
        else if(motor->foc.over_current_time > 0){
            motor->foc.over_current_time--;
        }
    }

    motor->task.test_error_code = motor->task.statusword_new.errors.errors_code;

    motor->main_comm.loop(&motor->main_comm,&motor->usrconfig,&motor->task,&motor->encoder,&motor->controller,&motor->enable);
    
}

void task_high_frequency_task(struct MOTOR_s *motor)
{
    motor->encoder.loop(&motor->encoder,&motor->usrconfig,&motor->task);
    motor->foc.v_bus = read_vbus();

    motor->foc.i_a = motor->adc_sensor.read_iphase_c(&motor->adc_sensor);
    motor->foc.i_b = motor->adc_sensor.read_iphase_b(&motor->adc_sensor);
    motor->foc.i_c = -(motor->foc.i_a + motor->foc.i_b);
    
    switch(motor->task.fsm.state)
    {
        case BOOT_UP:
            break;
        case IDLE:
        {
            motor->foc.over_current_time = 0;
            break;
        }
        case CALIBRATION:
        {
            motor->calibration.calibration_loop(motor);
            break;
        }
        case ZERO_CAILBRATION:
        {
            motor->zero_cailbration.loop(motor);
            motor->foc.current(motor,0, motor->controller.i_q_set, motor->encoder.phase, motor->encoder.phase_vel);
            break;
        }
        case RUN:
        {
            motor->foc.current(motor,0, motor->controller.i_q_set, motor->encoder.phase, motor->encoder.phase_vel);
            break;
        }
        case OPEN_LOOP_RUN:
        {
            motor->open_loop.loop(motor);
            break;
        }
        case ERRORCLEAR:
        {
            motor->task.reset_error(&motor->task);
            if(motor->task.statusword_new.errors.errors_code == 0){
                motor->task.statecmd = IDLE;
                motor->task.set_state(&motor->task,&motor->enable,&motor->usrconfig);
            }
            break;
        }
        default:
            break;
    }
}






