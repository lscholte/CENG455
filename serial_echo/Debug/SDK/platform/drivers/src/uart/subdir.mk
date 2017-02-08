################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../SDK/platform/drivers/src/uart/fsl_uart_common.c \
../SDK/platform/drivers/src/uart/fsl_uart_driver.c 

OBJS += \
./SDK/platform/drivers/src/uart/fsl_uart_common.o \
./SDK/platform/drivers/src/uart/fsl_uart_driver.o 

C_DEPS += \
./SDK/platform/drivers/src/uart/fsl_uart_common.d \
./SDK/platform/drivers/src/uart/fsl_uart_driver.d 


# Each subdirectory must supply rules for building sources it contributes
SDK/platform/drivers/src/uart/%.o: ../SDK/platform/drivers/src/uart/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK64FN1M0VLL12" -D"FSL_RTOS_MQX" -D"PEX_MQX_KSDK" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/hal/inc" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/hal/src/sim/MK64F12" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/system/src/clock/MK64F12" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/system/inc" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/osa/inc" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/CMSIS/Include" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/devices" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/devices/MK64F12/include" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/utilities/src" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/utilities/inc" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/devices/MK64F12/startup" -I"C:/ceng455/CENG455/serial_echo/Generated_Code/SDK/platform/devices/MK64F12/startup" -I"C:/ceng455/CENG455/serial_echo/Sources" -I"C:/ceng455/CENG455/serial_echo/Generated_Code" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/drivers/inc" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/psp/cortex_m" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/psp/cortex_m/cpu" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/config/common" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/include" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/bsp" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/psp/cortex_m/compiler/gcc_arm" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio/src" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio/fs" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio/drivers/nio_dummy" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio/drivers/nio_serial" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx/source/nio/drivers/nio_tty" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx_stdlib/source/include" -I"C:/ceng455/CENG455/serial_echo/SDK/rtos/mqx/mqx_stdlib/source/stdio" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/hal/src/uart" -I"C:/ceng455/CENG455/serial_echo/SDK/platform/drivers/src/uart" -I"C:/ceng455/CENG455/serial_echo/Generated_Code/SDK/rtos/mqx/config/board" -I"C:/ceng455/CENG455/serial_echo/Generated_Code/SDK/rtos/mqx/config/mcu" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


