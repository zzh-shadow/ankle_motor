#ifndef __TRAP_TRAJ_H__
#define __TRAP_TRAJ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_pubinclude.h"

struct TRAJ_s;
struct ENCODER_s;
struct CONTROLLER_s;
struct USR_CONFIG_s;

int traj_plan(struct TRAJ_s* traj,float t_pos,float s_pos, float t_vel, float s_vel, float time,float max_accel,float max_decel,float max_vel);
void traj_eval(struct TRAJ_s* traj,struct ENCODER_s* encoder,struct CONTROLLER_s* controller,struct USR_CONFIG_s *usr_config);


#ifdef __cplusplus
}
#endif


#endif