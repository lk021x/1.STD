/*
 * serial_send_thread.h
 *
 *  Created on: 2017-2-8
 *      Author: root
 */

#ifndef SERIAL_SEND_THREAD_H_
#define SERIAL_SEND_THREAD_H_

#include "MThread.h"
#define MAX_NUM		10

class SerailSendThread : public MThread
{
public:
	SerailSendThread();
	virtual ~SerailSendThread();

	void run();
private:
	bool m_bStart;
	int64_t Time_cur0, Time_last0;
    int64_t Time_cur1[5],Time_last1[5];
    int64_t Time_cur2[5],Time_last2[5];
    int64_t Time_cur3[5],Time_last3[5];
    int64_t Time_cur4[5],Time_last4[5];
    int64_t Time_cur5[5],Time_last5[5];
    int64_t Time_cur6[5],Time_last6[5];
    int64_t Time_cur7[5],Time_last7[5];

	int sLen;
	uint8_t sBuf[255];
	inline int CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur);
};


#endif /* SERIAL_SEND_THREAD_H_ */
