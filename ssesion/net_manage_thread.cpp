/*
 * send_thread.cpp
 *
 *  Created on: 2017-1-22
 *      Author: root
 */

#include "includes.h"
#include "net_manage_thread.h"

NetManageThread::NetManageThread()
{

}

NetManageThread::~NetManageThread()
{

}

void NetManageThread::run()
{
	uint8_t i=0;

	while(1)
	{
		if(System_Info.Config.PCS_Comm_Type == 1)
		{
			for(i=0;i<PCS_NUM;i++)
			{
				if(i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number) //唤醒
				{
					if(!ctrl.Client[i].GetRunState())
					{
						pthread_mutex_lock(ctrl.Client[i].GetMutex());
						ctrl.Client[i].SetRunState(true);
						pthread_cond_signal(ctrl.Client[i].GetCond());  //发送条件变量唤醒线程
						//printf("PCS net pthread %d run!\n", i);
						pthread_mutex_unlock(ctrl.Client[i].GetMutex());
					}
				}
				else  //暂停
				{
					if(ctrl.Client[i].GetRunState())
					{
						pthread_mutex_lock(ctrl.Client[i].GetMutex());
						ctrl.Client[i].SetRunState(false);
						//printf("PCS net pthread %d stop!\n", i);
						pthread_mutex_unlock(ctrl.Client[i].GetMutex());
					}
				}
			}
		}
		else
		{
			for(i=0;i<PCS_NUM;i++)
			{
				if(ctrl.Client[i].GetRunState())
				{
					pthread_mutex_lock(ctrl.Client[i].GetMutex());
					ctrl.Client[i].SetRunState(false);
					//printf("PCS net pthread %d stop!\n", i);
					pthread_mutex_unlock(ctrl.Client[i].GetMutex());
				}
			}
		}

		if(System_Info.Config.PV_Comm_Type==1 && System_Info.Config.Bus_Type == 1)
		{
			for(i=0;i<PV_NUM;i++)
			{
				if(i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number) //唤醒
				{
					if(!ctrl.PvClient[i].GetRunState())
					{
						pthread_mutex_lock(ctrl.PvClient[i].GetMutex());
						ctrl.PvClient[i].SetRunState(true);
						pthread_cond_signal(ctrl.PvClient[i].GetCond());  //发送条件变量唤醒线程
						//printf("PV net pthread %d run!\n", i);
						pthread_mutex_unlock(ctrl.PvClient[i].GetMutex());
					}
				}
				else  //暂停
				{
					if(ctrl.PvClient[i].GetRunState())
					{
						pthread_mutex_lock(ctrl.PvClient[i].GetMutex());
						ctrl.PvClient[i].SetRunState(false);
						//printf("PV net pthread %d stop!\n", i);
						pthread_mutex_unlock(ctrl.PvClient[i].GetMutex());
					}
				}
			}
		}
		else
		{
			for(i=0;i<PV_NUM;i++)
			{
				if(ctrl.PvClient[i].GetRunState())
				{
					pthread_mutex_lock(ctrl.PvClient[i].GetMutex());
					ctrl.PvClient[i].SetRunState(false);
					//printf("PV net pthread %d stop!\n", i);
					pthread_mutex_unlock(ctrl.PvClient[i].GetMutex());
				}
			}
		}

		sleep(1);
	}
}
