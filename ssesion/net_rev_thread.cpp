/*
 * client_thread.cpp
 *
 *  Created on: 2017-1-12
 *      Author: root
 */

#include "includes.h"
#include "net_rev_thread.h"

#define DELAY_OVER_TIME   200

NetRevThread::NetRevThread()
{
	pMutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);  //互斥锁
	pthread_mutex_init(pMutex, &attr);
	pthread_mutexattr_destroy(&attr);

	pCond = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(pCond, NULL);

	m_bStart = false;
	srvPort = 502;
	memset(revbuf, 0x00, 256);
	len = 0;
	over_timer = 0;
	pInfo = NULL;
	pInfo = (clinet_info*) malloc(sizeof(clinet_info));
	pInfo->type = CLIENT_PROTOCOL_PCS_TCPMODBUS;
	conn = new Connection();
}

NetRevThread::~NetRevThread()
{
	pthread_mutex_destroy(pMutex);
	if(pMutex!=NULL)free(pMutex);
	pMutex = NULL;
	if(pCond!=NULL)free(pCond);
	pCond=NULL;
	free(pInfo);
	delete conn;
}

int NetRevThread::SetServer(char *IP, int port)
{
	memset(srvIP, 0x00, sizeof(srvIP));
	memcpy(srvIP, IP, strlen(IP));
	pInfo->state = CLIENT_DISCONNECT;
	srvPort = port;
	conn->DisConnect();
	conn->SetServerIP(IP);
	conn->SetServerPort(port);
	return 0;
}

int NetRevThread::SetProtocol(CLIENT_PROTOCOL_TYPE type)
{
	pInfo->type = type;
	return 0;
}

bool NetRevThread::ConnectStatus()
{
	return pInfo->state;
}

Connection *NetRevThread::NetRevThread::GetConn()
{
	return conn;
}

pthread_t NetRevThread::GetID()
{
	return (getThreadID());
}

int NetRevThread::GetRunState()
{
	return m_bStart;
}

pthread_mutex_t* NetRevThread::GetMutex()
{
	return pMutex;
}

pthread_cond_t* NetRevThread::GetCond()
{
	return pCond;
}

int NetRevThread::SetRunState(bool st)
{
	m_bStart = st;
	return 0;
}

int NetRevThread::ConnectToServer()
{
	int ret = 0;

	if (pInfo->state == CLIENT_DISCONNECT)
	{
		ret = conn->Connect();
		if (ret < 0)
		{
			conn->DisConnect();
			pInfo->client = NULL;
			pInfo->state = CLIENT_DISCONNECT;
			//printf("ClientThread::id %lu, try after 3 sec IP: (%s) Port(%d)\n", getThreadID(), srvIP, srvPort);
			//sleep(3);
		}
		else
		{
			pInfo->state = CLIENT_CONNECT;
			pInfo->client = conn;
			printf("ClientThread::id %lu, Connect IP: (%s) Port(%d) Success!\n", getThreadID(), srvIP, srvPort);
		}
	}

	return SUCCESS;
}

inline int NetRevThread::CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur)
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
}

void NetRevThread::run()
{
	while(1)
	{
		pthread_mutex_lock(pMutex);
		while(!m_bStart)
		{
			over_timer = 0;
			conn->DisConnect();
			pInfo->client = NULL;
			pInfo->state = CLIENT_DISCONNECT;
			//printf("Client ID %lu Sleep\n", getThreadID());
			pthread_cond_wait(pCond, pMutex);
		}
		pthread_mutex_unlock(pMutex);

		//////////////////////////////////////////////////////////
		if (pInfo->state == CLIENT_CONNECT)
		{
			len = pInfo->client->Receive(revbuf, 256);  // 10ms超时， PCS读[最大50台]->处理->下发
			//if(len>0)printf("ID[%lu], rec:%d, [%2x %2x %2x %2x %2x %2x]\n", getThreadID(),len,revbuf[6],revbuf[7],revbuf[8],revbuf[9],revbuf[10],revbuf[11]);
			if (len > 0){
				//if(pInfo->client->CheckConnectStatus())pInfo->client->Send(revbuf, len); //测试
				//printf("net rec:");for(int j=0; j<len; j++) printf(" %02x", revbuf[j]);printf("\r\n");
				over_timer = 0;
				ParserData(pInfo->type, revbuf, len);
				usleep(10);
			}else{
				if(len != -128)
				{
					over_timer = 0;
					printf("ID[%lu], IP %s: %d Disconnect!\n", getThreadID(), srvIP, srvPort);
					conn->DisConnect();
					pInfo->state = CLIENT_DISCONNECT;
					pInfo->client = NULL;
				}
				else
				{
					over_timer++;
					if(over_timer >= 500) //10ms x 500
					{
						over_timer = 0;
						printf("ID[%lu], IP %s: %d Disconnect!\n", getThreadID(), srvIP, srvPort);
						conn->DisConnect();
						pInfo->state = CLIENT_DISCONNECT;
						pInfo->client = NULL;
					}
				}
			}
		}
		else
		{
			ConnectToServer();
			sleep(3);
		}
	}
}

void NetRevThread::ParserData(CLIENT_PROTOCOL_TYPE type, char* buf, int len)
{
	switch (type)
	{
	case CLIENT_PROTOCOL_PCS_TCPMODBUS:
		mutex_for_timer.lock();
		EMS_Process_Data_From_PCS_By_Net((uint8_t*) buf, len);
		mutex_for_timer.unlock();
		break;
	case CLIENT_PROTOCOL_IEC104:
		mutex_for_timer.lock();
		mutex_for_timer.unlock();
		break;
	case CLIENT_PROTOCOL_PV_TCPMODBUS:
		mutex_for_timer.lock();
		EMS_Process_Data_From_PV_By_Net((uint8_t*) buf, len);
		mutex_for_timer.unlock();
		break;
	default:
		break;
	}
}

