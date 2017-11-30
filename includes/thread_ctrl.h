/*
 * thread_ctrl.h
 *
 *  Created on: 2017-1-11
 *      Author: root
 */

#ifndef _THRREAD_CTRL_H
#define _THRREAD_CTRL_H

#include "property.h"
#include "property_interface.h"
#include "iec104exe.h"
#include "iec104rtu.h"
#include "log.h"
#include "log_interface.h"
#include "http_interface.h"
#include "Connection.h"
#include "common_data.h"
#include "serial_rev_thread.h"
#include "serial_send_thread.h"
#include "can_thread.h"
#include "net_rev_thread.h"
#include "net_send_thread.h"
#include "net_manage_thread.h"
#include "timer_thread.h"
#include "http_server.h"
#include "adc_thread.h"
#include "net_ntp_thread.h"
#include "net_server_thread.h"

class Thread_Ctrl
{
public:
	Thread_Ctrl();
	~Thread_Ctrl();

	SerialRevThread serialRev_thread[6];	//485接收线程
	SerailSendThread serialSend_thread;		//串口发送线程
	CanThread canRev_thread;				//CAN0接收线程
	NetRevThread Client[PCS_NUM];           //PCS客户端线程
	NetRevThread PvClient[PV_NUM];			//PV客户端线程
	TimerThread timer;						//定时器线程
	AdcThread adc;							//ADC线程
	NetSendThread  netClientSend_thread;    //TCP客户端发送线程
	NetManageThread manage;
	NetNtpThread ntp;
	NetServerThread netServer;
};
extern Thread_Ctrl ctrl;
#endif /* _THRREAD_CTRL_H_ */
