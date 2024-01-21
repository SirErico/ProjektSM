################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Components/Inc/btn.c \
../Components/Inc/btn_config.c \
../Components/Inc/dio.c 

OBJS += \
./Components/Inc/btn.o \
./Components/Inc/btn_config.o \
./Components/Inc/dio.o 

C_DEPS += \
./Components/Inc/btn.d \
./Components/Inc/btn_config.d \
./Components/Inc/dio.d 


# Each subdirectory must supply rules for building sources it contributes
Components/Inc/%.o Components/Inc/%.su Components/Inc/%.cyclo: ../Components/Inc/%.c Components/Inc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Components-2f-Inc

clean-Components-2f-Inc:
	-$(RM) ./Components/Inc/btn.cyclo ./Components/Inc/btn.d ./Components/Inc/btn.o ./Components/Inc/btn.su ./Components/Inc/btn_config.cyclo ./Components/Inc/btn_config.d ./Components/Inc/btn_config.o ./Components/Inc/btn_config.su ./Components/Inc/dio.cyclo ./Components/Inc/dio.d ./Components/Inc/dio.o ./Components/Inc/dio.su

.PHONY: clean-Components-2f-Inc

