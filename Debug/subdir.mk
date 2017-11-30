################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../main.cpp 

OBJS += \
./main.o 

CPP_DEPS += \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-g++ -I"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/includes" -I"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/UdpUtil" -I"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/libevent" -I"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/libevent/event2" -I"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/AES" -I"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/includes/web" -I"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/embf" -I"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/MThread" -I"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/TcpUtil" -O1 -g3 -ggdb -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


