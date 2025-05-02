################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/SEGGER_RTT/SEGGER_RTT.c \
../src/SEGGER_RTT/SEGGER_RTT_printf.c 

C_DEPS += \
./src/SEGGER_RTT/SEGGER_RTT.d \
./src/SEGGER_RTT/SEGGER_RTT_printf.d 

OBJS += \
./src/SEGGER_RTT/SEGGER_RTT.o \
./src/SEGGER_RTT/SEGGER_RTT_printf.o 

SREC += \
RTOS_project_1.srec 

MAP += \
RTOS_project_1.map 


# Each subdirectory must supply rules for building sources it contributes
src/SEGGER_RTT/%.o: ../src/SEGGER_RTT/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_CORE=CM33 -D_RA_ORDINAL=1 -I"C:/Projects/Renasas e2 studio/RTOS_project_1/src" -I"." -I"C:/Projects/Renasas e2 studio/RTOS_project_1/ra/fsp/inc" -I"C:/Projects/Renasas e2 studio/RTOS_project_1/ra/fsp/inc/api" -I"C:/Projects/Renasas e2 studio/RTOS_project_1/ra/fsp/inc/instances" -I"C:/Projects/Renasas e2 studio/RTOS_project_1/ra/fsp/src/rm_freertos_port" -I"C:/Projects/Renasas e2 studio/RTOS_project_1/ra/aws/FreeRTOS/FreeRTOS/Source/include" -I"C:/Projects/Renasas e2 studio/RTOS_project_1/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"C:/Projects/Renasas e2 studio/RTOS_project_1/ra_gen" -I"C:/Projects/Renasas e2 studio/RTOS_project_1/ra_cfg/fsp_cfg/bsp" -I"C:/Projects/Renasas e2 studio/RTOS_project_1/ra_cfg/fsp_cfg" -I"C:/Projects/Renasas e2 studio/RTOS_project_1/ra_cfg/aws" -std=c99 -Wno-stringop-overflow -Wno-format-truncation --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

