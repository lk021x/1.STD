/*
 * client_thread.h
 *
 *  Created on: 2017-1-12
 *      Author: root
 */

#ifndef _NET_REV_THREAD_H_
#define _NET_REV_THREAD_H_

#include "global.h"
#include "MThread.h"
#include "Connection.h"

class NetRevThread : public MThread
{
public:
	NetRevThread();
	virtual ~NetRevThread();

	int SetProtocol(CLIENT_PROTOCOL_TYPE type);
	int SetServer(char *IP, int port);
	bool ConnectStatus();
	Connection *GetConn();
	pthread_t GetID();
	int GetRunState();
	int SetRunState(bool st);
	pthread_mutex_t* GetMutex();
	pthread_cond_t* GetCond();
	inline int CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur);
private:
	bool m_bStart;
	char srvIP[32];
	int srvPort;
	char revbuf[256];
	int len;
	int over_timer;
	clinet_info *pInfo;
	Connection *conn;
	pthread_mutex_t *pMutex;
	pthread_cond_t *pCond;
private:
	int ConnectToServer();
	void run();
	void ParserData(CLIENT_PROTOCOL_TYPE type, char* buf, int len);
};


#endif /* _NET_REV_THREAD_H_ */
