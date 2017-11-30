/*
 * client_thread.h
 *
 *  Created on: 2017-1-12
 *      Author: root
 */

#ifndef _NET_MANAGE_THREAD_H_
#define _NET_MANAGE_THREAD_H_

#include "global.h"
#include "MThread.h"
#include "Connection.h"

class NetManageThread : public MThread
{
public:
	NetManageThread();
	virtual ~NetManageThread();

private:
	void run();
};


#endif /* _NET_MANAGE_THREAD_H_ */
