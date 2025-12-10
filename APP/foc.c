#include "foc.h"


void FOC_update_current_ctrl_gain(struct FOC_s* foc, struct USR_CONFIG_s* usr_config)
{
     // float bandwidth = MIN(usr_config->current_ctrl_bw, 0.25f * TIM_PWM_HZ);
     foc->current_ctrl_p_gain =  0.25;
     foc->current_ctrl_i_gain =  4.3;
     // foc->current_ctrl_p_gain =  usr_config->motor_phase_inductance / CURRENT_MEASURE_PERIOD / 4;
     // foc->current_ctrl_i_gain =  usr_config->motor_phase_resistance / CURRENT_MEASURE_PERIOD / 4;
     
}
void FOC_init(struct FOC_s *foc,struct USR_CONFIG_s* usr_config)
{
     foc->v_bus = 0;
     foc->v_bus_filt = 0;
     foc->i_q = 0;
     foc->i_q_filt = 0;
     foc->i_bus = 0;
     foc->i_bus_filt = 0;
     foc->power_filt = 0;
     
     foc->is_armed = 0;
     
     FOC_update_current_ctrl_gain(foc, usr_config);
}

void FOC_arm(struct FOC_s* foc, struct ENABLE_s* enable ,struct PWM_GEN_s* pwm_gen)
{
     if(foc->is_armed){
          return;
     }
     __disable_irq();

     foc->i_q = 0;
     foc->i_q_filt = 0;
     foc->i_bus = 0;
     foc->i_bus_filt = 0;
     foc->power_filt = 0;
     foc->check_flg = true;
     foc->adc_offest_idx = 0;

     foc->current_ctrl_integral_d = 0;
     foc->current_ctrl_integral_q = 0;

     foc->time_count = 0;
     foc->torque_time_count = 0;

     enable->drive_enable(enable,true);
     pwm_gen->turn_on_low_sides(pwm_gen);
     foc->is_armed = true;


     __enable_irq();



}


void FOC_disarm(struct FOC_s* foc, struct ENABLE_s* enable ,struct PWM_GEN_s* pwm_gen)
{
     if(!foc->is_armed){
          return;
     }
     __disable_irq();

     foc->i_q = 0;
     foc->i_q_filt = 0;
     foc->i_bus = 0;
     foc->i_bus_filt = 0;
     foc->power_filt = 0;

     pwm_gen->switch_off_pwm(pwm_gen);
     foc->is_armed = false;
     enable->drive_enable(enable,false);

     __enable_irq();


}


void FOC_voltage(struct FOC_s* foc,float vd_set, float vq_set, float phase,struct PWM_GEN_s* pwm_gen)
{
     float i_alpha = 0, i_beta = 0;
     clarke_transform(foc->i_a,foc->i_b,foc->i_c,&i_alpha,&i_beta);

     float i_d, i_q;
     park_transform(i_alpha,i_beta,phase,&i_d,&i_q);

     foc->i_q = i_q;
     UTILS_LP_FAST(foc->i_q_filt, foc->i_q, 0.01f);
     foc->i_d = i_d;
     UTILS_LP_FAST(foc->i_d_filt, foc->i_d, 0.01f);

     float v_to_mod = 1.0f / (foc->v_bus * 2.0 / 3.0f);
     float mod_d = v_to_mod * vd_set;
     float mod_q = v_to_mod * vq_set;

     float mod_scalefactor = 0.95f * SQRT3_BY_2 / sqrtf(SQ(mod_d) + SQ(mod_q));

    if (mod_scalefactor < 1.0f) {
        mod_d *= mod_scalefactor;
        mod_q *= mod_scalefactor;
    }

    inverse_park(mod_d, mod_q, phase, &foc->mod_alpha, &foc->mod_beta);

    if(0 == svm(foc->mod_alpha, foc->mod_beta, &foc->dtc_a, &foc->dtc_b, &foc->dtc_c)){
        pwm_gen->set_a_duty(pwm_gen,(uint16_t)(foc->dtc_a * (float)HALF_PWM_PERIOD_CYCLES));
        pwm_gen->set_b_duty(pwm_gen,(uint16_t)(foc->dtc_b * (float)HALF_PWM_PERIOD_CYCLES));
        pwm_gen->set_c_duty(pwm_gen,(uint16_t)(foc->dtc_c * (float)HALF_PWM_PERIOD_CYCLES));
    }


}
void FOC_current(struct MOTOR_s* motor,float id_set, float iq_set, float phase, float phase_vel)
{
     float i_alpha, i_beta;
     float i_d, i_q;

     clarke_transform(motor->foc.i_a,motor->foc.i_b,motor->foc.i_c,&i_alpha,&i_beta);
     
     park_transform(i_alpha,i_beta,phase,&i_d,&i_q);

     float mod_to_v = motor->foc.v_bus * 0.66666f;
     float v_to_mod = 1.0f / mod_to_v;

     float i_err_d = id_set - i_d;
     float i_err_q = iq_set - i_q;
     
     float mod_d = v_to_mod * (motor->foc.current_ctrl_integral_d + i_err_d * motor->foc.current_ctrl_p_gain);
     float mod_q = v_to_mod * (motor->foc.current_ctrl_integral_q + i_err_q * motor->foc.current_ctrl_p_gain);

     float mod_scalefactor = 0.95f * SQRT3_BY_2 / sqrtf(SQ(mod_d) + SQ(mod_q));

     if(mod_scalefactor < 1.0f){
          mod_d *= mod_scalefactor;
          mod_q *= mod_scalefactor;
          motor->foc.current_ctrl_integral_d *= 0.99f;
          motor->foc.current_ctrl_integral_q *= 0.99f;
     }
     else{
          if(motor->usrconfig.control_mode > 0 && ABS(0 - motor->encoder.vel) < motor->usrconfig.target_velcity_window ) {
               if(motor->task.statusword_new.status.target_reached == 1) {
                    motor->foc.torque_time_count++;
                    if(motor->foc.torque_time_count >= 50) {
                         motor->foc.torque_time_count = 50;
                         motor->foc.current_ctrl_integral_d = 0;
                         motor->foc.current_ctrl_integral_q = 0;
                    }
               }
               else {
                    motor->foc.current_ctrl_integral_d += i_err_d * (motor->foc.current_ctrl_i_gain * CURRENT_MEASURE_PERIOD);
                    motor->foc.current_ctrl_integral_q += i_err_q * (motor->foc.current_ctrl_i_gain * CURRENT_MEASURE_PERIOD);
                    motor->foc.torque_time_count = 0;
               }
          }
          else {
               motor->foc.current_ctrl_integral_d += i_err_d * (motor->foc.current_ctrl_i_gain * CURRENT_MEASURE_PERIOD);
               motor->foc.current_ctrl_integral_q += i_err_q * (motor->foc.current_ctrl_i_gain * CURRENT_MEASURE_PERIOD);
          }
     }
          

     
     float pwm_phase = phase + phase_vel * CURRENT_MEASURE_PERIOD;
     float mod_alpha,mod_beta;
     inverse_park(mod_d, mod_q, pwm_phase, &mod_alpha, &mod_beta);

     motor->foc.i_q = i_q;

     motor->foc.i_d = i_d;

     if (0 == svm(mod_alpha, mod_beta, &motor->foc.dtc_a, &motor->foc.dtc_b, &motor->foc.dtc_c)) {
          motor->pwm_gen.set_a_duty(&motor->pwm_gen,(uint16_t)(motor->foc.dtc_a * (float)HALF_PWM_PERIOD_CYCLES));
          motor->pwm_gen.set_b_duty(&motor->pwm_gen,(uint16_t)(motor->foc.dtc_b * (float)HALF_PWM_PERIOD_CYCLES));
          motor->pwm_gen.set_c_duty(&motor->pwm_gen,(uint16_t)(motor->foc.dtc_c * (float)HALF_PWM_PERIOD_CYCLES));
     }
}
