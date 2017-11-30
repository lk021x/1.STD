/*
 * can_thread.cpp
 *
 *  Created on: 2017-1-12
 *      Author: root
 */

#include "includes.h"
#include "can_thread.h"

CanThread::CanThread()
{
	m_bStart = true;
	fd = -1;
	mode = 0;
	rLen = 0;
	sLen = 0;
	Time_cur = 0;
	Time_last = 0;
}

CanThread::~CanThread()
{

}

int CanThread::SetDevicePath(const char* path)
{
	memset(Path, 0x00, sizeof(Path));
	memcpy(Path, path, strlen(path));
	return 0;
}

int CanThread::Open(void)
{
	if (strlen(Path) == 0) return -1;
	fd = drv_open((char*) Path, 0, 0);
	if (fd < 0)
	{
		printf("Error: Opening %s err\n", Path);
		return -1;
	}
	else
	{
		//if(mode == REV_MODE) printf("Success: Open %s REV_MODE ok\n", Path);
		//else if(mode == SEND_MODE) printf("Success: Open %s SEND_MODE ok\n", Path);
		return 0;
	}

	return 0;
}

int CanThread::Get_FD()
{
	return fd;
}

inline int CanThread::CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur)
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

int CanThread::recv_frame(const int sockfd, uint8_t* buf, const int count, const int timeout_ms)
{
    struct timeval tv_timeout;
    fd_set fs_read;
    int ret = -1;
    if(sockfd < 0) return -1;

	tv_timeout.tv_sec  = timeout_ms  / 1000;
	tv_timeout.tv_usec = (timeout_ms % 1000) * 1000;
	FD_ZERO(&fs_read);
	FD_SET(sockfd, &fs_read);	//if fd == -1, FD_SET will block here
	ret = select((int)sockfd + 1, &fs_read, NULL, NULL, &tv_timeout);

    if(ret == 0) {return 0;} //rev overtime
    if(ret < 0) {return ret;} // select error
    ret = read(sockfd, (char*)buf, count);
    if(ret <= 0) {return  -1;}
    return  ret;
}

void CanThread::run()
{
	int ret = -1;
	struct can_frame fr;

	if (Open() < 0) m_bStart = false;

	while(1)
	{
		if(m_bStart)
		{
			if(System_Info.IO_To_BMS_Mode == COM_BMS_MODE_CAN)
			{
				ret = recv_frame(fd, (uint8_t*)&fr, sizeof(fr), 10);    //没有数据时会阻塞
				if(ret > 2)
				{
					//for(int i = 0; i < fr.can_dlc; i++)printf("%02x ", fr.data[i]);printf("\n");
					rLen=IO_Receive_Data_From_DSP_By_CAN(rBuf, (uint8_t*)&fr, MODBUS_BUF_LEN);
					if(rLen > 0)
					{
						mutex_for_can.lock();
						can_id = 0x01;
						//printf("rec:");for(int j=0; j<rLen; j++) printf(" %02x", rBuf[j]);printf("\r\n");
						IO_Process_Data_From_DSP_By_UART_Or_CAN(rBuf, rLen);
						mutex_for_can.unlock();
					}
				}
				usleep(10);
			}
			else
			{
				sleep(3);
			}
		}
		else
		{
			sleep(3);
		}
	}
}
