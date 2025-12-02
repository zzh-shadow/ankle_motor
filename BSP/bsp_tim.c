#include "bsp_tim.h"


uint32_t duty_a ,duty_b ,duty_c;
void set_a_duty(struct PWM_GEN_s* pwm_gen,uint32_t duty)
{
    pwm_gen->motor_tim->Instance->CCR1 = duty;
    duty_a = duty;
    // HAL_TIM_OC_Start(pwm_gen->motor_tim,TIM_CHANNEL_1);
    // __HAL_TIM_SET_COMPARE(pwm_gen->motor_tim,TIM_CHANNEL_1,duty);
}

void set_b_duty(struct PWM_GEN_s* pwm_gen,uint32_t duty)
{
    pwm_gen->motor_tim->Instance->CCR2 = duty;
    duty_b = duty;
    // HAL_TIM_OC_Start(pwm_gen->motor_tim,TIM_CHANNEL_2);
}


void set_c_duty(struct PWM_GEN_s* pwm_gen,uint32_t duty)
{
    pwm_gen->motor_tim->Instance->CCR3 = duty;
    duty_c = duty;
    // HAL_TIM_OC_Start(pwm_gen->motor_tim,TIM_CHANNEL_3);
}

void switch_on_pwm(struct PWM_GEN_s* pwm_gen)
{
    set_a_duty(pwm_gen,TIM_PERIOD_CLOCK/2);
    set_b_duty(pwm_gen,TIM_PERIOD_CLOCK/2);
    set_c_duty(pwm_gen,TIM_PERIOD_CLOCK/2);

    __HAL_TIM_CLEAR_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE);
    while(RESET == __HAL_TIM_GET_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE)){};
	__HAL_TIM_CLEAR_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE);

    HAL_TIM_OC_Start(pwm_gen->motor_tim,TIM_CHANNEL_1);
    HAL_TIM_OC_Start(pwm_gen->motor_tim,TIM_CHANNEL_2);
    HAL_TIM_OC_Start(pwm_gen->motor_tim,TIM_CHANNEL_3);

}

void switch_off_pwm(struct PWM_GEN_s* pwm_gen)
{
	HAL_TIM_OC_Stop(pwm_gen->motor_tim, TIM_CHANNEL_1);
	HAL_TIM_OC_Stop(pwm_gen->motor_tim, TIM_CHANNEL_2);
	HAL_TIM_OC_Stop(pwm_gen->motor_tim, TIM_CHANNEL_3);

    __HAL_TIM_CLEAR_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE);
    while(RESET == __HAL_TIM_GET_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE)){};
	__HAL_TIM_CLEAR_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE);

}

void turn_on_low_sides_pwm(struct PWM_GEN_s* pwm_gen)
{
    set_a_duty(pwm_gen,0);
    set_b_duty(pwm_gen,0);
    set_c_duty(pwm_gen,0);

    __HAL_TIM_CLEAR_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE);
    while(RESET == __HAL_TIM_GET_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE)){};
	__HAL_TIM_CLEAR_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE);

    HAL_TIM_OC_Start(pwm_gen->motor_tim,TIM_CHANNEL_1);
    HAL_TIM_OC_Start(pwm_gen->motor_tim,TIM_CHANNEL_2);
    HAL_TIM_OC_Start(pwm_gen->motor_tim,TIM_CHANNEL_3);
}


void bsp_tim_init(void)
{
    HAL_TIM_Base_Start_IT(&htim1);

    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);

    HAL_TIM_OC_Start(&htim1,TIM_CHANNEL_4);

    __HAL_TIM_MOE_DISABLE_UNCONDITIONALLY(&htim1);
    __HAL_DBGMCU_FREEZE_TIM1();
	__HAL_TIM_MOE_ENABLE(&htim1);
    
	__HAL_TIM_ENABLE_IT(&htim1,TIM_IT_BREAK);
	__HAL_TIM_ENABLE_IT(&htim1,TIM_IT_UPDATE);	
    
}


