#ifndef GLOBALA_H_
#define GLOBALA_H_

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <termios.h>            /* tcgetattr, tcsetattr */
#include <stdio.h>              /* perror, printf, puts, fprintf, fputs */
#include <unistd.h>             /* read, write, close */
#include <fcntl.h>              /* open */
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>             /* bzero, memcpy */
#include <limits.h>             /* CHAR_MAX */
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <time.h>
#include <linux/watchdog.h>

#define TRUE  true
#define FALSE false

#define SYS_TEST        1 		//open test
#define G_DEBUG         1 		//open debug
#define USE_FULL_ASSERT 1 		//open assert

#ifndef UINT8_MAX
#define UINT8_MAX				255
#endif

#ifndef UINT16_MAX
#define UINT16_MAX				65535
#endif

#ifndef UINT32_MAX
#define UINT32_MAX				4294967295u
#endif

#define EMBF_VER "EMBF-V1-A(1.0.8)(P-A-000092)"

#if G_DEBUG
#define DBPRINTF(format, arg...) { printf("%s:%d::",__FILE__,__LINE__);printf(format, ## arg); }
#else
#define DBPRINTF(format, arg...)
#endif

//save already drv handles
typedef struct {
    int32_t fserial1;	// serial port1
    int32_t fserial2;	// serial port2
    int32_t fserial3; 	// serial port3
    int32_t fserial4; 	// serial port4
    int32_t fserial5; 	// serial port5
    int32_t fserial6; 	// serial port6
    int32_t fserial7; 	// serial port7
    int32_t fserial8; 	// serial port8
    int32_t fserial9; 	// serial port9
    int32_t fserial10;	// serial port10
    int32_t flcd;     	// LCD
    int32_t fadc1;    	// ADC
    int32_t fads1247; 	// ext ADC
    int32_t fdcan1;   	// CAN1
    int32_t fdcan2;   	// CAN2
    int32_t frtc;     	// RTC
    int32_t fextwdg;  	// ext wdg
    int32_t fnet;     	// net work
    int32_t fdido;    	// DIDO
    int32_t fsd;      	// SD
}t_gparam,*p_gparam;
extern volatile t_gparam gt_glp;

typedef struct
{
	int32_t baudrate;  		// 1200 2400 4800 9600 .. 115200
	int32_t databit;		// 5, 6, 7, 8
	char *stopbit;			//  "1", "1.5", "2"
	char parity;			// N(o), O(dd), E(ven)
	uint8_t vmin;
}serial_param,*p_serial_param;

enum CLIENT_PROTOCOL_TYPE
{
    CLIENT_PROTOCOL_PCS_TCPMODBUS=0,
    CLIENT_PROTOCOL_IEC104,
    CLIENT_PROTOCOL_PV_TCPMODBUS,
};

enum SERIAL_PROTOCOL_TYPE
{
	SERIAL_PROTOCOL_MODBUS_TOUCH=0,
	SERIAL_PROTOCOL_MODBUS_PCS,
	SERIAL_PROTOCOL_MODBUS_PV,
	SERIAL_PROTOCOL_MODBUS_DCDC,
	SERIAL_PROTOCOL_MODBUS_BMS,
	SERIAL_PROTOCOL_AMMETER,
	SERIAL_PROTOCOL_MODBUS_PVS,
	SERIAL_PROTOCOL_MODBUS_MONITOR,
	SERIAL_PROTOCOL_MODBUS_ENV,//SERIAL_PROTOCOL_MODBUS_BAK1,
	SERIAL_PROTOCOL_MODBUS_BAK2,
	SERIAL_PROTOCOL_MODBUS_NONE  //10
};

enum CLIENT_CONNECT_STATE
{
	CLIENT_DISCONNECT=0,
	CLIENT_CONNECT,
};

enum FailNum{
	SUCCESS = 0x00000000,
	FILE_NOT_EXSIT = 0x00000007,
	MEDIA_I_REQUEST = 0x00011000,
	MEDIA_RERIGISTER = 0x00011001,
	MEDIA_REGISTER_CLOSE = 0x00011002,
	MEDIA_CONNECT_FAIL = 0x000110003,
	MEDIA_RIGISTER_FAIL = 0x00010001,
	MEDIA_RIGISTER_REPLY_FAIL = 0x00010002,
	MEDIA_RIGISTER_NO_REPLY = 0x00010003,
	TASK_EXISTED = 0x000100A1,
	TASK_NO_EXIST = 0x000100A2,
	TASK_TIME_OUT = 0x00011003,
	SESSION_END = 0x00010004,
	SESSION_START = 0x000110004,
	GPS_ERR = 0x00020001,
	RECORD_SEARCH_FAIL = 0x00020019,
	UPGRADE_RECFILE_FAIL = 0x00020050,
	UPGRADE_CHECK_FAIL = 0x00020051,
	UPGRADE_GETPATH_FAIL = 0x00020052,
	UPGRADE_OPENFILE_FAIL = 0x00020053,
	UPGRADE_GETHEAD_FAIL = 0x00020054,
	UPGRADE_FLASH_ERR = 0x000100A3,
	UPGRADE_TASK_ERR = 0x000100A2
};

#endif

