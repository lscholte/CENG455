################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDK/rtos/mqx/mqx_stdlib/source/strings/strings.c 

OBJS += \
./SDK/rtos/mqx/mqx_stdlib/source/strings/strings.o 

C_DEPS += \
./SDK/rtos/mqx/mqx_stdlib/source/strings/strings.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/rtos/mqx/mqx_stdlib/source/strings/%.o: ../SDK/rtos/mqx/mqx_stdlib/source/strings/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK64FN1M0VLL12" -D"FSL_RTOS_MQX" -D"PEX_MQX_KSDK" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/hal/inc" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/hal/src/sim/MK64F12" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/system/src/clock/MK64F12" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/system/inc" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/osa/inc" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/CMSIS/Include" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/devices" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/devices/MK64F12/include" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/utilities/src" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/utilities/inc" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/devices/MK64F12/startup" -I"C:/Users/lscholte/CENG455/dd_scheduler/Generated_Code/SDK/platform/devices/MK64F12/startup" -I"C:/Users/lscholte/CENG455/dd_scheduler/Sources" -I"C:/Users/lscholte/CENG455/dd_scheduler/Generated_Code" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/drivers/inc" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx/source/psp/cortex_m" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx/source/psp/cortex_m/cpu" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/config/common" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx/source/include" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx/source/bsp" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx/source/psp/cortex_m/compiler/gcc_arm" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx/source/nio" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx/source/nio/src" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx/source/nio/fs" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx/source/nio/drivers/nio_dummy" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx/source/nio/drivers/nio_serial" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx/source/nio/drivers/nio_tty" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx_stdlib/source/include" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/rtos/mqx/mqx_stdlib/source/stdio" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/hal/src/uart" -I"C:/Users/lscholte/CENG455/dd_scheduler/SDK/platform/drivers/src/uart" -I"C:/Users/lscholte/CENG455/dd_scheduler/Generated_Code/SDK/rtos/mqx/config/board" -I"C:/Users/lscholte/CENG455/dd_scheduler/Generated_Code/SDK/rtos/mqx/config/mcu" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


