################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../ssesion/adc_thread.cpp \
../ssesion/app.cpp \
../ssesion/can_thread.cpp \
../ssesion/com_io_ammeter.cpp \
../ssesion/com_io_bms.cpp \
../ssesion/com_io_dcdc.cpp \
../ssesion/com_io_dsp.cpp \
../ssesion/com_io_env.cpp \
../ssesion/com_io_monitor.cpp \
../ssesion/com_io_pcs.cpp \
../ssesion/com_io_pv.cpp \
../ssesion/com_io_pvs.cpp \
../ssesion/com_io_touch.cpp \
../ssesion/common_data.cpp \
../ssesion/common_function.cpp \
../ssesion/connect_thread.cpp \
../ssesion/ems.cpp \
../ssesion/ems_calc.cpp \
../ssesion/http_interface.cpp \
../ssesion/http_server.cpp \
../ssesion/iapv2.cpp \
../ssesion/iapv2_interface.cpp \
../ssesion/iec104exe.cpp \
../ssesion/iec104rtu.cpp \
../ssesion/log.cpp \
../ssesion/log_interface.cpp \
../ssesion/myLog.cpp \
../ssesion/net_event_thread.cpp \
../ssesion/net_manage_thread.cpp \
../ssesion/net_ntp_thread.cpp \
../ssesion/net_rev_thread.cpp \
../ssesion/net_send_thread.cpp \
../ssesion/net_server_thread.cpp \
../ssesion/net_util.cpp \
../ssesion/property.cpp \
../ssesion/property_interface.cpp \
../ssesion/serial_rev_thread.cpp \
../ssesion/serial_send_thread.cpp \
../ssesion/thread_ctrl.cpp \
../ssesion/timer_thread.cpp 

OBJS += \
./ssesion/adc_thread.o \
./ssesion/app.o \
./ssesion/can_thread.o \
./ssesion/com_io_ammeter.o \
./ssesion/com_io_bms.o \
./ssesion/com_io_dcdc.o \
./ssesion/com_io_dsp.o \
./ssesion/com_io_env.o \
./ssesion/com_io_monitor.o \
./ssesion/com_io_pcs.o \
./ssesion/com_io_pv.o \
./ssesion/com_io_pvs.o \
./ssesion/com_io_touch.o \
./ssesion/common_data.o \
./ssesion/common_function.o \
./ssesion/connect_thread.o \
./ssesion/ems.o \
./ssesion/ems_calc.o \
./ssesion/http_interface.o \
./ssesion/http_server.o \
./ssesion/iapv2.o \
./ssesion/iapv2_interface.o \
./ssesion/iec104exe.o \
./ssesion/iec104rtu.o \
./ssesion/log.o \
./ssesion/log_interface.o \
./ssesion/myLog.o \
./ssesion/net_event_thread.o \
./ssesion/net_manage_thread.o \
./ssesion/net_ntp_thread.o \
./ssesion/net_rev_thread.o \
./ssesion/net_send_thread.o \
./ssesion/net_server_thread.o \
./ssesion/net_util.o \
./ssesion/property.o \
./ssesion/property_interface.o \
./ssesion/serial_rev_thread.o \
./ssesion/serial_send_thread.o \
./ssesion/thread_ctrl.o \
./ssesion/timer_thread.o 

CPP_DEPS += \
./ssesion/adc_thread.d \
./ssesion/app.d \
./ssesion/can_thread.d \
./ssesion/com_io_ammeter.d \
./ssesion/com_io_bms.d \
./ssesion/com_io_dcdc.d \
./ssesion/com_io_dsp.d \
./ssesion/com_io_env.d \
./ssesion/com_io_monitor.d \
./ssesion/com_io_pcs.d \
./ssesion/com_io_pv.d \
./ssesion/com_io_pvs.d \
./ssesion/com_io_touch.d \
./ssesion/common_data.d \
./ssesion/common_function.d \
./ssesion/connect_thread.d \
./ssesion/ems.d \
./ssesion/ems_calc.d \
./ssesion/http_interface.d \
./ssesion/http_server.d \
./ssesion/iapv2.d \
./ssesion/iapv2_interface.d \
./ssesion/iec104exe.d \
./ssesion/iec104rtu.d \
./ssesion/log.d \
./ssesion/log_interface.d \
./ssesion/myLog.d \
./ssesion/net_event_thread.d \
./ssesion/net_manage_thread.d \
./ssesion/net_ntp_thread.d \
./ssesion/net_rev_thread.d \
./ssesion/net_send_thread.d \
./ssesion/net_server_thread.d \
./ssesion/net_util.d \
./ssesion/property.d \
./ssesion/property_interface.d \
./ssesion/serial_rev_thread.d \
./ssesion/serial_send_thread.d \
./ssesion/thread_ctrl.d \
./ssesion/timer_thread.d 


# Each subdirectory must supply rules for building sources it contributes
ssesion/%.o: ../ssesion/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-g++ -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/includes" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/UdpUtil" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/libevent" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/libevent/event2" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/AES" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/includes/web" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/embf" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/lib/MThread" -I"/nfs_root/1.EMS200/LCD_EMS200-V11_V1_A_M/TcpUtil" -O2 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


