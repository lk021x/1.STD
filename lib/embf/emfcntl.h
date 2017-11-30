/*
 * fcntl.h
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is a part of the mingw-runtime package.
 * No warranty is given; refer to the file DISCLAIMER within the package.
 *
 * Access constants for _open. Note that the permissions constants are
 * in sys/stat.h (ick).
 *
 */
#ifndef _FCNTLA_H
#define _FCNTLA_H

#include "global.h"

#define DSUCCESS     0
#define OPFAULT      (-1)
#define DEVBUSY      (-2)

#define SEEK_SET     0
#define SEEK_CUR     1
#define SEEK_END     2

#define DRV_POLLIN   1
#define DRV_POLLOUT  2

//gpio drv
#define GPIO_INPUT_PATH					"/sys/devices/ocp.2/44e0b000.i2c/i2c-0/0-0024/gpio"
#define GPIO_OUTPUT_PATH 				"/sys/devices/virtual/gpio"

//gpio mode
#define GPIO_INPUT 						0x01
#define GPIO_OUTPUT 					0x02
#define GPIO_ANALOG     				0x04

//GPIO45 -> D01
//GPIO44 -> DO2
//GPIO27 -> D03
//GPIO26 -> D04

//GPIO64 -> RUN    0 -- 亮 1 -- 灭
//GPIO65 -> FAULT  0 -- 亮 1 -- 灭
//GPIO68 -> BLE    0 -- 亮 1 -- 灭
#define GPIO_LIGHT_RUN					"/sys/devices/virtual/gpio/gpio64/value"
#define GPIO_LIGHT_FAULT				"/sys/devices/virtual/gpio/gpio65/value"
#define GPIO_LIGHT_BLE					"/sys/devices/virtual/gpio/gpio68/value"

//GPIO496   DIN1
//GPIO511   DIN2
//GPIO497   DIN3
//GPIO510   DIN4
//GPIO498   DIN5
//GPIO509   DIN6
//GPIO499   DIN7
//GPIO508   DIN8
//GPIO500   DIN9
//GPIO507   DIN10
//GPIO501   DIN11
//GPIO506   DIN12
//GPIO502   DIN13
//GPIO505   DIN14
//GPIO503   DIN15
//GPIO504   DIN16

//ecap
#define ECAP    						"/sys/class/pwm/ecap.0"
//serial drv
//#define TTYO0							"/dev/ttyO0"   	//调试口使用
#define TTYO1  							"/dev/ttyO1"
#define TTYO2							"/dev/ttyO2"
#define TTYO3  							"/dev/ttyO3"
#define TTYO4  							"/dev/ttyO4"
#define TTYO5  							"/dev/ttyXRUSB0"
#define TTYO6  							"/dev/ttyXRUSB1"
#define TTYS0							"/dev/ttyS0"
#define TTYS1							"/dev/ttyS1"
#define TTYS2							"/dev/ttyS2"
#define TTYS3							"/dev/ttyS3"
#define TTYS4							"/dev/ttyS4"
#define TTYS5							"/dev/ttyS5"
//wdogps
#define WDT     						"/dev/watchdog"
//can
#define CAN0							"can0"
#define CAN1							"can1"

//GPS
#define TTYGPS  						"/dev/ttyXRUSB2"

//CAN

#define CAN_BAUD_1Mbps      			(uint32_t)0x00000000U
#define CAN_BAUD_500Kbps    			(uint32_t)0x00000001U
#define CAN_BAUD_250Kbps    			(uint32_t)0x00000002U
#define CAN_BAUD_200Kbps    			(uint32_t)0x00000003U
#define CAN_BAUD_100Kbps   				(uint32_t)0x00000004U
#define CAN_BAUD_80Kbps     			(uint32_t)0x00000005U
#define CAN_BAUD_30Kbps     			(uint32_t)0x00000006U
#define CAN_BAUD_20Kbps     			(uint32_t)0x00000007U

//控制设备类型码
#define CTRL_GPIO						(uint32_t)0x00000100U
#define CTRL_SERIAL						(uint32_t)0x00000200U
#define CTRL_CAN						(uint32_t)0x00000300U
#define CTRL_NET						(uint32_t)0x00000400U

//ADC
#define ADC1  							"/sys/devices/ocp.2/48030000.spi/spi_master/spi1/spi1.0/spiadc"

//控制功能码
#define USART_SET_BAUD    				2U      //设置波特率
#define USART_GET_BAUD    				3U      //获取波特率
#define USART_SET_STOPBIT   			4U      //设置停止位
#define USART_SET_PARITY 	 			5U 		//设置校验位
#define USART_SET_DATA   				6U 		//设置数据位

#define STOPBIT1						1
#define STOPBIT1_5						2
#define STOPBIT2						3
#define DATABIT5						5
#define DATABIT6						6
#define DATABIT7						7
#define DATABIT8						8
#define NONE							1
#define ODD								2
#define EVEN							3

#define NET_SET_INIT_ALL      			1U
#define NET_SET_IP            			2U
#define NET_SET_SUBMASK       			3U
#define NET_SET_MAC           			4U
#define NET_SET_GATEWAY       			5U
#define NET_SET_DHCP_ON       			6U
#define NET_SET_DHCP_OFF      			7U
#define NET_GET_IP            			8U
#define NET_GET_SUBMASK       			9U
#define NET_GET_MAC           		   10U
#define NET_GET_GATEWAY       		   11U
#define NET_GET_DHCP_STATUS   		   12U
#define NET_GET_CONNECTED     		   13U
#define NET_GET_DNS1          		   14U
#define NET_GET_DNS2          		   15U

#define CAN_SET_PORT        			1U
#define CAN_SET_FILTER      			2U
#define CAN_GET_ERR         			3U
#define CAN_SET_BAUD        			4U

#ifdef __cplusplus
extern "C" {
#endif

int32_t drv_open(const char* pathname, int32_t flags, int32_t mode);
int32_t drv_read(int32_t fd, void* buf,uint32_t count);
int32_t drv_write(int32_t fd, const void* buf, uint32_t count);
int32_t drv_read_gpio(const char* pathname, uint8_t* buf,uint32_t count);
int32_t drv_read_adc(const char* pathname, void* buf, uint32_t count);
int32_t drv_write_gpio(const char* pathname, int32_t flags, void* arg);
int32_t drv_write_light_gpio(const char* path, uint8_t value);
int32_t drv_close(int32_t fd);

//request(00 IO引脚| 00 输出值| 00 设备类型| 00 功能类型)
int32_t drv_ioctl(int32_t fildes, uint32_t request,void* arg);
int Create_Process_ID();
void System_Reboot();
#ifdef __cplusplus
}
#endif

#endif  /* Not _FCNTL_H_ */

