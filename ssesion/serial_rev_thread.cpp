/*
 * serial_thread.cpp
 *
 *  Created on: 2017-1-12
 *      Author: root
 */

#include <fcntl.h>              /* open */
#include "includes.h"
#include "serial_rev_thread.h"

SerialRevThread::SerialRevThread()
{
	m_bStart = true;
	fd = -1;
	ret = -1;
	rLen = 0;
	sLen = 0;
	pos = 0;
	Time_cur = 0;
	Time_last = 0;
	protocol_type = SERIAL_PROTOCOL_MODBUS_PCS;
	st_Id = 0;
	en_Id = 1;
	m_baud = 9600;
	vmin = 255;
	memset(Path, 0x00, 128);
}

SerialRevThread::~SerialRevThread()
{

}

int SerialRevThread::SetComParam(COM_PARAM param)
{
	st_Id = param.sId;
	en_Id = param.eId;
	protocol_type = param.protocol;
	m_baud = param.baud;
	vmin = param.vmin;
	pos=0;
	return 0;
}

int SerialRevThread::GetProtocol()
{
	return protocol_type;
}

int SerialRevThread::GetBeginID()
{
	return st_Id;
}

int SerialRevThread::GetEndID()
{
	return en_Id;
}

int SerialRevThread::Set_Serial_Path(const char* path)
{
	memset(Path, 0x00, sizeof(Path));
	memcpy(Path, path, strlen(path));
	return 0;
}

int SerialRevThread::Set_Baud(uint32_t baud)
{
	uint32_t temp=0;
	if (fd < 0) return -1;
	m_baud = baud;
	pos=0;
	//printf("%s update baud is %d vmin %d ", Path, m_baud, vmin);
	temp = (vmin<<16)|m_baud;
	drv_ioctl(fd, CTRL_SERIAL | USART_SET_BAUD, &temp);
	if(protocol_type == SERIAL_PROTOCOL_AMMETER && System_Info.Config.Ammeter_Protocol_Type == 1)  //DLT645电表
	{
		temp = (uint32_t)EVEN;
	}
	else
	{
		temp = (uint32_t)NONE;
	}
	//printf("parity %d \n", temp);
	drv_ioctl(fd, CTRL_SERIAL | USART_SET_PARITY, &temp);
	return 0;
}

int SerialRevThread::Open(void)
{
	if (strlen(Path) == 0) return -1;

	//O_NDELAY 非阻塞（默认为阻塞，打开后也可以使用fcntl()重新设置）
	fd = drv_open((char*) Path, O_RDWR | O_NOCTTY, 0);   //设置为阻塞模式
	if (fd < 0)
	{
		//printf("Error: Opening Com %s\n", Path);
		return -1;
	}
	else
	{
		uint32_t temp = 0, parity = 0;
		temp = (uint32_t)(vmin<<16)|m_baud;
		drv_ioctl(fd, CTRL_SERIAL | USART_SET_BAUD, &temp);
		if(protocol_type == SERIAL_PROTOCOL_AMMETER && System_Info.Config.Ammeter_Protocol_Type == 1)  //DLT645电表
		{
			parity = (uint32_t)EVEN;
		}
		else
		{
			parity = (uint32_t)NONE;
		}
		drv_ioctl(fd, CTRL_SERIAL | USART_SET_PARITY, &parity);
		//printf("Success: Open Com %s, Baud %d, Parity %d, Vim %d, Protocol %d, ST %d END %d\n", Path, drv_ioctl(fd, CTRL_SERIAL | USART_GET_BAUD, 0),
			//	parity,vmin,protocol_type, st_Id, en_Id);
		return 0;
	}

	return 0;
}

int SerialRevThread::Get_FD()
{
	return fd;
}

inline int SerialRevThread::CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	cur = (int64_t)tv.tv_sec*1000+(int64_t)tv.tv_usec/1000; //毫秒

	if(last==0 || cur<last){last=cur;return 0;}
	if(cur > last && (cur-last) >= ms){ //定时时间到
		last = cur;
		return 1;
	}else{
		return 0;
	}

	return 0;
}

void SerialRevThread::run()
{
	if (Open() < 0) m_bStart = false;

	while (1)
	{
		while (m_bStart)
		{
			switch (protocol_type)
			{
				case SERIAL_PROTOCOL_MODBUS_TOUCH:
					rLen = drv_read(fd, &rBuf[pos], vmin);
					if (rLen > 0)
					{
						//for(int j=0; j<rLen; j++) printf("%02x ", rBuf[pos+j]);
						sLen = EMS_Receive_Data_From_Touch_By_UART(rBuf, pos+rLen, sBuf);
						if (sLen > 0)
						{
							drv_write(fd, sBuf, sLen);
							pos=0;
							//printf(" : ");for(int j=0; j<sLen; j++) printf("%02x ", sBuf[j]);printf("\r\n");
						}
						else if(sLen == -1)
						{
							pos += rLen;
							if(pos>=21)pos=0; //屏下发数据最大值21
						}
					}
					usleep(10); //小于20ms 推荐10，影响屏数据的刷新速度
					break;
				case SERIAL_PROTOCOL_MODBUS_MONITOR:
					rLen = drv_read(fd, rBuf, vmin);
					if (rLen > 0)
					{
						mutex_for_timer.lock();
						sLen = EMS_Receive_Data_From_Monitor_By_UART_Modbus(rBuf, rLen, sBuf);
						mutex_for_timer.unlock();
						if (sLen > 0)
						{
							drv_write(fd, sBuf, sLen);
						}
					}
					usleep(10);
					break;
				case SERIAL_PROTOCOL_MODBUS_PCS:
					if(System_Info.Config.PCS_Comm_Type == PCS_COMM_SERIAL && webPageShow.pcs == 1)
					{
						rLen = drv_read(fd, rBuf, 205);
						if (rLen > 0)
						{
							//printf("rec :");for(int j=0; j<rLen; j++) printf(" %02x", rBuf[j]);printf("\r\n");
							mutex_for_timer.lock();
							EMS_Process_Data_From_PCS_By_UART(rBuf, rLen);
							mutex_for_timer.unlock();
						}
					}else{
						sleep(2);
					}
					break;
				case SERIAL_PROTOCOL_MODBUS_PV:
					if (DC_BUS_SYS == 0 && System_Info.Config.PV_Comm_Type == PV_COMM_SERIAL && webPageShow.pv == 1)
					{
						rLen = drv_read(fd, rBuf, 171);
						if (rLen > 0)
						{
							//printf("rec:");for(int j=0; j<rLen; j++) printf(" %02x", rBuf[j]);printf("\r\n");
							mutex_for_timer.lock();
							EMS_Process_Data_From_PV_By_UART(rBuf, rLen);
							mutex_for_timer.unlock();
						}
					}else{
						sleep(2);
					}
					break;
				case SERIAL_PROTOCOL_MODBUS_PVS:
					if (webPageShow.pvs == 1)
					{
						rLen = drv_read(fd, rBuf, 79);
						if (rLen > 0)
						{
							mutex_for_timer.lock();
							EMS_Process_Data_From_PVS_By_UART(rBuf, rLen);
							mutex_for_timer.unlock();
						}
					}else{
						sleep(2);
					}
					break;
				case SERIAL_PROTOCOL_MODBUS_ENV:
					if (webPageShow.env == 1)
					{
						rLen = drv_read(fd, rBuf, 105);
						if (rLen > 0)
						{
							mutex_for_timer.lock();
							EMS_Process_Data_From_ENV_By_UART(rBuf, rLen);
							mutex_for_timer.unlock();
						}
					}else{
						sleep(2);
					}
					break;
				case SERIAL_PROTOCOL_MODBUS_DCDC:
					if (DC_BUS_SYS == 1 && System_Info.Config.DCDC_Type != 0 && webPageShow.dcdc == 1)
					{
						rLen = drv_read(fd, rBuf, 127);
						if (rLen > 0)
						{
							mutex_for_timer.lock();
							EMS_Process_Data_From_DCDC_By_UART(rBuf, rLen);
							mutex_for_timer.unlock();
						}
					}else{
						sleep(2);
					}
					break;
				case SERIAL_PROTOCOL_MODBUS_BMS:
					if(System_Info.Config.BMS_Type != 0 && webPageShow.bms == 1)
					{
						if (System_Info.Config.BMS_Type == BMS_68AH){
							rLen = drv_read(fd, rBuf, 105);
							if (rLen > 0)
							{
								//printf("rec:");for(int j=0; j<rLen; j++) printf(" %02x", rBuf[j]);printf("\r\n");
								mutex_for_timer.lock();
								EMS_Process_Data_From_BMS_By_UART(rBuf, rLen);
								mutex_for_timer.unlock();
							}
						}else if (System_Info.Config.BMS_Type == BMS_94AH){
							rLen = drv_read(fd, rBuf, 67);
							if (rLen > 0)
							{
								//printf("rec:");for(int j=0; j<rLen; j++) printf(" %02x", rBuf[j]);printf("\r\n");
								mutex_for_timer.lock();
								EMS_Process_Data_From_BMS_By_UART(rBuf, rLen);
								mutex_for_timer.unlock();
							}
						}else{
							sleep(2);
						}
					}else{
						sleep(2);
					}
					break;
				case SERIAL_PROTOCOL_AMMETER:
					//if(System_Info.Config.Meter_Comm_Enable == 0xAA)
					if (webPageShow.ammeter == 1)
					{
						if(System_Info.Config.Ammeter_Protocol_Type == 0){
							rLen = drv_read(fd, rBuf, 61);
							if (rLen > 0)
							{
								//printf("rec :");for(int j=0; j<rLen; j++) printf(" %02x", rBuf[j]);printf("\r\n");
								mutex_for_timer.lock();
								EMS_Process_Data_From_Ammeter_By_UART_For_Modbus(rBuf, rLen);
								mutex_for_timer.unlock();
							}
						}else{
							rLen = drv_read(fd, rBuf, 75);
							if (rLen > 0)
							{
								//printf("rec :");for(int j=0; j<rLen; j++) printf(" %02x", rBuf[j]);printf("\r\n");
								mutex_for_timer.lock();
								EMS_Process_Data_From_Ammeter_By_UART_For_DLT645(rBuf, rLen);
								mutex_for_timer.unlock();
							}
						}
					}
					else
					{
						sleep(2);
					}
					break;
				default:
					sleep(2);  //未处理的协议
					break;
			}
		}
		//printf("%s rec stop\n", Path);
		sleep(2);
	}
}
