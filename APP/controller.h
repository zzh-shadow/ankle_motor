#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_pubinclude.h"

#define CONTROLLER_DIFF     0.001f

typedef enum {
    CONTROL_MODE_TORQUE_RAMP        = 0,
    CONTROL_MODE_VELOCITY_RAMP      = 1,
    CONTROL_MODE_POSITION_FILTER    = 2,
    CONTROL_MODE_POSITION_PROFILE   = 3,
}CONTROL_MODE_e;

struct TASK_s;
struct MOTOR_s;
struct FOC_s;
struct USR_CONFIG_s;
struct CONTROLLER_s;
struct CONTROLLER_s;
struct ENCODER_s;
struct TRAJ_s;

int controller_set_home(struct CONTROLLER_s* controller,struct ENCODER_s* encoder,struct TASK_s* task,struct TRAJ_s* traj);
void controller_sync_callback(struct CONTROLLER_s* controller,struct USR_CONFIG_s* usr_config,struct TASK_s* task);
void controller_update_input_pos_filter_gain(struct CONTROLLER_s* controller,float bw);
void controller_init(struct CONTROLLER_s* controller,struct USR_CONFIG_s* usr_config);
void controller_reset(struct CONTROLLER_s* controller,struct ENCODER_s* encoder,struct TRAJ_s* traj,struct USR_CONFIG_s* usr_config);
void controller_loop(struct MOTOR_s* motor);






#ifdef __cplusplus
}
#endif


#endif
