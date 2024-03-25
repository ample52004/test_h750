################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/ample/h750-rt-thread/h750-rt-thread-main/Middlewares/Third_Party/RealThread_RTOS/components/device/device.c 

OBJS += \
./Middlewares/RT-Thread/RTOS/device/device.o 

C_DEPS += \
./Middlewares/RT-Thread/RTOS/device/device.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/RT-Thread/RTOS/device/device.o: /Users/ample/h750-rt-thread/h750-rt-thread-main/Middlewares/Third_Party/RealThread_RTOS/components/device/device.c Middlewares/RT-Thread/RTOS/device/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=64000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DSTM32H750xx -c -I../../Core/Inc -I../../RT-Thread -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/RealThread_RTOS/include/ -I../../Middlewares/Third_Party/RealThread_RTOS/bsp/_template/cubemx_config -I../../Middlewares/Third_Party/RealThread_RTOS/finsh/ -I../../Middlewares/Third_Party/RealThread_RTOS/components/finsh -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middlewares-2f-RT-2d-Thread-2f-RTOS-2f-device

clean-Middlewares-2f-RT-2d-Thread-2f-RTOS-2f-device:
	-$(RM) ./Middlewares/RT-Thread/RTOS/device/device.cyclo ./Middlewares/RT-Thread/RTOS/device/device.d ./Middlewares/RT-Thread/RTOS/device/device.o ./Middlewares/RT-Thread/RTOS/device/device.su

.PHONY: clean-Middlewares-2f-RT-2d-Thread-2f-RTOS-2f-device

