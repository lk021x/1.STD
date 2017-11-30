/*
 * serial_thread.h
 *
 *  Created on: 2017-1-12
 *      Author: root
 */

#ifndef SERIAL_REV_THREAD_H_
#define SERIAL_REV_THREAD_H_

#include "includes.h"
#include "MThread.h"

class SerialRevThread : public MThread
{
public:
	SerialRevThread();
	virtual ~SerialRevThread();

	int SetComParam(COM_PARAM param);
	int GetProtocol();
	int GetBeginID();
	int GetEndID();
	int Set_Serial_Path(const char* path);
	int Set_Baud(uint32_t baud);
	int Get_FD();
	void run();
private:
	bool m_bStart;
	char Path[128];
	int fd,ret;
	int rLen, sLen;

	uint16_t pos;
	uint8_t rBuf[255],sBuf[255];
	int64_t  Time_cur, Time_last;	//当前时间，上次时间
	SERIAL_PROTOCOL_TYPE protocol_type; //协议类型
	uint8_t st_Id, en_Id;
	uint16_t m_baud;
	uint16_t vmin;

	inline int CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur);

	int Open(void);
};


#endif /* _SERIAL_THREAD_H_ */
