################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Menuitems/menu_l0_intro.c 

OBJS += \
./Menuitems/menu_l0_intro.o 

C_DEPS += \
./Menuitems/menu_l0_intro.d 


# Each subdirectory must supply rules for building sources it contributes
Menuitems/%.o: ../Menuitems/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft '-D__weak=__attribute__((weak))' '-D__packed=__attribute__((__packed__))' -DUSE_HAL_DRIVER -DSTM32F042x6 -I"C:/Users/andylithia/Documents/STMicro/Project-Flora/ETest_SW4STM32/ETest_SW4STM32/Inc" -I"C:/Users/andylithia/Documents/STMicro/Project-Flora/ETest_SW4STM32/ETest_SW4STM32/Drivers/STM32F0xx_HAL_Driver/Inc" -I"C:/Users/andylithia/Documents/STMicro/Project-Flora/ETest_SW4STM32/ETest_SW4STM32/Drivers/STM32F0xx_HAL_Driver/Inc/Legacy" -I"C:/Users/andylithia/Documents/STMicro/Project-Flora/ETest_SW4STM32/ETest_SW4STM32/Drivers/CMSIS/Device/ST/STM32F0xx/Include" -I"C:/Users/andylithia/Documents/STMicro/Project-Flora/ETest_SW4STM32/ETest_SW4STM32/Drivers/CMSIS/Include" -I"C:/Users/andylithia/Documents/STMicro/Project-Flora/ETest_SW4STM32/ETest_SW4STM32/Inc"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


