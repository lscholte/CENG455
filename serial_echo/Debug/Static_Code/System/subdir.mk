################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Static_Code/System/PE_low_level_init.c 

OBJS += \
./Static_Code/System/PE_low_level_init.o 

C_DEPS += \
./Static_Code/System/PE_low_level_init.d 


# Each subdirectory must supply rules for building sources it contributes
Static_Code/System/%.o: ../Static_Code/System/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK64FN1M0VLL12" -D"FSL_RTOS_MQX" -D"PEX_MQX_KSDK" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/hal/inc" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/hal/src/sim/MK64F12" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/system/src/clock/MK64F12" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/system/inc" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/osa/inc" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/CMSIS/Include" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/devices" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/devices/MK64F12/include" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/utilities/src" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/utilities/inc" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/devices/MK64F12/startup" -I"C:/Users/sat/ceng455/CENG455/serial_echo/Generated_Code/SDK/platform/devices/MK64F12/startup" -I"C:/Users/sat/ceng455/CENG455/serial_echo/Sources" -I"C:/Users/sat/ceng455/CENG455/serial_echo/Generated_Code" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/drivers/inc" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/psp/cortex_m" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/psp/cortex_m/cpu" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/config/common" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/include" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/bsp" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/psp/cortex_m/compiler/gcc_arm" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio/src" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio/fs" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio/drivers/nio_dummy" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio/drivers/nio_serial" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio/drivers/nio_tty" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx_stdlib/source/include" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx_stdlib/source/stdio" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/hal/src/uart" -I"C:/Users/sat/ceng455/CENG455/serial_echo/SDK/platform/drivers/src/uart" -I"C:/Users/sat/ceng455/CENG455/serial_echo/Generated_Code/SDK/rtos/mqx/config/board" -I"C:/Users/sat/ceng455/CENG455/serial_echo/Generated_Code/SDK/rtos/mqx/config/mcu" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


