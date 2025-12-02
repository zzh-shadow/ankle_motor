#ifndef __BSP_TIM_H__
#define __BSP_TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tim.h"
#include "app_pubinclude.h"

#define PWM_FREQUENCY               TIM_PWM_HZ
#define CURRENT_MEASURE_HZ          PWM_FREQUENCY
#define CURRENT_MEASURE_PERIOD      (float)(1.0f/(float)CURRENT_MEASURE_HZ)

#define TIMER0_CLK_MHz               80
#define PWM_PERIOD_CYCLES            (uint16_t)((TIMER0_CLK_MHz*(uint32_t)1000000u/((uint32_t)(PWM_FREQUENCY)))&0xFFFE)
#define HALF_PWM_PERIOD_CYCLES       (uint16_t)(PWM_PERIOD_CYCLES/2U)

struct PWM_GEN_s;

void set_a_duty(struct PWM_GEN_s* pwm_gen,uint32_t duty);
void set_b_duty(struct PWM_GEN_s* pwm_gen,uint32_t duty);
void set_c_duty(struct PWM_GEN_s* pwm_gen,uint32_t duty);
void bsp_tim_init(void);
void switch_on_pwm(struct PWM_GEN_s* pwm_gen);
void switch_off_pwm(struct PWM_GEN_s* pwm_gen);
void turn_on_low_sides_pwm(struct PWM_GEN_s* pwm_gen);



#ifdef __cplusplus
}
#endif


#endif  //__bsp_tim_H__