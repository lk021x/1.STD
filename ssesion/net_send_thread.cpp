/*
 * send_thread.cpp
 *
 *  Created on: 2017-1-22
 *      Author: root
 */

#include "includes.h"
#include "net_send_thread.h"

NetSendThread::NetSendThread()
{
	m_bStart = true;
    Time_cur_1=0u;
    Time_last_1=0u;
    Time_cur_2=0u;
    Time_last_2=0u;
    Time_cur_3=0u;
    Time_last_3=0u;
    Time_cur_4=0u;
    Time_last_4=0u;
}

NetSendThread::~NetSendThread()
{

}

inline int NetSendThread::CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur)
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
extern void fm_test(void);
void NetSendThread::run()
{
	uint8_t i=0;
	uint8_t len=0;
	uint8_t buf[256]={0};
	static uint8_t sw = 0;
	static uint8_t sw2 = 0, add = 0;

	while(m_bStart)
	{
		if(System_Info.Config.PCS_Comm_Type==1)	//PCS使用网口模式
		{
			if(CheckTimeout(EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay, Time_cur_1, Time_last_1))   //需小于300，否则多台采集的数据过慢， 推荐200
			{

				if(Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode == FM)
				{
					fm_test();

					if(sw == 0)
					{
						sw = 1;
						for(i = 0; i< EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
						{
							if(ctrl.Client[i].ConnectStatus() == CLIENT_CONNECT)
							{
								mutex_for_timer.lock();
								len = EMS_Perpare_To_Send_Data_To_PCS_By_Net(buf, i);
								mutex_for_timer.unlock();
								if(len >= 6)
								{
									if(ctrl.Client[i].GetConn()->GetSocket())
									{
										//printf("I[%d], ID[%lu] send:",i, ctrl.Client[i].GetID());for(int j=0; j<len; j++) printf(" %02x", buf[j]);printf("\r\n");
										ctrl.Client[i].GetConn()->Send((char*)buf, len);
									}
								}
							}
						}
					}
					else
					{
						sw = 0;
						for(i = 0; i< EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
						{
							if(ctrl.Client[i].ConnectStatus() == CLIENT_CONNECT)
							{
								mutex_for_timer.lock();
								len = EMS_Perpare_To_Send_Para_To_PCS_By_Net(buf, i);
								mutex_for_timer.unlock();
								if(len >= 6)
								{
									if(ctrl.Client[i].GetConn()->GetSocket())
									{
										//printf("I[%d], ID[%lu] SEND:", i, ctrl.Client[i].GetID());for(int j=0; j<len; j++) printf(" %02x", buf[j]);printf("\r\n");
										ctrl.Client[i].GetConn()->Send((char*)buf, len);
									}
								}
							}
						}
					}
				}
				else
				{
					/*for(i = 0; i< EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
					{
						if(ctrl.Client[i].ConnectStatus() == CLIENT_CONNECT)
						{
							mutex_for_timer.lock();
							len = EMS_Perpare_To_Send_Data_To_PCS_By_Net_Not_FM(buf, i);
							mutex_for_timer.unlock();
							if(len >= 6)
							{
								if(ctrl.Client[i].GetConn()->GetSocket())
								{
									//printf("I[%d], ID[%lu] SEND:", i, ctrl.Client[i].GetID());for(int j=0; j<len; j++) printf(" %02x", buf[j]);printf("\r\n");
									ctrl.Client[i].GetConn()->Send((char*)buf, len);
								}
							}
						}
					}*/

					if(sw2 == 0)
					{
						sw2 = 1;
						for(i = 0; i< EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
						{
							if(ctrl.Client[i].ConnectStatus() == CLIENT_CONNECT)
							{
								mutex_for_timer.lock();
								len = EMS_Perpare_To_Send_Data_To_PCS_By_Net(buf, i);
								mutex_for_timer.unlock();
								if(len >= 6)
								{
									if(ctrl.Client[i].GetConn()->GetSocket())
									{
										//printf("I[%d], ID[%lu] send:",i, ctrl.Client[i].GetID());for(int j=0; j<len; j++) printf(" %02x", buf[j]);printf("\r\n");
										ctrl.Client[i].GetConn()->Send((char*)buf, len);
									}
								}
							}
						}
					}
					else
					{
						sw2 = 0;
						add++;
						if(add >= EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle)
						{
							add = 0;
							for(i = 0; i< EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
							{
								if(ctrl.Client[i].ConnectStatus() == CLIENT_CONNECT)
								{
									mutex_for_timer.lock();
									len = EMS_Perpare_To_Send_Para_To_PCS_By_Net(buf, i);
									mutex_for_timer.unlock();
									if(len >= 6)
									{
										if(ctrl.Client[i].GetConn()->GetSocket())
										{
											//printf("I[%d], ID[%lu] SEND:", i, ctrl.Client[i].GetID());for(int j=0; j<len; j++) printf(" %02x", buf[j]);printf("\r\n");
											ctrl.Client[i].GetConn()->Send((char*)buf, len);
										}
									}
								}
							}
						}
					}
				}
			}
		}

		//PV网口通信
		if(System_Info.Config.PV_Comm_Type==1 && System_Info.Config.Bus_Type == 1)	//PV使用网口模式
		{
			if(CheckTimeout(300, Time_cur_3, Time_last_3))
			{
				for(i = 0; i< EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
				{
					if(ctrl.PvClient[i].ConnectStatus() == CLIENT_CONNECT)
					{
						mutex_for_timer.lock();
						len = EMS_Perpare_To_Send_Data_To_PV_By_Net(buf, i);
						mutex_for_timer.unlock();
						if(len >= 6)
						{
							if(ctrl.PvClient[i].ConnectStatus() == CLIENT_CONNECT && ctrl.PvClient[i].GetConn()->GetSocket())
							{
								//printf("%lu send:",ctrl.Client[i].GetID());for(int j=0; j<len; j++) printf(" %02x", buf[j]);printf("\r\n");
								ctrl.PvClient[i].GetConn()->Send((char*)buf, len);
							}
						}
					}
				}
			}

			/*if(CheckTimeout(1500, Time_cur_4, Time_last_4))
			{
				for(i = 0; i< EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
				{
					if(ctrl.PvClient[i].ConnectStatus() == CLIENT_CONNECT)
					{
						mutex_for_timer.lock();
						len = EMS_Perpare_To_Send_Para_To_PV_By_Net(buf, i);
						mutex_for_timer.unlock();
						if(len >= 6)
						{
							if(ctrl.PvClient[i].GetConn()->GetSocket()){
								//printf("%lu SEND:",ctrl.Client[i].GetID());for(int j=0; j<len; j++) printf(" %02x", buf[j]);printf("\r\n");
								ctrl.PvClient[i].GetConn()->Send((char*)buf, len);
							}
						}
					}
				}
			}*/
		}

		usleep(2);
	}
}
