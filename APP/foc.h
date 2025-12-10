#ifndef __FOC_H__
#define __FOC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_pubinclude.h"

struct FOC_s;
struct USR_CONFIG_s;
struct PWM_GEN_s;
struct ENABLE_s;
struct TASK_s;
struct ENCODER_s;

void FOC_init(struct FOC_s *foc,struct USR_CONFIG_s* usr_config);
void FOC_update_current_ctrl_gain(struct FOC_s* Foc, struct USR_CONFIG_s* UsrConfig);
void FOC_arm(struct FOC_s* Foc, struct ENABLE_s* enable, struct PWM_GEN_s* PWMGen);
void FOC_disarm(struct FOC_s* Foc, struct ENABLE_s* enable ,struct PWM_GEN_s* PWMGen);
void FOC_voltage(struct FOC_s* Foc,float Vd_set, float Vq_set, float phase,struct PWM_GEN_s* PWMGen);
void FOC_current(struct MOTOR_s* motor,float id_set, float iq_set, float phase, float phase_vel);


#ifdef __cplusplus
}
#endif


#endif
