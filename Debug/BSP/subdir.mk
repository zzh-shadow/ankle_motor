################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/bsp_adc.c \
../BSP/bsp_gpio.c \
../BSP/bsp_tim.c \
../BSP/bsp_usart.c \
../BSP/dwt.c 

OBJS += \
./BSP/bsp_adc.o \
./BSP/bsp_gpio.o \
./BSP/bsp_tim.o \
./BSP/bsp_usart.o \
./BSP/dwt.o 

C_DEPS += \
./BSP/bsp_adc.d \
./BSP/bsp_gpio.d \
./BSP/bsp_tim.d \
./BSP/bsp_usart.d \
./BSP/dwt.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/%.o BSP/%.su BSP/%.cyclo: ../BSP/%.c BSP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L496xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/daima/huaiguanjie_bat/ankle_motor/APP" -I"D:/daima/huaiguanjie_bat/ankle_motor/Common" -I"D:/daima/huaiguanjie_bat/ankle_motor/BSP" -I../Middlewares/ST/ARM/DSP/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP

clean-BSP:
	-$(RM) ./BSP/bsp_adc.cyclo ./BSP/bsp_adc.d ./BSP/bsp_adc.o ./BSP/bsp_adc.su ./BSP/bsp_gpio.cyclo ./BSP/bsp_gpio.d ./BSP/bsp_gpio.o ./BSP/bsp_gpio.su ./BSP/bsp_tim.cyclo ./BSP/bsp_tim.d ./BSP/bsp_tim.o ./BSP/bsp_tim.su ./BSP/bsp_usart.cyclo ./BSP/bsp_usart.d ./BSP/bsp_usart.o ./BSP/bsp_usart.su ./BSP/dwt.cyclo ./BSP/dwt.d ./BSP/dwt.o ./BSP/dwt.su

.PHONY: clean-BSP

