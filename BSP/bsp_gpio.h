#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "app_pubinclude.h"

struct ENABLE_s;


void enable_motor_drive(struct ENABLE_s *enable ,bool is_enable);
void enable_boost(struct ENABLE_s *enable ,bool is_enable);
void enable_encoder(struct ENABLE_s *enable ,bool is_enable);
void test_gpio(struct ENABLE_s *enable ,bool is_enable);

#ifdef __cplusplus
}
#endif


#endif  //__BSP_GPIO_H__