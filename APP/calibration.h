#ifndef __CALIBRATION_H__
#define __CALIBRATION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_pubinclude.h"



struct MOTOR_s;
struct FOC_s;
struct PWM_GEN_s;
struct USR_CONFIG_s;
struct CALIBRATION_s;

void calibration_start(struct CALIBRATION_s *calibration ,struct USR_CONFIG_s *usr_config);
void calibration_end(struct CALIBRATION_s *calibration ,struct FOC_s *foc ,struct PWM_GEN_s *pwm_gen);
void calibration_loop(struct MOTOR_s *motor);




#ifdef __cplusplus
}
#endif


#endif
