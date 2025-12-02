################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Common/util.c 

OBJS += \
./Common/util.o 

C_DEPS += \
./Common/util.d 


# Each subdirectory must supply rules for building sources it contributes
Common/%.o Common/%.su Common/%.cyclo: ../Common/%.c Common/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32L496xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/daima/huaiguanjie/APP" -I"D:/daima/huaiguanjie/BSP" -I"D:/daima/huaiguanjie/Common" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Common

clean-Common:
	-$(RM) ./Common/util.cyclo ./Common/util.d ./Common/util.o ./Common/util.su

.PHONY: clean-Common

