#include "calibration.h"
#include "encoder.h"

void calibration_start(struct CALIBRATION_s *calibration ,struct USR_CONFIG_s *usr_config)
{
    usr_config->encoder_dir = +1;
    usr_config->calib_valid = false;
    calibration->calib_step = CS_DIR_PP_START;
    calibration->phase_set = 0;
    calibration->error_lut_idx = 0;
    usr_config->encoder_ele_offest = 0;
    calibration->error_lut_mov = 0;
    calibration->loop_count = 0;
    calibration->get_out_count = 0;
}


void calibration_end(struct CALIBRATION_s *calibration ,struct FOC_s *foc ,struct PWM_GEN_s *pwm_gen)
{
    calibration->calib_step = CS_NULL;
}


void calibration_loop(struct MOTOR_s *motor)
{
    float calib_phase_vel =  motor->usrconfig.motor_pole_pairs * M_2PI;
    float time = (float)motor->calibration.loop_count * CURRENT_MEASURE_PERIOD;

    const float voltage = motor->usrconfig.calib_voltage;

    int32_t diff = 0;

    switch(motor->calibration.calib_step)
    {
        case CS_DIR_PP_START:
        {
            motor->foc.voltage(&motor->foc,voltage,0,motor->calibration.phase_set,&motor->pwm_gen);
            if(time >= 1.0f) {
                motor->calibration.start_count = (float)motor->encoder.shadow_count;
                motor->calibration.calib_step = CS_DIR_PP_LOOP;
            }
            else if(time >= 0.5f) {
                motor->calibration.check_count = motor->encoder.one_pos_count;
                motor->usrconfig.encoder_offset = motor->calibration.check_count;
                motor->calibration.last_error_lut = motor->encoder.ele_phase;
            }
            break;
        }
        case CS_DIR_PP_LOOP:
        {
            float ele_phase = motor->encoder.phase;
            while(ele_phase > M_2PI) ele_phase -= M_2PI;

            motor->calibration.phase_set += calib_phase_vel * CURRENT_MEASURE_PERIOD;
            motor->foc.voltage(&motor->foc,voltage, 0, motor->calibration.phase_set,&motor->pwm_gen);
            if(motor->calibration.phase_set >= M_2PI){
				
                motor->calibration.calib_step = CS_DIR_PP_CW_LOOP;
                diff = motor->encoder.shadow_count - motor->calibration.start_count;
                if(diff > 0){
                    motor->usrconfig.encoder_dir = +1;
                }
                else{
                    motor->usrconfig.encoder_dir = -1;
                }
                if(ABS(motor->encoder.phase - M_2PI) > 0.5 ) {
                    motor->calibration.loop_count = 0;
                    if(motor->calibration.get_out_count < 2) {
                        motor->calibration.calib_step = CS_REPORT_OFFSET_LUT;
                    }
                    else {
                        motor->task.statusword_new.errors.cailbration = true;
                    }
                }
            }
            if((ABS(motor->usrconfig.calib_voltage - motor->foc.i_a ) < 0.01f) && ele_phase > M_PI) {
                motor->calibration.error_lut_mov += ele_phase;
                motor->calibration.error_lut_idx ++;
            }
            break;
        }
        case CS_DIR_PP_CW_LOOP:
        {
            float ele_phase = motor->encoder.phase;
            while(ele_phase > M_2PI) ele_phase -= M_2PI;

            motor->calibration.phase_set -= calib_phase_vel * CURRENT_MEASURE_PERIOD;
            motor->foc.voltage(&motor->foc,voltage, 0, motor->calibration.phase_set,&motor->pwm_gen);
            if(motor->calibration.phase_set <= 0) { 
				
                motor->calibration.calib_step = CS_DIR_PP_END;
            }
            if(ABS(motor->usrconfig.calib_voltage - motor->foc.i_a ) < 0.01f && ele_phase > M_PI) {
                motor->calibration.error_lut_mov += ele_phase;
                motor->calibration.error_lut_idx ++;
            }
            break;
        }   
        case CS_DIR_PP_END:
        {
            motor->usrconfig.encoder_ele_offest = M_2PI - (motor->calibration.error_lut_mov / motor->calibration.error_lut_idx);
            motor->usrconfig.encoder_offset += motor->usrconfig.encoder_ele_offest * ENCODER_CPR_F / (M_2PI * motor->usrconfig.motor_pole_pairs);
            // motor->usrconfig.encoder_offset = motor->calibration.check_count;
            motor->usrconfig.calib_valid = true;
            motor->task.statecmd = IDLE;
            motor->calibration.loop_count = 0;
            motor->task.set_state(&motor->task,&motor->enable,&motor->usrconfig);
            motor->calibration.calib_step = CS_DIR_PP_START;
            // motor->calibration.loop_count = 0;
            break;
        }

        case CS_ENCODER_START:
        {
            motor->calibration.phase_set = 0;
            motor->calibration.loop_count = 0;
            motor->calibration.sample_count = 0;
            motor->calibration.next_sample_time = 0;
            motor->calibration.calib_step = CS_ENCODER_CW_LOOP;
            break;
        }

        case CS_ENCODER_CW_LOOP:
        {
            if(motor->calibration.sample_count < (motor->usrconfig.motor_pole_pairs * SAMPLES_PER_PPAIR)){
                if(time > motor->calibration.next_sample_time){
                    motor->calibration.next_sample_time += M_2PI / ((float)SAMPLES_PER_PPAIR * calib_phase_vel);
                    
                    int count_ref = (motor->calibration.phase_set * ENCODER_CPR_F) / (M_2PI * (float)motor->usrconfig.motor_pole_pairs);
                    int error = motor->encoder.one_pos_count - count_ref;
                    error += ENCODER_CPR * (error<0);
                    motor->calibration.error_arr[motor->calibration.sample_count] = error;
                    
                    motor->calibration.sample_count ++;
                }
                
                motor->calibration.phase_set += calib_phase_vel * CURRENT_MEASURE_PERIOD;
            }
            else{
                motor->calibration.phase_set -= calib_phase_vel * CURRENT_MEASURE_PERIOD;
                motor->calibration.loop_count = 0;
                motor->calibration.sample_count --;
                motor->calibration.next_sample_time = 0;
                motor->calibration.calib_step = CS_ENCODER_CCW_LOOP;
                break;
            }
            motor->foc.voltage(&motor->foc,voltage, 0, motor->calibration.phase_set,&motor->pwm_gen);
            break;
        }

        case CS_ENCODER_CCW_LOOP:
        {
            if(motor->calibration.sample_count >= 0){
                if(time > motor->calibration.next_sample_time){
                    motor->calibration.next_sample_time += M_2PI / ((float)SAMPLES_PER_PPAIR * calib_phase_vel);
                    
                    int count_ref = (motor->calibration.phase_set * ENCODER_CPR_F) / (M_2PI * (float)motor->usrconfig.motor_pole_pairs);
                    int error = motor->encoder.one_pos_count - count_ref;
                    error += ENCODER_CPR * (error<0);
                    motor->calibration.error_arr[motor->calibration.sample_count] = (motor->calibration.error_arr[motor->calibration.sample_count] + error) / 2;
                    
                    motor->calibration.sample_count --;
                }
                
                motor->calibration.phase_set -= calib_phase_vel * CURRENT_MEASURE_PERIOD;
            }
            else{
                motor->pwm_gen.turn_on_low_sides(&motor->pwm_gen);
                motor->calibration.loop_count = 0;
                motor->calibration.calib_step = CS_ENCODER_END;
                break;
            }
            motor->foc.voltage(&motor->foc,voltage, 0, motor->calibration.phase_set,&motor->pwm_gen);
            break;
        }
        
        case CS_ENCODER_CHECK:
        {
            motor->foc.voltage(&motor->foc,0,0,motor->calibration.phase_set,&motor->pwm_gen);
            if(time >= 2.0f) {
                motor->calibration.calib_step = CS_ENCODER_END;
            }
            break;
        }

        case CS_ENCODER_END:
        {
            int64_t moving_avg = 0;
            for(int i = 0; i<(motor->usrconfig.motor_pole_pairs * SAMPLES_PER_PPAIR); i++){
                moving_avg += motor->calibration.error_arr[i];
            }
            motor->usrconfig.encoder_offset = moving_avg/(motor->usrconfig.motor_pole_pairs * SAMPLES_PER_PPAIR);
            
            int window = SAMPLES_PER_PPAIR;
            int lut_offset = motor->calibration.error_arr[0] * OFFSET_LUT_NUM / ENCODER_CPR;
            for(int i = 0; i<OFFSET_LUT_NUM; i++)
            {
                moving_avg = 0;
                for(int j = (-window)/2; j<(window)/2; j++){
                    int index = i*motor->usrconfig.motor_pole_pairs*SAMPLES_PER_PPAIR/OFFSET_LUT_NUM + j;
                    if(index<0){
                        index += (SAMPLES_PER_PPAIR*motor->usrconfig.motor_pole_pairs);
                    }else if(index>(SAMPLES_PER_PPAIR*motor->usrconfig.motor_pole_pairs-1)){
                        index -= (SAMPLES_PER_PPAIR*motor->usrconfig.motor_pole_pairs);
                    }
                    moving_avg += motor->calibration.error_arr[index];
                }
                moving_avg = moving_avg/window;
                int lut_index = lut_offset + i;
                if(lut_index > (OFFSET_LUT_NUM-1)){
                    lut_index -= OFFSET_LUT_NUM;
                }
                motor->usrconfig.offset_lut[lut_index] = moving_avg - motor->usrconfig.encoder_offset;
            }
            motor->calibration.loop_count = 0;
            motor->calibration.sample_count = 0;
            motor->calibration.next_sample_time = 0;
            motor->calibration.calib_step = CS_NULL;
            motor->usrconfig.calib_valid = true;
            motor->task.statecmd=IDLE;
            motor->task.set_state(&motor->task,&motor->enable,&motor->usrconfig);
            break;
        }

        case CS_REPORT_OFFSET_LUT:
        {
            motor->calibration.phase_set -= calib_phase_vel * CURRENT_MEASURE_PERIOD;
            motor->foc.voltage(&motor->foc,voltage, 0, motor->calibration.phase_set,&motor->pwm_gen);
            if(time >= 2.0f) {
                motor->calibration.loop_count = 0;
                motor->calibration.get_out_count++;
                motor->calibration.phase_set = 0;
                motor->calibration.calib_step = CS_DIR_PP_START;
            }
        }
        default:
            break;
    }
    motor->calibration.loop_count++;
}

