#include "bsp_adc.h"


float bsp_read_iphase_a(struct ADC_SENSOR_s *adc_sensor)
{
    int adc_value = adc_sensor->ADC_a->Instance->JDR1 - adc_sensor->phase_a_adc_offest;
    // if(ABS(adc_value) <= 5) {
    //     adc_value = 0;
    // }
    return (float)adc_value * SAMPLE_CURR_CON_FACTOR;
}

float bsp_read_iphase_b(struct ADC_SENSOR_s *adc_sensor)
{
    int adc_value = adc_sensor->ADC_b->Instance->JDR2 - adc_sensor->phase_b_adc_offset;
    // if(ABS(adc_value) <= 5) {
    //     adc_value = 0;
    // }
    return (float)adc_value * SAMPLE_CURR_CON_FACTOR;
}

float bsp_read_iphase_c(struct ADC_SENSOR_s *adc_sensor)
{
    int adc_value = adc_sensor->ADC_c->Instance->JDR3 - adc_sensor->phase_c_adc_offset;
    // if(ABS(adc_value) <= 5) {
    //     adc_value = 0;
    // }
    return (float)adc_value * SAMPLE_CURR_CON_FACTOR;
}


int pwmc_current_reading_polarization(struct ADC_SENSOR_s *adc_sensor,struct PWM_GEN_s *pwm_gen)
{
    int i = 0;
    int adc_sum_a = 0;
    int adc_sum_b = 0;
    int adc_sum_c = 0;
    
    __HAL_TIM_CLEAR_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE);
    while(RESET == __HAL_TIM_GET_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE)){}
	__HAL_TIM_CLEAR_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE);

    while(i < 1024){
        while(SET == __HAL_TIM_GET_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE)){
            __HAL_TIM_CLEAR_FLAG(pwm_gen->motor_tim,TIM_FLAG_UPDATE);
            
            i++;

            adc_sum_a += (uint16_t)adc_sensor->ADC_a->Instance->JDR1;
            adc_sum_b += (uint16_t)adc_sensor->ADC_b->Instance->JDR2;
            adc_sum_c += (uint16_t)adc_sensor->ADC_c->Instance->JDR3;
        }
    }
    adc_sensor->phase_a_adc_offest = adc_sum_a / i;
    adc_sensor->phase_b_adc_offset = adc_sum_b / i;
    adc_sensor->phase_c_adc_offset = adc_sum_c / i;

    const int v_out = 2048;
    const int check_threshold = 75;

    if(adc_sensor->phase_a_adc_offest >(v_out + check_threshold) || adc_sensor->phase_a_adc_offest < (v_out - check_threshold)){
        return -1;
    }
    if(adc_sensor->phase_b_adc_offset >(v_out + check_threshold) || adc_sensor->phase_b_adc_offset < (v_out - check_threshold)){
        return -1;
    }
    if(adc_sensor->phase_c_adc_offset >(v_out + check_threshold) || adc_sensor->phase_c_adc_offset < (v_out - check_threshold)){
        return -1;
    }

    return 0;
}

extern MOTOR_t motor[NUM_MOTORS];

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
        if(__HAL_ADC_GET_FLAG(hadc,ADC_FLAG_JEOC))
        {
            for (int i = 0; i < NUM_MOTORS; i++) {
                motor[i].task.high_frequency_task(&motor[i]);
                motor[i].task.safety_task(&motor[i].task,&motor[i].usrconfig,&motor[i].foc);
            }
            __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_JEOC);
        }
    }
}


void bsp_adc_init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1,ADC_SINGLE_ENDED);

    HAL_ADCEx_InjectedStart_IT(&hadc1);
}


