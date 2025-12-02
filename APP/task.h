#ifndef __TASK_H__
#define __TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_pubinclude.h"

#define CHARGE_BOOT_CAP_MS        10
#define CHARGE_BOOT_CAP_TICKS    (uint16_t)((PWM_FREQUENCY * CHARGE_BOOT_CAP_MS)/ 1000)

#define CHARGE_MOTOR_ENABLE_CAP_MS 100
#define CHARGE_MOTOR_ENABLE_CAP_TICKS    (uint16_t)((PWM_FREQUENCY * CHARGE_MOTOR_ENABLE_CAP_MS)/ 1000)


struct TASK_s;
struct MOTOR_s;
struct FOC_s;
struct USR_CONFIG_s;
struct CALIBRATION_s;

void task_init(struct TASK_s *task);
void task_reset_error(struct TASK_s *task);
int task_set_state(struct TASK_s *task,struct ENABLE_s *enable,struct USR_CONFIG_s *usrconfig);
void task_enter_state(struct MOTOR_s *motor);
void task_exit_state(struct MOTOR_s *motor);
void task_safety_task(struct TASK_s *task , struct USR_CONFIG_s *usr_config,struct FOC_s *foc);
void task_low_priority_task(struct MOTOR_s *motor);
void task_high_frequency_task(struct MOTOR_s *motor);





#ifdef __cplusplus
}
#endif


#endif