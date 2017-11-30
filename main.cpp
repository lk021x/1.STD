//============================================================================
// Name        : main.cpp
// Author      :
// Version     :
// Copyright   :
// Description :
// minicom工具调试: 打开方式 minicom -D /dev/ttyS0 -b 115200 -8 -c on -H
// 串口设置: minicom -s , 一定要把硬件流关闭，否则无法使用串口发送 (打开串口后，使用echo "hello">/dev/ttyS0测试发送)
//============================================================================
//关于多线程定时，程序中用到定时精度达到20ms，就需要让每个线程的保持时间低于20ms，特别的在网口接收过中数据接收超时需小于20ms，推荐10ms（255B）。

#include "includes.h"

//调试前使用下面的命令挂载本地NFS目录
//mount -t nfs 192.168.1.128:/nfs_root /mnt/nfs_root/ -o nolock
//调试使用 -O0优先级， 正常使用时设置到-O1模式

Thread_Ctrl ctrl;
NetEventThread net_event_srv;
CCriticalSection mutex_for_iap;
CCriticalSection mutex_for_web;
CCriticalSection mutex_for_timer;
CCriticalSection mutex_for_can;
CCriticalSection mutex_for_adc;
CCriticalSection mutex_for_iec104;
CCriticalSection mutex_for_client;

volatile t_gparam gt_glp;

int main(int argc, char** argv)
{
	init_eeprom();
	get_sys_time();
    Global_Variable_Init();
    (void)log_init();
	aes_init();
	Read_History_Data();
	Read_Web_History_Data();
    Next_Day_Data_Process();//判断是否到另一天，一些数据要清0
    Write_HisResetRecord_Init();
    Write_HisResetRecord_Process();
	Write_HisEventRecord_Init();
    Write_HisFaultRecord_Init();
    Write_HisAlarmRecord_Init();

	uint32_t temp = CAN_BAUD_250Kbps;  //DSP -- 250Kbps, 修改接收超时为10ms
	drv_ioctl(0, CTRL_CAN | CAN_SET_BAUD, &temp);
    (void)iapv2_init();
    //Create_Process_ID();

	//启动线程， 串口优先级推荐 SCHED_RR， 保证数据公平处理，均等时间片
	ctrl.serialRev_thread[0].Set_Serial_Path(TTYO1);
	ctrl.serialRev_thread[0].SetComParam(comParam[0]);
	ctrl.serialRev_thread[0].setThreadPriority(SCHED_RR);
	ctrl.serialRev_thread[0].setThreadPriorityParam(5);
	ctrl.serialRev_thread[0].start();
	ctrl.serialRev_thread[1].Set_Serial_Path(TTYO2);
	ctrl.serialRev_thread[1].SetComParam(comParam[1]);
	ctrl.serialRev_thread[1].setThreadPriority(SCHED_RR);
	ctrl.serialRev_thread[1].setThreadPriorityParam(5);
	ctrl.serialRev_thread[1].start();
	ctrl.serialRev_thread[2].Set_Serial_Path(TTYO3);
	ctrl.serialRev_thread[2].SetComParam(comParam[2]);
	ctrl.serialRev_thread[2].setThreadPriority(SCHED_RR);
	ctrl.serialRev_thread[2].setThreadPriorityParam(5);
	ctrl.serialRev_thread[2].start();
	ctrl.serialRev_thread[3].Set_Serial_Path(TTYO4);
	ctrl.serialRev_thread[3].SetComParam(comParam[3]);
	ctrl.serialRev_thread[3].setThreadPriority(SCHED_RR);
	ctrl.serialRev_thread[3].setThreadPriorityParam(5);
	ctrl.serialRev_thread[3].start();
	ctrl.serialRev_thread[4].Set_Serial_Path(TTYO5);
	ctrl.serialRev_thread[4].SetComParam(comParam[4]);
	ctrl.serialRev_thread[4].setThreadPriority(SCHED_RR);
	ctrl.serialRev_thread[4].setThreadPriorityParam(5);
	ctrl.serialRev_thread[4].start();
	ctrl.serialRev_thread[5].Set_Serial_Path(TTYO6);
	ctrl.serialRev_thread[5].SetComParam(comParam[5]);  //固定给屏或monitor使用
	ctrl.serialRev_thread[5].setThreadPriority(SCHED_RR);
	ctrl.serialRev_thread[5].setThreadPriorityParam(5);
	ctrl.serialRev_thread[5].start();
	if(System_Info.Config.PCS_Comm_Type== PCS_COMM_SERIAL){
		ctrl.serialSend_thread.setThreadPriority(SCHED_FIFO);
		ctrl.serialSend_thread.setThreadPriorityParam(99);
	}else{
		ctrl.serialSend_thread.setThreadPriority(SCHED_RR);
		ctrl.serialSend_thread.setThreadPriorityParam(7);
	}
	ctrl.serialSend_thread.start();
	sleep(1);

	//默认优先级
    ctrl.canRev_thread.SetDevicePath((char*)CAN0);
    ctrl.canRev_thread.start();
	ctrl.adc.SetDevicePath((char*)ADC1);
	ctrl.adc.start();

	char ipStr[16]={0};
	memset(ipStr, 0x00, 16);
	sprintf(ipStr,"%d.%d.%d.%d", System_Info.NTP.ip[0],System_Info.NTP.ip[1],System_Info.NTP.ip[2],System_Info.NTP.ip[3]);
    ctrl.ntp.SetServer(ipStr, System_Info.NTP.port);
	ctrl.ntp.start();
	sleep(1);

	for(int i=0; i<PCS_NUM; i++)
	{
		memset(ipStr, 0x00, 16);
		sprintf(ipStr,"%d.%d.%d.%d", System_Info.Server[i].ip[0],System_Info.Server[i].ip[1],System_Info.Server[i].ip[2],System_Info.Server[i].ip[3]);
		ctrl.Client[i].SetServer(ipStr, System_Info.Server_Modbus_Port[i]);
		ctrl.Client[i].SetProtocol(CLIENT_PROTOCOL_PCS_TCPMODBUS);
		ctrl.Client[i].setThreadPriority(SCHED_FIFO);   //使用SCHED_FIFO, 一方面数量多时系统资源利用率高，一方面为了满足同步需要  ***** PCS读 -> PCS数据处理 -> 数据发送 -> PV读(低优先级) -> PCS读 ... *****
		ctrl.Client[i].setThreadPriorityParam(99);  //最高实时性
	}
	for(int i=0; i<PCS_NUM; i++) ctrl.Client[i].start();  //修改接收超时为10ms
	sleep(1);

	ctrl.timer.setThreadPriority(SCHED_FIFO);   //数据处理  ***** PCS读 -> PCS数据处理 -> 数据发送 -> PV读(低优先级) -> PCS读 ... *****
	ctrl.timer.setThreadPriorityParam(99);  //最高实时性
	ctrl.timer.start();

	ctrl.netClientSend_thread.setThreadPriority(SCHED_FIFO);  //数据发送  ***** PCS读 -> PCS数据处理 -> 数据发送 -> PV读(低优先级) -> PCS读 ... *****
	ctrl.netClientSend_thread.setThreadPriorityParam(99);   //最高实时性
	ctrl.netClientSend_thread.start();
	sleep(1);

	for(int i=0; i<PV_NUM; i++)
	{
		memset(ipStr, 0x00, 16);
		sprintf(ipStr,"%d.%d.%d.%d", System_Info.PVServer[i].ip[0],System_Info.PVServer[i].ip[1],System_Info.PVServer[i].ip[2],System_Info.PVServer[i].ip[3]);
		ctrl.PvClient[i].SetServer(ipStr, System_Info.PVServer_Modbus_Port[i]);
		ctrl.PvClient[i].SetProtocol(CLIENT_PROTOCOL_PV_TCPMODBUS);
		ctrl.PvClient[i].setThreadPriority(SCHED_FIFO);   //推荐使用SCHED_FIFO，数量过多时使用SCHED_RR会导致系统资源紧张.
		ctrl.PvClient[i].setThreadPriorityParam(9);  //一般实时性， 队列在发送之后  ***** PCS读 -> PCS数据处理 -> 数据发送 -> PV读(低优先级) -> PCS读 ... *****
	}
	for(int i=0; i<PV_NUM; i++) ctrl.PvClient[i].start();  //修改接收超时为10ms
	sleep(1);

	ctrl.netServer.start();
	ctrl.manage.start();
	sleep(1);
	net_event_srv.start();

  	//等待线程退出
	for(int i=0; i<6; i++)ctrl.serialRev_thread[i].wait();
  	ctrl.serialSend_thread.wait();
  	ctrl.canRev_thread.wait();
  	ctrl.adc.wait();
  	ctrl.ntp.wait();
	for(int i=0; i<PCS_NUM; i++){ctrl.Client[i].wait();}
  	ctrl.timer.wait();
  	ctrl.netClientSend_thread.wait();
	for(int i=0; i<PV_NUM; i++){ctrl.PvClient[i].wait();}
  	ctrl.netServer.wait();
	ctrl.manage.wait();
  	net_event_srv.wait();
	return 0;
}
