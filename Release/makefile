################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include ssesion/subdir.mk
-include lib/embf/subdir.mk
-include UdpUtil/subdir.mk
-include TcpUtil/subdir.mk
-include MQTT/subdir.mk
-include AES/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: LCD_EMS200-V11_V1_A_M

# Tool invocations
LCD_EMS200-V11_V1_A_M: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-linux-g++ -L"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/embf" -L"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/libevent" -L"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/MThread" -o "LCD_EMS200-V11_V1_A_M" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) LCD_EMS200-V11_V1_A_M
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
