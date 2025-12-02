#ifndef __OPEN_LOOP_H__
#define __OPEN_LOOP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_pubinclude.h"

struct FOC_s;
struct USR_CONFIG_s;
struct OPEN_LOOP_s;
struct MOTOR_s;


void open_loop_start(struct OPEN_LOOP_s *open_loop ,struct FOC_s *foc, struct PWM_GEN_s *pwm_gen ,struct USR_CONFIG_s *usr_config);
void open_loop_end(struct OPEN_LOOP_s *open_loop , struct FOC_s *foc , struct PWM_GEN_s *pwm_gen);
void open_loop_loop(struct MOTOR_s *motor);


#ifdef __cplusplus
}
#endif


#endif