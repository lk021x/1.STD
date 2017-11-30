################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../AES/aes_modes.c \
../AES/aescrypt.c \
../AES/aeskey.c \
../AES/aestab.c 

OBJS += \
./AES/aes_modes.o \
./AES/aescrypt.o \
./AES/aeskey.o \
./AES/aestab.o 

C_DEPS += \
./AES/aes_modes.d \
./AES/aescrypt.d \
./AES/aeskey.d \
./AES/aestab.d 


# Each subdirectory must supply rules for building sources it contributes
AES/%.o: ../AES/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gcc -O1 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


