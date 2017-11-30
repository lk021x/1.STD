################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../lib/socket_pool/ErrorLog.cpp \
../lib/socket_pool/SocketBase.cpp \
../lib/socket_pool/SocketThread.cpp \
../lib/socket_pool/TcpSocket.cpp \
../lib/socket_pool/Thread.cpp \
../lib/socket_pool/ThreadPool.cpp 

OBJS += \
./lib/socket_pool/ErrorLog.o \
./lib/socket_pool/SocketBase.o \
./lib/socket_pool/SocketThread.o \
./lib/socket_pool/TcpSocket.o \
./lib/socket_pool/Thread.o \
./lib/socket_pool/ThreadPool.o 

CPP_DEPS += \
./lib/socket_pool/ErrorLog.d \
./lib/socket_pool/SocketBase.d \
./lib/socket_pool/SocketThread.d \
./lib/socket_pool/TcpSocket.d \
./lib/socket_pool/Thread.d \
./lib/socket_pool/ThreadPool.d 


# Each subdirectory must supply rules for building sources it contributes
lib/socket_pool/%.o: ../lib/socket_pool/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/nfs_root/LCD_EMS100-100K-V11_V1_B_M/includes" -I"/nfs_root/LCD_EMS100-100K-V11_V1_B_M/lib/libev" -I"/nfs_root/LCD_EMS100-100K-V11_V1_B_M/AES" -I"/nfs_root/LCD_EMS100-100K-V11_V1_B_M/includes/web" -I"/nfs_root/LCD_EMS100-100K-V11_V1_B_M/lib/embf" -I"/nfs_root/LCD_EMS100-100K-V11_V1_B_M/lib/MThread" -I"/nfs_root/LCD_EMS100-100K-V11_V1_B_M/TcpUtil" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


