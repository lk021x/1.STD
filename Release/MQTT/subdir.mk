################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MQTT/MQTTRTU.cpp 

OBJS += \
./MQTT/MQTTRTU.o 

CPP_DEPS += \
./MQTT/MQTTRTU.d 


# Each subdirectory must supply rules for building sources it contributes
MQTT/%.o: ../MQTT/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-g++ -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/includes" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/UdpUtil" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/libevent" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/libevent/event2" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/AES" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/includes/web" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/embf" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/MThread" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/TcpUtil" -O2 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


