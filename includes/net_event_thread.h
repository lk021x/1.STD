/*
 * net_event_thread.h
 *
 *  Created on: 2017-1-21
 *      Author: root
 */

#ifndef NET_EVENT_THREAD_H_
#define NET_EVENT_THREAD_H_

#include "includes.h"
#include "MThread.h"

class NetEventThread: public MThread
{
public:
	NetEventThread();
	virtual ~NetEventThread();
	void stop_web();
	void run();
private:
	bool m_bStart;
};

extern uint16_t failed_accept_times;
int server_close(void);
#endif /* NET_EVENT_THREAD_H_ */
