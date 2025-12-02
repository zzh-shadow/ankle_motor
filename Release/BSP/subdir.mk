################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/bsp_adc.c \
../BSP/bsp_tim.c 

OBJS += \
./BSP/bsp_adc.o \
./BSP/bsp_tim.o 

C_DEPS += \
./BSP/bsp_adc.d \
./BSP/bsp_tim.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/%.o BSP/%.su BSP/%.cyclo: ../BSP/%.c BSP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L496xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/daima/huaiguanjie/APP" -I"D:/daima/huaiguanjie/BSP" -I"D:/daima/huaiguanjie/Common" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP

clean-BSP:
	-$(RM) ./BSP/bsp_adc.cyclo ./BSP/bsp_adc.d ./BSP/bsp_adc.o ./BSP/bsp_adc.su ./BSP/bsp_tim.cyclo ./BSP/bsp_tim.d ./BSP/bsp_tim.o ./BSP/bsp_tim.su

.PHONY: clean-BSP

