################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/kernel_src/sample_code/event_sample.c \
../Core/Src/kernel_src/sample_code/memory_sample.c \
../Core/Src/kernel_src/sample_code/msg_queue_sample.c \
../Core/Src/kernel_src/sample_code/mutex_sample.c \
../Core/Src/kernel_src/sample_code/semaphore_sample.c \
../Core/Src/kernel_src/sample_code/soft_timer_sample.c \
../Core/Src/kernel_src/sample_code/task_sample.c 

OBJS += \
./Core/Src/kernel_src/sample_code/event_sample.o \
./Core/Src/kernel_src/sample_code/memory_sample.o \
./Core/Src/kernel_src/sample_code/msg_queue_sample.o \
./Core/Src/kernel_src/sample_code/mutex_sample.o \
./Core/Src/kernel_src/sample_code/semaphore_sample.o \
./Core/Src/kernel_src/sample_code/soft_timer_sample.o \
./Core/Src/kernel_src/sample_code/task_sample.o 

C_DEPS += \
./Core/Src/kernel_src/sample_code/event_sample.d \
./Core/Src/kernel_src/sample_code/memory_sample.d \
./Core/Src/kernel_src/sample_code/msg_queue_sample.d \
./Core/Src/kernel_src/sample_code/mutex_sample.d \
./Core/Src/kernel_src/sample_code/semaphore_sample.d \
./Core/Src/kernel_src/sample_code/soft_timer_sample.d \
./Core/Src/kernel_src/sample_code/task_sample.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/kernel_src/sample_code/event_sample.o: ../Core/Src/kernel_src/sample_code/event_sample.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F429xx -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/kernel_src/sample_code/event_sample.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/kernel_src/sample_code/memory_sample.o: ../Core/Src/kernel_src/sample_code/memory_sample.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F429xx -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/kernel_src/sample_code/memory_sample.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/kernel_src/sample_code/msg_queue_sample.o: ../Core/Src/kernel_src/sample_code/msg_queue_sample.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F429xx -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/kernel_src/sample_code/msg_queue_sample.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/kernel_src/sample_code/mutex_sample.o: ../Core/Src/kernel_src/sample_code/mutex_sample.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F429xx -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/kernel_src/sample_code/mutex_sample.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/kernel_src/sample_code/semaphore_sample.o: ../Core/Src/kernel_src/sample_code/semaphore_sample.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F429xx -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/kernel_src/sample_code/semaphore_sample.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/kernel_src/sample_code/soft_timer_sample.o: ../Core/Src/kernel_src/sample_code/soft_timer_sample.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F429xx -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/kernel_src/sample_code/soft_timer_sample.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/Src/kernel_src/sample_code/task_sample.o: ../Core/Src/kernel_src/sample_code/task_sample.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F429xx -c -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/Src/kernel_src/sample_code/task_sample.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

