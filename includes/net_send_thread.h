/*
 * send_thread.h
 *
 *  Created on: 2017-1-22
 *      Author: root
 */

#ifndef _NET_SEND_THREAD_H_
#define _NET_SEND_THREAD_H_

#include "MThread.h"
#define MAX_BUFFER_NUM		32
typedef struct{
	uint8_t buf[256];
	uint8_t len;
}BufferTag;

class NetSendThread : public MThread
{
public:
	NetSendThread();
	virtual ~NetSendThread();

private:
	bool m_bStart;
    int64_t Time_cur_1,Time_last_1;
    int64_t Time_cur_2,Time_last_2;
    int64_t Time_cur_3,Time_last_3;
    int64_t Time_cur_4,Time_last_4;

    inline int CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur);
	void run();
};


#endif /* _NET_SEND_THREAD_H_ */
