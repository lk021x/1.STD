/*
 * serial_send_thread.cpp
 *
 *  Created on: 2017-2-8
 *      Author: root
 */

#include "includes.h"
#include "serial_send_thread.h"

SerailSendThread::SerailSendThread()
{
	m_bStart = true;
	Time_cur0 = 0u;
	Time_last0 = 0u;
	for(int i=0;i<5;i++)
	{
	    Time_cur1[i]=0u;
	    Time_last1[i]=0u;
	    Time_cur2[i]=0u;
	    Time_last2[i]=0u;
	    Time_cur3[i]=0u;
	    Time_last3[i]=0u;
	    Time_cur4[i]=0u;
	    Time_last4[i]=0u;
	    Time_cur5[i]=0u;
	    Time_last5[i]=0u;
	    Time_cur6[i]=0u;
	    Time_last6[i]=0u;
	    Time_cur7[i]=0u;
	    Time_last7[i]=0u;
	}
    sLen=0;
}

SerailSendThread::~SerailSendThread()
{

}

inline int SerailSendThread::CheckTimeout(uint16_t ms, int64_t &last, int64_t &cur)
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

void SerailSendThread::run()
{
	uint8_t i = 0;

	while(m_bStart)
	{
		//CAN发送
		/*if(CheckTimeout(18, Time_cur0, Time_last0))
		{
			mutex_for_can.lock();
		    sLen=IO_Perpare_To_Send_Data_To_DSP(sBuf);
		    if(sLen>0)
		    {
		    	if(ctrl.canRev_thread.Get_FD()>0)
		    		IO_Send_Data_To_DSP_By_CAN(ctrl.canRev_thread.Get_FD(), sBuf,(((sLen-1)/8)+1)*8);//发送8字节的整数倍
		    }
		    mutex_for_can.unlock();
		}*/

		//判断串口1~5通讯协议，处理发送请求，串口6保留作为屏接口
		for(i = 0; i < 5; i++)
		{
			switch(ctrl.serialRev_thread[i].GetProtocol())
			{
				case SERIAL_PROTOCOL_MODBUS_PCS:   //PCS
					if(System_Info.Config.PCS_Comm_Type==0)	//PCS使用串口模式
					{
						if(CheckTimeout(310, Time_cur1[i], Time_last1[i]))   //推荐400, >=355(vmin=1 LEN=255)
						{
							mutex_for_timer.lock();
							sLen = EMS_Perpare_To_Send_Data_To_PCS_By_UART(i, sBuf,
									ctrl.serialRev_thread[i].GetBeginID(), ctrl.serialRev_thread[i].GetEndID());
							mutex_for_timer.unlock();
							if(sLen > 0)
							{
								//printf("send:");for(int j=0; j<sLen; j++) printf(" %02x", sBuf[j]);printf("\r\n");
								if(ctrl.serialRev_thread[i].Get_FD()>0)
									drv_write(ctrl.serialRev_thread[i].Get_FD(), sBuf, sLen);
							}
						}
					}
				break;
				case SERIAL_PROTOCOL_MODBUS_PV:	//PV
					if(DC_BUS_SYS == 0) //交流母线
					{
						if(CheckTimeout(310, Time_cur2[i], Time_last2[i]))	//推荐400, >=355(vmin=1 LEN=255)
						{
							mutex_for_timer.lock();
							sLen = EMS_Perpare_To_Send_Data_To_PV_By_UART(i, sBuf,
									ctrl.serialRev_thread[i].GetBeginID(), ctrl.serialRev_thread[i].GetEndID());
							mutex_for_timer.unlock();
							if(sLen > 0)
							{
								//printf("send:");for(int j=0; j<sLen; j++) printf(" %02x", sBuf[j]);printf("\r\n");
								if(ctrl.serialRev_thread[i].Get_FD()>0)
									drv_write(ctrl.serialRev_thread[i].Get_FD(), sBuf, sLen);
							}
						}
					}
				break;
				case SERIAL_PROTOCOL_MODBUS_DCDC:	//DCDC
					if (DC_BUS_SYS == 1) //直流母线
					{
						if(System_Info.Config.DCDC_Type != 0)
						{
							if(CheckTimeout(310, Time_cur3[i], Time_last3[i]))	//推荐400, >=355(vmin=1 LEN=255)
							{
								mutex_for_timer.lock();
								sLen = EMS_Perpare_To_Send_Data_To_DCDC(i, sBuf,
										ctrl.serialRev_thread[i].GetBeginID(), ctrl.serialRev_thread[i].GetEndID());
								mutex_for_timer.unlock();
								if(sLen > 0)
								{
									//printf("send:");for(int j=0; j<sLen; j++) printf(" %02x", sBuf[j]);printf("\r\n");
									if(ctrl.serialRev_thread[i].Get_FD()>0)
										drv_write(ctrl.serialRev_thread[i].Get_FD(), sBuf, sLen);

								}
							}
						}
					}
				break;
				case SERIAL_PROTOCOL_MODBUS_BMS:	//BMS
					if(System_Info.Config.BMS_Type != 0)
					{
						if(CheckTimeout(310, Time_cur4[i], Time_last4[i]))	//推荐400, >=355(vmin=1 LEN=255)
						{
							mutex_for_timer.lock();
							sLen = EMS_Perpare_To_Send_Data_To_BMS(i, sBuf,
									ctrl.serialRev_thread[i].GetBeginID(), ctrl.serialRev_thread[i].GetEndID());
							mutex_for_timer.unlock();
							if(sLen > 0)
							{
								//printf("send:");for(int j=0; j<sLen; j++) printf(" %02x", sBuf[j]);printf("\r\n");
								if(ctrl.serialRev_thread[i].Get_FD()>0)
									drv_write(ctrl.serialRev_thread[i].Get_FD(), sBuf, sLen);
							}
						}
					}
				break;
				case SERIAL_PROTOCOL_AMMETER:	//AMMETER
					//if(System_Info.Config.Meter_Comm_Enable == 0xAA)
					if(1)
					{
						if(CheckTimeout(310, Time_cur5[i], Time_last5[i]))	//推荐400, >=355(vmin=1 LEN=255)
						{
							if(System_Info.Config.Ammeter_Protocol_Type == 0)	//MODBUS电表协议
							{
								mutex_for_timer.lock();
								sLen = EMS_Perpare_To_Send_Data_To_Ammeter_For_Modbus(i, sBuf,
									ctrl.serialRev_thread[i].GetBeginID(), ctrl.serialRev_thread[i].GetEndID());
								mutex_for_timer.unlock();
							}
							else	//DL/T645标准电表协议
							{
								sLen = EMS_Perpare_To_Send_Data_To_Ammeter_For_DLT645(i, sBuf,
									ctrl.serialRev_thread[i].GetBeginID(), ctrl.serialRev_thread[i].GetEndID());
							}
							if(sLen > 0)
							{
								//printf("send:");for(int j=0; j<sLen; j++) printf(" %02x", sBuf[j]);printf("\r\n");
								if(ctrl.serialRev_thread[i].Get_FD()>0)
									drv_write(ctrl.serialRev_thread[i].Get_FD(), sBuf, sLen);
							}
						}
					}
				break;
				case SERIAL_PROTOCOL_MODBUS_PVS:	//PVS
					if (webPageShow.pvs == 1)
					{
						if(CheckTimeout(310, Time_cur6[i], Time_last6[i]))	//推荐400, >=355(vmin=1 LEN=255)
						{
							mutex_for_timer.lock();
							sLen = EMS_Perpare_To_Send_Data_To_PVS(i, sBuf,
									ctrl.serialRev_thread[i].GetBeginID(), ctrl.serialRev_thread[i].GetEndID());
							mutex_for_timer.unlock();
							if(sLen > 0)
							{
								//printf("send:");for(int j=0; j<sLen; j++) printf(" %02x", sBuf[j]);printf("\r\n");
								if(ctrl.serialRev_thread[i].Get_FD()>0)
									drv_write(ctrl.serialRev_thread[i].Get_FD(), sBuf, sLen);
							}
						}
					}
				break;
				case SERIAL_PROTOCOL_MODBUS_ENV:	//环境检测仪
					if (webPageShow.env == 1)
					{
						if(CheckTimeout(310, Time_cur7[i], Time_last7[i]))	//推荐400, >=355(vmin=1 LEN=255)
						{
							mutex_for_timer.lock();
							sLen = EMS_Perpare_To_Send_Data_To_ENV(i, sBuf,
									ctrl.serialRev_thread[i].GetBeginID(), ctrl.serialRev_thread[i].GetEndID());
							mutex_for_timer.unlock();
							if(sLen > 0)
							{
								//printf("send:");for(int j=0; j<sLen; j++) printf(" %02x", sBuf[j]);printf("\r\n");
								if(ctrl.serialRev_thread[i].Get_FD()>0)
									drv_write(ctrl.serialRev_thread[i].Get_FD(), sBuf, sLen);
							}
						}
					}
				break;
				default:
					//sleep(2);  //不能加长延时
				break;
			}
		}
		usleep(2);
	}
}

