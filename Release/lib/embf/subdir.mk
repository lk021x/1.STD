################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/embf/emfcntl.c \
../lib/embf/serial.c \
../lib/embf/utils.c 

OBJS += \
./lib/embf/emfcntl.o \
./lib/embf/serial.o \
./lib/embf/utils.o 

C_DEPS += \
./lib/embf/emfcntl.d \
./lib/embf/serial.d \
./lib/embf/utils.d 


# Each subdirectory must supply rules for building sources it contributes
lib/embf/%.o: ../lib/embf/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gcc -O2 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


