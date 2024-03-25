################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/ample/h750-rt-thread/h750-rt-thread-main/Core/Src/gpio.c \
/Users/ample/h750-rt-thread/h750-rt-thread-main/Core/Src/main.c \
/Users/ample/h750-rt-thread/h750-rt-thread-main/Core/Src/stm32h7xx_it.c \
../Application/User/Core/syscalls.c \
../Application/User/Core/sysmem.c \
/Users/ample/h750-rt-thread/h750-rt-thread-main/Core/Src/usart.c 

OBJS += \
./Application/User/Core/gpio.o \
./Application/User/Core/main.o \
./Application/User/Core/stm32h7xx_it.o \
./Application/User/Core/syscalls.o \
./Application/User/Core/sysmem.o \
./Application/User/Core/usart.o 

C_DEPS += \
./Application/User/Core/gpio.d \
./Application/User/Core/main.d \
./Application/User/Core/stm32h7xx_it.d \
./Application/User/Core/syscalls.d \
./Application/User/Core/sysmem.d \
./Application/User/Core/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/Core/gpio.o: /Users/ample/h750-rt-thread/h750-rt-thread-main/Core/Src/gpio.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=64000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DSTM32H750xx -c -I../../Core/Inc -I../../RT-Thread -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/RealThread_RTOS/include/ -I../../Middlewares/Third_Party/RealThread_RTOS/bsp/_template/cubemx_config -I../../Middlewares/Third_Party/RealThread_RTOS/finsh/ -I../../Middlewares/Third_Party/RealThread_RTOS/components/finsh -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/Core/main.o: /Users/ample/h750-rt-thread/h750-rt-thread-main/Core/Src/main.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=64000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DSTM32H750xx -c -I../../Core/Inc -I../../RT-Thread -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/RealThread_RTOS/include/ -I../../Middlewares/Third_Party/RealThread_RTOS/bsp/_template/cubemx_config -I../../Middlewares/Third_Party/RealThread_RTOS/finsh/ -I../../Middlewares/Third_Party/RealThread_RTOS/components/finsh -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/Core/stm32h7xx_it.o: /Users/ample/h750-rt-thread/h750-rt-thread-main/Core/Src/stm32h7xx_it.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=64000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DSTM32H750xx -c -I../../Core/Inc -I../../RT-Thread -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/RealThread_RTOS/include/ -I../../Middlewares/Third_Party/RealThread_RTOS/bsp/_template/cubemx_config -I../../Middlewares/Third_Party/RealThread_RTOS/finsh/ -I../../Middlewares/Third_Party/RealThread_RTOS/components/finsh -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/Core/%.o Application/User/Core/%.su Application/User/Core/%.cyclo: ../Application/User/Core/%.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=64000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DSTM32H750xx -c -I../../Core/Inc -I../../RT-Thread -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/RealThread_RTOS/include/ -I../../Middlewares/Third_Party/RealThread_RTOS/bsp/_template/cubemx_config -I../../Middlewares/Third_Party/RealThread_RTOS/finsh/ -I../../Middlewares/Third_Party/RealThread_RTOS/components/finsh -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/Core/usart.o: /Users/ample/h750-rt-thread/h750-rt-thread-main/Core/Src/usart.c Application/User/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DUSE_FULL_LL_DRIVER -DHSE_VALUE=25000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DEXTERNAL_CLOCK_VALUE=12288000 -DHSI_VALUE=64000000 -DLSI_VALUE=32000 -DVDD_VALUE=3300 -DSTM32H750xx -c -I../../Core/Inc -I../../RT-Thread -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/RealThread_RTOS/include/ -I../../Middlewares/Third_Party/RealThread_RTOS/bsp/_template/cubemx_config -I../../Middlewares/Third_Party/RealThread_RTOS/finsh/ -I../../Middlewares/Third_Party/RealThread_RTOS/components/finsh -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-Core

clean-Application-2f-User-2f-Core:
	-$(RM) ./Application/User/Core/gpio.cyclo ./Application/User/Core/gpio.d ./Application/User/Core/gpio.o ./Application/User/Core/gpio.su ./Application/User/Core/main.cyclo ./Application/User/Core/main.d ./Application/User/Core/main.o ./Application/User/Core/main.su ./Application/User/Core/stm32h7xx_it.cyclo ./Application/User/Core/stm32h7xx_it.d ./Application/User/Core/stm32h7xx_it.o ./Application/User/Core/stm32h7xx_it.su ./Application/User/Core/syscalls.cyclo ./Application/User/Core/syscalls.d ./Application/User/Core/syscalls.o ./Application/User/Core/syscalls.su ./Application/User/Core/sysmem.cyclo ./Application/User/Core/sysmem.d ./Application/User/Core/sysmem.o ./Application/User/Core/sysmem.su ./Application/User/Core/usart.cyclo ./Application/User/Core/usart.d ./Application/User/Core/usart.o ./Application/User/Core/usart.su

.PHONY: clean-Application-2f-User-2f-Core

