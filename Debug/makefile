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
-include AES/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: LCD_EMS200-V11_V1_A_M.bin

# Tool invocations
LCD_EMS200-V11_V1_A_M.bin: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-linux-g++ -L"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/embf" -L"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/iconv/lib" -L"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/libevent" -L"/nfs_root/git/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/MThread" -o "LCD_EMS200-V11_V1_A_M.bin" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) LCD_EMS200-V11_V1_A_M.bin
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
