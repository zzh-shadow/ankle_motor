#include "bsp_gpio.h"

void enable_motor_drive(struct ENABLE_s *enable ,bool is_enable)
{
    if(is_enable){
        HAL_GPIO_WritePin(enable->motor_enable_gpio, enable->motor_drive_pin, GPIO_PIN_SET);
    }
    else{
        HAL_GPIO_WritePin(enable->motor_enable_gpio, enable->motor_drive_pin, GPIO_PIN_RESET);
    }
}

void enable_boost(struct ENABLE_s *enable ,bool is_enable)
{
    if(is_enable){
        HAL_GPIO_WritePin(enable->motor_boost_gpio, enable->motor_boost_pin, GPIO_PIN_SET);   //test
    }
    else{
        HAL_GPIO_WritePin(enable->motor_boost_gpio, enable->motor_boost_pin, GPIO_PIN_RESET);
    }
}

void enable_encoder(struct ENABLE_s *enable ,bool is_enable)
{
    if(is_enable){
        HAL_GPIO_WritePin(enable->encoder_enable_gpio, enable->encoder_enable_pin, GPIO_PIN_SET);
    }
    else{
        HAL_GPIO_WritePin(enable->encoder_enable_gpio, enable->encoder_enable_pin, GPIO_PIN_RESET);
    }
}

void test_gpio(struct ENABLE_s *enable ,bool is_enable)
{
    if(is_enable){
        HAL_GPIO_WritePin(enable->test_gpio, enable->test_pin, GPIO_PIN_SET);
    }
    else{
        HAL_GPIO_WritePin(enable->test_gpio, enable->test_pin, GPIO_PIN_RESET);
    }
}


