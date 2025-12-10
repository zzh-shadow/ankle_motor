################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/battery.c \
../APP/calibration.c \
../APP/controller.c \
../APP/encoder.c \
../APP/foc.c \
../APP/main_communication.c \
../APP/open_loop.c \
../APP/task.c \
../APP/trap_traj.c \
../APP/usr_config.c \
../APP/zero_cailbration.c 

OBJS += \
./APP/battery.o \
./APP/calibration.o \
./APP/controller.o \
./APP/encoder.o \
./APP/foc.o \
./APP/main_communication.o \
./APP/open_loop.o \
./APP/task.o \
./APP/trap_traj.o \
./APP/usr_config.o \
./APP/zero_cailbration.o 

C_DEPS += \
./APP/battery.d \
./APP/calibration.d \
./APP/controller.d \
./APP/encoder.d \
./APP/foc.d \
./APP/main_communication.d \
./APP/open_loop.d \
./APP/task.d \
./APP/trap_traj.d \
./APP/usr_config.d \
./APP/zero_cailbration.d 


# Each subdirectory must supply rules for building sources it contributes
APP/%.o APP/%.su APP/%.cyclo: ../APP/%.c APP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L496xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/daima/huaiguanjie_bat/ankle_motor/APP" -I"D:/daima/huaiguanjie_bat/ankle_motor/Common" -I"D:/daima/huaiguanjie_bat/ankle_motor/BSP" -I../Middlewares/ST/ARM/DSP/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-APP

clean-APP:
	-$(RM) ./APP/battery.cyclo ./APP/battery.d ./APP/battery.o ./APP/battery.su ./APP/calibration.cyclo ./APP/calibration.d ./APP/calibration.o ./APP/calibration.su ./APP/controller.cyclo ./APP/controller.d ./APP/controller.o ./APP/controller.su ./APP/encoder.cyclo ./APP/encoder.d ./APP/encoder.o ./APP/encoder.su ./APP/foc.cyclo ./APP/foc.d ./APP/foc.o ./APP/foc.su ./APP/main_communication.cyclo ./APP/main_communication.d ./APP/main_communication.o ./APP/main_communication.su ./APP/open_loop.cyclo ./APP/open_loop.d ./APP/open_loop.o ./APP/open_loop.su ./APP/task.cyclo ./APP/task.d ./APP/task.o ./APP/task.su ./APP/trap_traj.cyclo ./APP/trap_traj.d ./APP/trap_traj.o ./APP/trap_traj.su ./APP/usr_config.cyclo ./APP/usr_config.d ./APP/usr_config.o ./APP/usr_config.su ./APP/zero_cailbration.cyclo ./APP/zero_cailbration.d ./APP/zero_cailbration.o ./APP/zero_cailbration.su

.PHONY: clean-APP

