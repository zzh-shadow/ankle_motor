#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "app_pubinclude.h"
#include "adc.h"

#define ADC_REF_V               (float)(3.3f)

#define SAMPLE_RES              (double)(0.02f)
#define AMP_GAIN                (double)(2.0f) 
#define ADC_MAX                 (double)(4095.0f)

#define SAMPLE_CURR_CON_FACTOR  (double)(ADC_REF_V/ADC_MAX/AMP_GAIN/SAMPLE_RES)
// #define SAMPLE_CURR_CON_FACTOR  (double)(ADC_REF_V/ADC_MAX)

struct PWM_GEN_s;
struct ADC_SENSOR_s;



void bsp_adc_init(void);
float bsp_read_iphase_a(struct ADC_SENSOR_s *adc_sensor);
float bsp_read_iphase_b(struct ADC_SENSOR_s *adc_sensor);
float bsp_read_iphase_c(struct ADC_SENSOR_s *adc_sensor);
int pwmc_current_reading_polarization(struct ADC_SENSOR_s *adc_sensor,struct PWM_GEN_s *pwm_gen);





#ifdef __cplusplus
}
#endif


#endif  //__bsp_adc_H__