/*
 * can_thread.h
 *
 *  Created on: 2017-1-12
 *      Author: root
 */

#ifndef  _CAN_THREAD_H_
#define _CAN_THREAD_H_

#include "MThread.h"

#define REV_MODE 	0
#define SEND_MODE   1

class CanThread : public MThread
{
public:
	CanThread();
	virtual ~CanThread();

	int SetDevicePath(const char* path);
	int Open(void);
	int Get_FD(void);
	void run();
private:
	bool m_bStart;
	char Path[128];
	int fd, mode;
	int rLen, sLen;
	uint8_t rBuf[255],sBuf[255];

	int64_t  Time_cur, Time_last;	//当前时间，上次时间
	inline int CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur);
	int recv_frame(const int sockfd, uint8_t* buf, const int count, const int timeout_ms);
};


#endif /* _CAN1_THREAD_H_ */
