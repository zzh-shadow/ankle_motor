#ifndef __ZERO_CALIBRATION_H__
#define __ZERO_CALIBRATION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_pubinclude.h"

struct MOTOR_s;
struct ZERO_CAILBRATION_s;
struct CONTROLLER_s;
struct USR_CONFIG_s;
struct ENCODER_s;

void zero_cailbration_start(struct ZERO_CAILBRATION_s *zero_cail,struct USR_CONFIG_s *usr_config);
void zero_cailbration_loop(struct MOTOR_s *motor);
void zero_cailbration_stop(struct ZERO_CAILBRATION_s *zero_cail,struct CONTROLLER_s *controller,struct USR_CONFIG_s *usr_config,struct ENCODER_s* encoder);


#ifdef __cplusplus
}
#endif


#endif