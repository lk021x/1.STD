#include "includes.h"

//检测序列号是否格式有误
bool Check_Sn(char buf[], uint8_t len)
{
	bool bool_val = false;
	for (uint8_t i = 0; i < len; i++)
	{
		if ((buf[i] >= '0' && buf[i] <= '9') || (buf[i] >= 'A' && buf[i] <= 'Z') || (buf[i] >= 'a' && buf[i] <= 'z'))
		{
			bool_val = true;
		}
		else
		{
			bool_val = false;
			break;
		}
	}
	return bool_val;
}
//检测序列号的有效位数
uint8_t Sn_Num(char* sn, uint8_t num)
{
	uint8_t i = 0;
	for (i = 0; i < num; i++)
	{
		if (sn[i] == 0)
		{
			break;
		}
	}
	return i;
}
//获取当前系统时间
int get_sys_time(void)
{
	time_t cur_t;
	struct tm* cur_tm;

	time(&cur_t);
	cur_tm = localtime(&cur_t);
	showTime.tm_year = cur_tm->tm_year + 1900 - 2000;
	showTime.tm_mon = cur_tm->tm_mon + 1;
	showTime.tm_day = cur_tm->tm_mday;
	showTime.tm_hour = cur_tm->tm_hour;
	showTime.tm_min = cur_tm->tm_min;
	showTime.tm_sec = cur_tm->tm_sec;
	//printf("%d-%d-%d %d:%d:%d\n",
	//		showTime.tm_year,showTime.tm_mon,showTime.tm_day,showTime.tm_hour,showTime.tm_min,showTime.tm_sec);
	return 0;
}
//设置当前系统时间
int set_sys_time(void)
{
	struct tm tm;
	struct timeval tv;
	time_t timep;
	int ret = -1;

	//#cat /etc/time_utc.sh
	//#!/bin/sh
	//hwclock -u -s
	//export TZ=UTC-08:00
	tm.tm_year = Touch_Write_Time[0]+100;
	tm.tm_mon = Touch_Write_Time[1]-1;
	tm.tm_mday = Touch_Write_Time[2];
	tm.tm_hour = Touch_Write_Time[3];
	tm.tm_min = Touch_Write_Time[4];
	tm.tm_sec = Touch_Write_Time[5];

	timep = mktime(&tm);
	tv.tv_sec = timep;
	tv.tv_usec = 100000;
	ret = settimeofday(&tv, (struct timezone *) 0);
	if(ret < 0)
	{
		return -1;
	}
	else
	{
		FILE * fp;
		if ((fp = popen("hwclock -w -u", "w") ) == NULL){   //写入硬件
			//printf("POPEN error: %s\n", strerror(errno));
			return -1;
		}else{
			pclose(fp);
		}
	}
	return 0;
}

//返回U32
uint32_t GetU32(void* value)
{
	return (((uint8_t*) value)[2] << 24) | (((uint8_t*) value)[3] << 16) | (((uint8_t*) value)[0] << 8) | (((uint8_t*) value)[1]);
}
//返回U16
uint16_t GetU16(void* value)
{
	return (((uint8_t*) value)[0] << 8) | (((uint8_t*) value)[1]);
}
//设置到U16
void SetU16(void* value, uint16_t fact)
{
	((uint8_t*) value)[0] = HIBYTE(fact);
	((uint8_t*) value)[1] = LOBYTE(fact);
}
//设置U16到buf
uint8_t* devices_savedata(uint8_t *data, uint16_t value)
{
	data[0] = HIBYTE(value);
	data[1] = LOBYTE(value);
	return data + 2;
}
//
uint16_t Caculate_Value16(uint8_t Value_H, uint8_t Value_L)
{
	uint16_t Value_16;
	Value_16 = Value_H * 256 + Value_L;
	return (Value_16);
}


//BCD转16进制
uint8_t BcdToBin(uint8_t n)
{
	return ((((n >> 4) & 0x0F) * 10) + (n & 0x0F));
}
//16进制转BCD
uint8_t BinToBcd(uint8_t n)
{
	return (((n / 10) << 4) | (n % 10));
}

//获取协议号
uint32_t Get_Protocol_ID(uint8_t protocol)
{
	uint32_t Protocol_ID = 0;
	switch (protocol)
	{
	case PROPERTY_PROTOCOL_NUM:             //整机属性协议号
		Protocol_ID = 0x00200001;
		break;
	case APP_PROTOCOL_NUM:                  //应用协议号
		Protocol_ID = 0x01113001;
		break;
	case IAP_PROTOCOL_NUM:                  //IAP协议号
		Protocol_ID = 0x03112001;
		break;
	case IAC_PROTOCOL_NUM:                  //IAC协议号
		Protocol_ID = 0x02112001;
		break;
	case IAD_PROTOCOL_NUM:                  //IAD协议号
		Protocol_ID = 0x04112001;
		break;
	case IAT_PROTOCOL_NUM:                  //IAT协议号
		Protocol_ID = 0x05112001;
		break;
	default:
		Protocol_ID = 0;
		break;
	}
	return Protocol_ID;
}
//获取协议版本
uint32_t Get_Protocol_Version(uint8_t protocol)
{
	uint32_t Protocol_Version = 0;
	switch (protocol)
	{
	case PROPERTY_PROTOCOL_VEB:             //整机属性协议版本
		Protocol_Version = 0x01000100;
		break;
	case APP_PROTOCOL_VEB:                  //应用协议版本
		Protocol_Version = 0x01000102;
		break;
	case IAP_PROTOCOL_VEB:                  //IAP协议版本
		Protocol_Version = 0x01000100;
		break;
	case IAC_PROTOCOL_VEB:                  //IAC协议版本
		Protocol_Version = 0x01000101;
		break;
	case IAD_PROTOCOL_VEB:                  //IAD协议版本
		Protocol_Version = 0x01000100;
		break;
	case IAT_PROTOCOL_VEB:                  //IAT协议版本
		Protocol_Version = 0x01000101;
		break;
	default:
		Protocol_Version = 0;
		break;
	}
	return Protocol_Version;
}

//获取ETH0mac
void Get_Local_Mac(NetParam *param, char *ETH)
{
    int sock_mac;
    int i=0;

    struct ifreq ifr_mac;
    char mac_addr[30];

    sock_mac = socket( AF_INET, SOCK_STREAM, 0 );
    if( sock_mac == -1)
    {
        perror("create socket falise...mac\n");
        return ;
    }

    memset(&ifr_mac,0,sizeof(ifr_mac));
    strncpy(ifr_mac.ifr_name, ETH, sizeof(ifr_mac.ifr_name)-1);

    if( (ioctl( sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)
    {
        printf("mac ioctl error\n");
        return ;
    }

    for(i=0; i<6; i++) param->mac[i] = (uint8_t)ifr_mac.ifr_hwaddr.sa_data[i];

    sprintf(mac_addr,"%s: %02x-%02x-%02x-%02x-%02x-%02x", ETH,
    		param->mac[0],param->mac[1],param->mac[2],param->mac[3],param->mac[4],param->mac[5]);
    printf("local mac:%s\n",mac_addr);

    close( sock_mac );
}

//保存网卡参数, ETH -- "eth0","eth1"
int SetNetParam(NetParam param, char *ETH)
{
	char strSh1[]=
	{
		"#!/bin/sh\n"
		"#\n"
		"# Start the network....\n"
		"#\n\n"
		"# Debian ifupdown needs the /run/network lock directory\n"
		"mkdir -p /run/network\n\n"
		"case \"$1\" in\n"
		"  start)\n"
		"    echo \"Starting network...\"\n"
	};
	char strSh2[]=
	{
		"    ;;\n"
		"  stop)\n"
		"    echo -n \"Stopping network...\"\n"
		"    /sbin/ifdown -a\n"
		"    ;;\n"
		"  restart|reload)\n"
		"    \"$0\" stop\n"
		"    \"$0\" start\n"
		"    ;;\n"
		"  *)\n"
		"    echo \"Usage: $0 {start|stop|restart}\"\n"
		"    exit 1\n"
		"esac\n"
		"exit $?\n"
	};
	char str[128];
	int id;
	FILE *fp=NULL;
	if (0 == strcmp (ETH, "eth0")){
		id = 0;
		fp = fopen("/etc/init.d/S42network0", "wt");
		if(fp==NULL) return -1;
	}else if (0 == strcmp (ETH, "eth1")){
		id = 1;
		fp = fopen("/etc/init.d/S41network1", "wt");
		if(fp==NULL) return -1;
	}else{
		return -1;
	}

	fseek(fp, 0, SEEK_SET);
	fwrite((char*)strSh1, 1, strlen(strSh1), fp);
	memset(str, 0x00, 128);
	sprintf(str, "    ifconfig eth%d %d.%d.%d.%d netmask %d.%d.%d.%d up\n",id,param.ip[0],param.ip[1],param.ip[2],param.ip[3],param.submask[0],param.submask[1],param.submask[2],param.submask[3]);
	fwrite((char*)str, 1, strlen(str), fp);
	//memset(str, 0x00, 128);
	//sprintf(str, "    ifconfig eth%d hw ether %02x:%02x:%02x:%02x:%02x:%02x\n",id,param.mac[0],param.mac[1],param.mac[2],param.mac[3],param.mac[4],param.mac[5]);
	//fwrite((char*)str, 1, strlen(str), fp);
	memset(str, 0x00, 128);
	sprintf(str, "    route add default gw %d.%d.%d.%d\n",param.gateway[0],param.gateway[1],param.gateway[2],param.gateway[3]);
	fwrite((char*)str, 1, strlen(str), fp);
	fwrite((char*)strSh2, 1, strlen(strSh2), fp);
	fclose(fp);

	return 0;
}
//读网页历史参数
void Read_Web_History_Data()
{
	//webserver
	//页面语言
	read_eeprom(WEBSERVER_LANGUAGE_ADDRESS, &Webserver_Language, 1);
	if (Webserver_Language != CHINESE && Webserver_Language != ENGLISH)
	{
		Webserver_Language = CHINESE;
		write_eeprom(WEBSERVER_LANGUAGE_ADDRESS, &Webserver_Language, 1);
	}

	//web是否显示PCS界面
	read_eeprom(WEB_PAGE_PCS_ST_ADDRESS, &webPageShow.pcs, 1);
	if (webPageShow.pcs != 0 && webPageShow.pcs != 1)
	{
		webPageShow.pcs = 1;
		write_eeprom(WEB_PAGE_PCS_ST_ADDRESS, &webPageShow.pcs, 1);
	}

	//web是否显示PV界面
	read_eeprom(WEB_PAGE_PV_ST_ADDRESS, &webPageShow.pv, 1);
	if (webPageShow.pv != 0 && webPageShow.pv != 1)
	{
		webPageShow.pv = 0;
		write_eeprom(WEB_PAGE_PV_ST_ADDRESS, &webPageShow.pv, 1);
	}

	//web是否显示DCDC界面
	read_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);
	if (webPageShow.dcdc != 0 && webPageShow.dcdc != 1)
	{
		webPageShow.dcdc = 0;
		write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);
	}

	//web是否显示PVS界面
	read_eeprom(WEB_PAGE_PVS_ST_ADDRESS, &webPageShow.pvs, 1);
	if (webPageShow.pvs != 0 && webPageShow.pvs != 1)
	{
		webPageShow.pvs = 0;
		write_eeprom(WEB_PAGE_PVS_ST_ADDRESS, &webPageShow.pvs, 1);
	}

	//web是否显示BMS界面
	read_eeprom(WEB_PAGE_BMS_ST_ADDRESS, &webPageShow.bms, 1);
	if (webPageShow.bms != 0 && webPageShow.bms != 1)
	{
		webPageShow.pcs = 1;
		write_eeprom(WEB_PAGE_BMS_ST_ADDRESS, &webPageShow.bms, 1);
	}

	//web是否显示电表界面
	read_eeprom(WEB_PAGE_AMMETER_ST_ADDRESS, &webPageShow.ammeter, 1);
	if (webPageShow.ammeter != 0 && webPageShow.ammeter != 1)
	{
		webPageShow.ammeter = 1;
		write_eeprom(WEB_PAGE_AMMETER_ST_ADDRESS, &webPageShow.ammeter, 1);
	}

	//web是否显示实时监控界面
	//read_eeprom(WEB_PAGE_REALTIMEMONITOR_ST_ADDRESS, &webPageShow.realtimeMonitor, 1);
	//if (webPageShow.realtimeMonitor != 0 && webPageShow.realtimeMonitor != 1)
	//{
		webPageShow.realtimeMonitor = 0;
		write_eeprom(WEB_PAGE_REALTIMEMONITOR_ST_ADDRESS, &webPageShow.realtimeMonitor, 1);
	//}

	//web是否显示电池控制界面
	//read_eeprom(WEB_PAGE_BATTERYCONTROL_ST_ADDRESS, &webPageShow.batteryctrl, 1);
	//if (webPageShow.batteryctrl != 0 && webPageShow.batteryctrl != 1)
	//{
		webPageShow.batteryctrl = 0;
		write_eeprom(WEB_PAGE_BATTERYCONTROL_ST_ADDRESS, &webPageShow.batteryctrl, 1);
	//}

	//web是否显示环境仪界面
	read_eeprom(WEB_PAGE_ENV_ST_ADDRESS, &webPageShow.env, 1);
	if (webPageShow.env != 0 && webPageShow.env != 1)
	{
		webPageShow.env = 1;
		write_eeprom(WEB_PAGE_ENV_ST_ADDRESS, &webPageShow.env, 1);
	}

	//操作员密码
    (void)read_eeprom(OPERATOR_PASSWORD_ADDRESS, operator_passwd, 8);
	if(operator_passwd[0]==0 && operator_passwd[1]==0 && operator_passwd[2]==0 && operator_passwd[3]==0 &&
			operator_passwd[4]==0 && operator_passwd[5]==0 && operator_passwd[6]==0 && operator_passwd[7]==0)
	{
		for(uint8_t i=0;i<4;i++){operator_passwd[i]='1';}
		for(uint8_t i=4;i<8;i++){operator_passwd[i]=0;}
		(void)write_eeprom(OPERATOR_PASSWORD_ADDRESS, operator_passwd, 8);
	}
	//管理员密码
    (void)read_eeprom(ADMINISTRATOR_PASSWORD_ADDRESS, administrator_passwd, 8);
	if(administrator_passwd[0]==0 && administrator_passwd[1]==0 && administrator_passwd[2]==0 && administrator_passwd[3]==0 &&
			administrator_passwd[4]==0 && administrator_passwd[5]==0 && administrator_passwd[6]==0 && administrator_passwd[7]==0)
	{
		for(uint8_t i=0;i<4;i++){administrator_passwd[i]='2';}
		for(uint8_t i=4;i<8;i++){administrator_passwd[i]=0;}
		(void)write_eeprom(ADMINISTRATOR_PASSWORD_ADDRESS, administrator_passwd, 8);
	}
	//维护员密码
    (void)read_eeprom(MAINTRAINER_PASSWORD_ADDRESS, maintainer_passwd, 8);
	if(maintainer_passwd[0]==0 && maintainer_passwd[1]==0 && maintainer_passwd[2]==0 && maintainer_passwd[3]==0 &&
			maintainer_passwd[4]==0 && maintainer_passwd[5]==0 && maintainer_passwd[6]==0 && maintainer_passwd[7]==0)
	{
		for(uint8_t i=0;i<4;i++){maintainer_passwd[i]='2';}
		for(uint8_t i=4;i<8;i++){maintainer_passwd[i]=0;}
		(void)write_eeprom(MAINTRAINER_PASSWORD_ADDRESS, maintainer_passwd, 8);
	}
}

//系统初始化
void Global_Variable_Init(void)
{
	g_adjust_flag = 0xA1;                                       //lixia;2012.12.20;0xA1非校表，0xA2校表
	System_Info.Config.System_Power_Set = SYS_POWER_SET_DFT;
	System_Info.Config.PCS_Type = 0;

	for (uint8_t i = 0; i < PCS_NUM; i++)
	{
		PCS_Com_Err[i] = false;
		PCS_Com_OK[i]=false;
		System_Info.EMS_PCS_Com_Count[i] = SIGNAL_FULL;
		Search_PCS_Info_0x03[i] = true;
		Search_PCS_Info_0x04_01[i] = true;
		Search_PCS_Info_0x04_02[i] = true;
		Search_PCS_Info_0x04_03[i] = true;
		Send_Data_To_PCS_0x10_01[i] = true;
		Send_Data_To_PCS_0x10_02[i] = false;
		PCS_Ctrl_Data[i].Grid_Mode_Set = 0x05;
	}
	for (uint8_t i = 0; i < PV_NUM; i++)
	{
		PV_Com_Err[i] = false;
		System_Info.EMS_PV_Com_Count[i] = SIGNAL_FULL;
		Search_PV_Info_0x03[i] = true;
		Send_Data_To_PV_0x10_01[i] = true;
		Send_Data_To_PV_0x10_02[i] = false;
		Search_PV_Info_0x04[i]=true;
	}
	for (uint8_t i = 0; i < BMS_NUM; i++)  //caoxh
	{
		BMS_Com_Err[i] = false;
		System_Info.EMS_BMS_CAN_Com_Count[i] = (BMS_SIGNAL_FULL); // 初始上电10秒，DSP无通讯报警
		BMS_DC_Contactor_on_cmd[i] = false;
		BMS_DC_Contactor_off_cmd[i] = false;
		BMS_All_Fault_Reset_cmd[i] = false;
	}
	for (uint8_t i = 0; i < DCDC_NUM; i++)
	{
		DCDC_Com_Err[i] = false;
		System_Info.EMS_DCDC_Com_Count[i]=SIGNAL_FULL;
		Search_DCDC_Info_0x03[i]     = true;
		Search_DCDC_Info_0x04_01[i]  = true;
		Search_DCDC_Info_0x04_02[i]  = true;
		Search_DCDC_Info_0x04_03[i]  = true;
		Send_Data_To_DCDC_0x10_01[i] = true;
		Send_Data_To_DCDC_0x10_02[i] = false;
	}
	for(uint8_t i=0;i<Meter_NUM;i++)
	{
		Meter_Com_Err[i] = false;
		System_Info.EMS_Meter_Com_Count[i] = METER_SIGNAL_FULL;
		Meter_System_Init(i);	//dl_t645 设置串口校验方式:Even,数据位8b
	}	
	for(uint8_t i=0;i<PVS_NUM;i++)
	{
		PVS_Com_Err[i] = false;
		System_Info.EMS_PVS_Com_Count[i] = PVS_SIGNAL_FULL;
	}
	for(uint8_t i=0;i<ENV_NUM;i++)
	{
		ENV_Com_Err[i] = false;
		System_Info.EMS_ENV_Com_Count[i] = ENV_SIGNAL_FULL;
	}
    for(uint8_t i=0;i<DSP_NUM;i++)  //caoxh
    {
    	DSP_Com_Err[i] = false;
        System_Info.EMS_DSP_Com_Count[i] = (SIGNAL_FULL-30); // 初始上电5秒，DSP无通讯报警
    }
	//初始让触摸屏显示无通讯
	if(DC_BUS_SYS)
	{
		for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
		{
			DCDC_Info[i].Module_1.Module_Dev_Status = 0xff;
			DCDC_Info[i].Module_2.Module_Dev_Status = 0xff;
			DCDC_Info[i].Module_3.Module_Dev_Status = 0xff;
			DCDC_Info[i].Module_4.Module_Dev_Status = 0xff;
		}
	}
	
	Bat_Status.Vbat_G95 = false;
	Bat_Status.Vbat_G90 = false;
	Bat_Status.Vbat_L80 = true;
	Bat_Status.Vbat_L40 = false;
	Bat_Status.Vbat_L30 = false;
	Bat_Status.SOC_G95 = false;
	Bat_Status.SOC_G90 = false;
	Bat_Status.SOC_L80 = true;
	Bat_Status.SOC_L40 = false;
	Bat_Status.SOC_L30 = false;

	IOD_Poweron=1;
	IO_Set_Touch_Req=1;
	read_event_record.read_event_record_flash_flag=true;
	read_fault_record.read_fault_record_flash_flag=true;
 	//write_event_record.event_code=UINT8_MAX;
	System_Info.Config.PCS_Type = SC100;

    //IEC104参数
    System_Info.IEC104_Port = IEC104_PORT;//端口号
    //Webserver参数
    System_Info.Webserver_Port = WEBSERVER_PORT;//端口号
	
}
//到第二天的一些数据处理
void Next_Day_Data_Process(void)
{
	LogTime log_time = {Com_Data.Year_Last,Com_Data.Month_Last,Com_Data.Day_Last,	\
									Com_Data.Hour_Last,Com_Data.Minute_Last,Com_Data.Second_Last};

	//----------------------------------
	if (log_time.Year != 0 && log_time.Month != 0 && log_time.Date != 0 && showTime.tm_year != 0 && showTime.tm_mon != 0 && showTime.tm_day != 0)
	{
		//判断是否是另外一天
		if ((showTime.tm_year != Com_Data.Year_Last) || (showTime.tm_mon != Com_Data.Month_Last) || (showTime.tm_day != Com_Data.Day_Last))
		{
			setLogLastDaytime(&log_time);
			//写历史曲线数据
			Write_Histroy_Curve_Data();
			//清曲线数据
			Clear_Day_Curve_Array();
			//更新运行时间统计值
			System_Run_Info.Day_Run_Time = 0;
			System_Run_Info.Day_MG_Run_Time = 0;
			System_Run_Info.Day_Grid_Run_Time = 0;
			System_Run_Info.Day_Charge_Time = 0;
			System_Run_Info.Day_Discharge_Time = 0;
			System_Run_Info.Day_PV_Run_Time = 0;
			System_Run_Info.Day_Load_Supply_Time = 0;
			System_Run_Info.Day_Fault_Time = 0;

			write_eeprom(DAY_RUNTIME_ADDRESS, &System_Run_Info.Day_Run_Time, 2);
			write_eeprom(DAY_MG_RUNTIME_ADDRESS, &System_Run_Info.Day_MG_Run_Time, 2);
			write_eeprom(DAY_GRID_RUNTIME_ADDRESS, &System_Run_Info.Day_Grid_Run_Time, 2);
			write_eeprom(DAY_CHARGE_TIME_ADDRESS, &System_Run_Info.Day_Charge_Time, 2);
			write_eeprom(DAY_DISCHARGE_TIME_ADDRESS, &System_Run_Info.Day_Discharge_Time, 2);
			write_eeprom(DAY_PV_RUNTIME_ADDRESS, &System_Run_Info.Day_PV_Run_Time, 2);
			write_eeprom(DAY_LOAD_SUPPLY_TIME_ADDRESS, &System_Run_Info.Day_Load_Supply_Time, 2);
			write_eeprom(DAY_FAULT_TIME_ADDRESS, &System_Run_Info.Day_Fault_Time, 2);

			System_Run_Info.Day_Charge_Energy = 0;
			System_Run_Info.Day_Discharge_Energy = 0;
			System_Run_Info.Day_PV_Energy = 0;
			System_Run_Info.Day_Load_Energy = 0;
			write_eeprom(DAY_CHARGE_ENERGY_ADDRESS, &System_Run_Info.Day_Charge_Energy, 2);
			write_eeprom(DAY_DISCHARGE_ENERGY_ADDRESS, &System_Run_Info.Day_Discharge_Energy, 2);
			write_eeprom(DAY_PV_ENERGY_ADDRESS, &System_Run_Info.Day_PV_Energy, 2);
			write_eeprom(DAY_LOAD_ENERGY_ADDRESS, &System_Run_Info.Day_Load_Energy, 2);

			System_Run_Info.Day_Load_Energy_Acc = 0;
			write_eeprom(DAY_LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Day_Load_Energy_Acc, 8);

			//分时电价日收益清0
			Time_Div_Info.Day_Save_Money = 0;
			Time_Div_Info.Day_Charge_Energy_Acc = 0;
			Time_Div_Info.Day_Charge_Money_Acc = 0;
			Time_Div_Info.Day_Discharge_Energy_Acc = 0;
			Time_Div_Info.Day_Discharge_Money_Acc = 0;
			write_eeprom(TIME_DIV_DAY_CHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Day_Charge_Energy_Acc, 8);
			write_eeprom(TIME_DIV_DAY_CHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Day_Charge_Money_Acc, 8);
			write_eeprom(TIME_DIV_DAY_DISCHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Day_Discharge_Energy_Acc, 8);
			write_eeprom(TIME_DIV_DAY_DISCHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Day_Discharge_Money_Acc, 8);
		}
		//判断是否是另外一月
		if ((showTime.tm_year != Com_Data.Year_Last) || (showTime.tm_mon != Com_Data.Month_Last))
		{
			//分时电价月收益清0
			Time_Div_Info.Mon_Save_Money = 0;
			Time_Div_Info.Mon_Charge_Energy_Acc = 0;
			Time_Div_Info.Mon_Charge_Money_Acc = 0;
			Time_Div_Info.Mon_Discharge_Energy_Acc = 0;
			Time_Div_Info.Mon_Discharge_Money_Acc = 0;
			write_eeprom(TIME_DIV_MON_CHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Mon_Charge_Energy_Acc, 8);
			write_eeprom(TIME_DIV_MON_CHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Mon_Charge_Money_Acc, 8);
			write_eeprom(TIME_DIV_MON_DISCHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Mon_Discharge_Energy_Acc, 8);
			write_eeprom(TIME_DIV_MON_DISCHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Mon_Discharge_Money_Acc, 8);
		}
		//判断是否是另外一年
		if (showTime.tm_year != Com_Data.Year_Last)
		{
			//分时电价年收益清0
			Time_Div_Info.Year_Save_Money = 0;
			Time_Div_Info.Year_Charge_Energy_Acc = 0;
			Time_Div_Info.Year_Charge_Money_Acc = 0;
			Time_Div_Info.Year_Discharge_Energy_Acc = 0;
			Time_Div_Info.Year_Discharge_Money_Acc = 0;
			write_eeprom(TIME_DIV_YEAR_CHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Year_Charge_Energy_Acc, 8);
			write_eeprom(TIME_DIV_YEAR_CHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Year_Charge_Money_Acc, 8);
			write_eeprom(TIME_DIV_YEAR_DISCHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Year_Discharge_Energy_Acc, 8);
			write_eeprom(TIME_DIV_YEAR_DISCHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Year_Discharge_Money_Acc, 8);
		}
	}
	//记录下最新的日期，留待下次作比较   ccg 2013.10.25
	if (showTime.tm_year != 0 && showTime.tm_mon != 0 && showTime.tm_day != 0)
	{
		Com_Data.Year_Last = showTime.tm_year;
		Com_Data.Month_Last = showTime.tm_mon;
		Com_Data.Day_Last = showTime.tm_day;
		Com_Data.Hour_Last = showTime.tm_hour;
		Com_Data.Minute_Last = showTime.tm_min;
		Com_Data.Second_Last = showTime.tm_sec;
	}
}

//加载默认运行信息数据   被 Read_History_Data（）调用
void Load_Default_RunInfo_Data(void)
{
	memset(&System_Run_Info, 0, sizeof(S_System_Run_Info_T));

	write_eeprom(TOTAL_RUNTIME_ADDRESS, &System_Run_Info.Total_Run_Time, 4);
	write_eeprom(TOTAL_MG_RUNTIME_ADDRESS, &System_Run_Info.Total_MG_Run_Time, 4);
	write_eeprom(TOTAL_GRID_RUNTIME_ADDRESS, &System_Run_Info.Total_Grid_Run_Time, 4);
	write_eeprom(TOTAL_CHARGE_TIME_ADDRESS, &System_Run_Info.Total_Charge_Time, 4);
	write_eeprom(TOTAL_DISCHARGE_TIME_ADDRESS, &System_Run_Info.Total_Discharge_Time, 4);
	write_eeprom(TOTAL_PV_RUNTIME_ADDRESS, &System_Run_Info.Total_PV_Run_Time, 4);
	write_eeprom(TOTAL_LOAD_SUPPLY_TIME_ADDRESS, &System_Run_Info.Total_Load_Supply_Time, 4);
	write_eeprom(TOTAL_FAULT_TIME_ADDRESS, &System_Run_Info.Total_Fault_Time, 4);

	write_eeprom(DAY_RUNTIME_ADDRESS, &System_Run_Info.Day_Run_Time, 2);
	write_eeprom(DAY_MG_RUNTIME_ADDRESS, &System_Run_Info.Day_MG_Run_Time, 2);
	write_eeprom(DAY_GRID_RUNTIME_ADDRESS, &System_Run_Info.Day_Grid_Run_Time, 2);
	write_eeprom(DAY_CHARGE_TIME_ADDRESS, &System_Run_Info.Day_Charge_Time, 2);
	write_eeprom(DAY_DISCHARGE_TIME_ADDRESS, &System_Run_Info.Day_Discharge_Time, 2);
	write_eeprom(DAY_PV_RUNTIME_ADDRESS, &System_Run_Info.Day_PV_Run_Time, 2);
	write_eeprom(DAY_LOAD_SUPPLY_TIME_ADDRESS, &System_Run_Info.Day_Load_Supply_Time, 2);
	write_eeprom(DAY_FAULT_TIME_ADDRESS, &System_Run_Info.Day_Fault_Time, 2);

	write_eeprom(RUNMIN_ADDRESS, &System_Run_Info.Run_min, 4);
	write_eeprom(MG_RUNMIN_ADDRESS, &System_Run_Info.MG_Run_min, 4);
	write_eeprom(GRID_RUNMIN_ADDRESS, &System_Run_Info.Grid_Run_min, 4);
	write_eeprom(CHARGE_MIN_ADDRESS, &System_Run_Info.Charge_min, 4);
	write_eeprom(DISCHARGE_MIN_ADDRESS, &System_Run_Info.Discharge_min, 4);
	write_eeprom(PV_RUNMIN_ADDRESS, &System_Run_Info.PV_Run_min, 4);
	write_eeprom(LOAD_SUPPLY_MIN_ADDRESS, &System_Run_Info.Load_Supply_min, 4);
	write_eeprom(FAULT_MIN_ADDRESS, &System_Run_Info.Fault_min, 4);

	write_eeprom(TOTAL_CHARGE_ENERGY_ADDRESS, &System_Run_Info.Total_Charge_Energy, 4);
	write_eeprom(TOTAL_DISCHARGE_ENERGY_ADDRESS, &System_Run_Info.Total_Discharge_Energy, 4);
	write_eeprom(TOTAL_PV_ENERGY_ADDRESS, &System_Run_Info.Total_PV_Energy, 4);
	write_eeprom(TOTAL_LOAD_ENERGY_ADDRESS, &System_Run_Info.Total_Load_Energy, 4);

	write_eeprom(DAY_CHARGE_ENERGY_ADDRESS, &System_Run_Info.Day_Charge_Energy, 2);
	write_eeprom(DAY_DISCHARGE_ENERGY_ADDRESS, &System_Run_Info.Day_Discharge_Energy, 2);
	write_eeprom(DAY_PV_ENERGY_ADDRESS, &System_Run_Info.Day_PV_Energy, 2);
	write_eeprom(DAY_LOAD_ENERGY_ADDRESS, &System_Run_Info.Day_Load_Energy, 2);

	write_eeprom(CHARGE_ENERGY_ACC_ADDRESS, &System_Run_Info.Charge_Energy_Acc, 8);
	write_eeprom(DISCHARGE_ENERGY_ACC_ADDRESS, &System_Run_Info.Discharge_Energy_Acc, 8);
	write_eeprom(PV_ENERGY_ACC_ADDRESS, &System_Run_Info.PV_Energy_Acc, 8);
	write_eeprom(LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Load_Energy_Acc, 8);

	write_eeprom(DAY_LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Day_Load_Energy_Acc, 8);
	write_eeprom(E_LOAD_ADJUST_ADDRESS, &System_Run_Info.E_Load_Adjust, 2);

	//分时电价统计量恢复
	Time_Div_Info.Day_Save_Money = 0;
	Time_Div_Info.Mon_Save_Money = 0;
	Time_Div_Info.Year_Save_Money = 0;
	Time_Div_Info.Total_Save_Money = 0;
	//分时电价总收益清0
	Time_Div_Info.Charge_Energy_Acc = 0;
	Time_Div_Info.Charge_Money_Acc = 0;
	Time_Div_Info.Discharge_Energy_Acc = 0;
	Time_Div_Info.Discharge_Money_Acc = 0;
	write_eeprom(TIME_DIV_CHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Charge_Energy_Acc, 8);
	write_eeprom(TIME_DIV_CHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Charge_Money_Acc, 8);
	write_eeprom(TIME_DIV_DISCHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Discharge_Energy_Acc, 8);
	write_eeprom(TIME_DIV_DISCHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Discharge_Money_Acc, 8);
	//分时电价日收益清0
	Time_Div_Info.Day_Charge_Energy_Acc = 0;
	Time_Div_Info.Day_Charge_Money_Acc = 0;
	Time_Div_Info.Day_Discharge_Energy_Acc = 0;
	Time_Div_Info.Day_Discharge_Money_Acc = 0;
	write_eeprom(TIME_DIV_DAY_CHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Day_Charge_Energy_Acc, 8);
	write_eeprom(TIME_DIV_DAY_CHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Day_Charge_Money_Acc, 8);
	write_eeprom(TIME_DIV_DAY_DISCHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Day_Discharge_Energy_Acc, 8);
	write_eeprom(TIME_DIV_DAY_DISCHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Day_Discharge_Money_Acc, 8);
	//分时电价月收益清0
	Time_Div_Info.Mon_Charge_Energy_Acc = 0;
	Time_Div_Info.Mon_Charge_Money_Acc = 0;
	Time_Div_Info.Mon_Discharge_Energy_Acc = 0;
	Time_Div_Info.Mon_Discharge_Money_Acc = 0;
	write_eeprom(TIME_DIV_MON_CHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Mon_Charge_Energy_Acc, 8);
	write_eeprom(TIME_DIV_MON_CHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Mon_Charge_Money_Acc, 8);
	write_eeprom(TIME_DIV_MON_DISCHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Mon_Discharge_Energy_Acc, 8);
	write_eeprom(TIME_DIV_MON_DISCHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Mon_Discharge_Money_Acc, 8);
	//分时电价年收益清0
	Time_Div_Info.Year_Charge_Energy_Acc = 0;
	Time_Div_Info.Year_Charge_Money_Acc = 0;
	Time_Div_Info.Year_Discharge_Energy_Acc = 0;
	Time_Div_Info.Year_Discharge_Money_Acc = 0;
	write_eeprom(TIME_DIV_YEAR_CHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Year_Charge_Energy_Acc, 8);
	write_eeprom(TIME_DIV_YEAR_CHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Year_Charge_Money_Acc, 8);
	write_eeprom(TIME_DIV_YEAR_DISCHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Year_Discharge_Energy_Acc, 8);
	write_eeprom(TIME_DIV_YEAR_DISCHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Year_Discharge_Money_Acc, 8);
}
//读取历史运行信息数据   被 Read_History_Data（）调用
void Read_History_RunInfo_Data(void)
{
	read_eeprom(TOTAL_RUNTIME_ADDRESS, &System_Run_Info.Total_Run_Time, 4);
	read_eeprom(TOTAL_MG_RUNTIME_ADDRESS, &System_Run_Info.Total_MG_Run_Time, 4);
	read_eeprom(TOTAL_GRID_RUNTIME_ADDRESS, &System_Run_Info.Total_Grid_Run_Time, 4);
	read_eeprom(TOTAL_CHARGE_TIME_ADDRESS, &System_Run_Info.Total_Charge_Time, 4);
	read_eeprom(TOTAL_DISCHARGE_TIME_ADDRESS, &System_Run_Info.Total_Discharge_Time, 4);
	read_eeprom(TOTAL_PV_RUNTIME_ADDRESS, &System_Run_Info.Total_PV_Run_Time, 4);
	read_eeprom(TOTAL_LOAD_SUPPLY_TIME_ADDRESS, &System_Run_Info.Total_Load_Supply_Time, 4);
	read_eeprom(TOTAL_FAULT_TIME_ADDRESS, &System_Run_Info.Total_Fault_Time, 4);

	read_eeprom(DAY_RUNTIME_ADDRESS, &System_Run_Info.Day_Run_Time, 2);
	read_eeprom(DAY_MG_RUNTIME_ADDRESS, &System_Run_Info.Day_MG_Run_Time, 2);
	read_eeprom(DAY_GRID_RUNTIME_ADDRESS, &System_Run_Info.Day_Grid_Run_Time, 2);
	read_eeprom(DAY_CHARGE_TIME_ADDRESS, &System_Run_Info.Day_Charge_Time, 2);
	read_eeprom(DAY_DISCHARGE_TIME_ADDRESS, &System_Run_Info.Day_Discharge_Time, 2);
	read_eeprom(DAY_PV_RUNTIME_ADDRESS, &System_Run_Info.Day_PV_Run_Time, 2);
	read_eeprom(DAY_LOAD_SUPPLY_TIME_ADDRESS, &System_Run_Info.Day_Load_Supply_Time, 2);
	read_eeprom(DAY_FAULT_TIME_ADDRESS, &System_Run_Info.Day_Fault_Time, 2);

	read_eeprom(RUNMIN_ADDRESS, &System_Run_Info.Run_min, 4);
	read_eeprom(MG_RUNMIN_ADDRESS, &System_Run_Info.MG_Run_min, 4);
	read_eeprom(GRID_RUNMIN_ADDRESS, &System_Run_Info.Grid_Run_min, 4);
	read_eeprom(CHARGE_MIN_ADDRESS, &System_Run_Info.Charge_min, 4);
	read_eeprom(DISCHARGE_MIN_ADDRESS, &System_Run_Info.Discharge_min, 4);
	read_eeprom(PV_RUNMIN_ADDRESS, &System_Run_Info.PV_Run_min, 4);
	read_eeprom(LOAD_SUPPLY_MIN_ADDRESS, &System_Run_Info.Load_Supply_min, 4);
	read_eeprom(FAULT_MIN_ADDRESS, &System_Run_Info.Fault_min, 4);

	read_eeprom(TOTAL_CHARGE_ENERGY_ADDRESS, &System_Run_Info.Total_Charge_Energy, 4);
	read_eeprom(TOTAL_DISCHARGE_ENERGY_ADDRESS, &System_Run_Info.Total_Discharge_Energy, 4);
	read_eeprom(TOTAL_PV_ENERGY_ADDRESS, &System_Run_Info.Total_PV_Energy, 4);
	read_eeprom(TOTAL_LOAD_ENERGY_ADDRESS, &System_Run_Info.Total_Load_Energy, 4);

	read_eeprom(DAY_CHARGE_ENERGY_ADDRESS, &System_Run_Info.Day_Charge_Energy, 2);
	read_eeprom(DAY_DISCHARGE_ENERGY_ADDRESS, &System_Run_Info.Day_Discharge_Energy, 2);
	read_eeprom(DAY_PV_ENERGY_ADDRESS, &System_Run_Info.Day_PV_Energy, 2);
	read_eeprom(DAY_LOAD_ENERGY_ADDRESS, &System_Run_Info.Day_Load_Energy, 2);

	read_eeprom(CHARGE_ENERGY_ACC_ADDRESS, &System_Run_Info.Charge_Energy_Acc, 8);
	read_eeprom(DISCHARGE_ENERGY_ACC_ADDRESS, &System_Run_Info.Discharge_Energy_Acc, 8);
	read_eeprom(PV_ENERGY_ACC_ADDRESS, &System_Run_Info.PV_Energy_Acc, 8);
	read_eeprom(LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Load_Energy_Acc, 8);

	read_eeprom(DAY_LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Day_Load_Energy_Acc, 8);
	read_eeprom(E_LOAD_ADJUST_ADDRESS, &System_Run_Info.E_Load_Adjust, 2);

	//分时电价统计量记忆
	//分时电价总收益
	read_eeprom(TIME_DIV_CHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Charge_Energy_Acc, 8);
	read_eeprom(TIME_DIV_CHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Charge_Money_Acc, 8);
	read_eeprom(TIME_DIV_DISCHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Discharge_Energy_Acc, 8);
	read_eeprom(TIME_DIV_DISCHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Discharge_Money_Acc, 8);
	//分时电价日收益
	read_eeprom(TIME_DIV_DAY_CHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Day_Charge_Energy_Acc, 8);
	read_eeprom(TIME_DIV_DAY_CHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Day_Charge_Money_Acc, 8);
	read_eeprom(TIME_DIV_DAY_DISCHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Day_Discharge_Energy_Acc, 8);
	read_eeprom(TIME_DIV_DAY_DISCHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Day_Discharge_Money_Acc, 8);
	//分时电价月收益
	read_eeprom(TIME_DIV_MON_CHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Mon_Charge_Energy_Acc, 8);
	read_eeprom(TIME_DIV_MON_CHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Mon_Charge_Money_Acc, 8);
	read_eeprom(TIME_DIV_MON_DISCHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Mon_Discharge_Energy_Acc, 8);
	read_eeprom(TIME_DIV_MON_DISCHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Mon_Discharge_Money_Acc, 8);
	//分时电价年收益
	read_eeprom(TIME_DIV_YEAR_CHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Year_Charge_Energy_Acc, 8);
	read_eeprom(TIME_DIV_YEAR_CHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Year_Charge_Money_Acc, 8);
	read_eeprom(TIME_DIV_YEAR_DISCHARGE_ENERGY_ACC_ADDRESS, &Time_Div_Info.Year_Discharge_Energy_Acc, 8);
	read_eeprom(TIME_DIV_YEAR_DISCHARGE_MONEY_ACC_ADDRESS, &Time_Div_Info.Year_Discharge_Money_Acc, 8);

	//日收益
	if (Time_Div_Info.Day_Discharge_Money_Acc > Time_Div_Info.Day_Charge_Money_Acc)
	{
		Time_Div_Info.Day_Save_Money = Time_Div_Info.Day_Discharge_Money_Acc - Time_Div_Info.Day_Charge_Money_Acc;
	}
	else
	{
		Time_Div_Info.Day_Save_Money = 0;
	}
	//月收益
	if (Time_Div_Info.Mon_Discharge_Money_Acc > Time_Div_Info.Mon_Charge_Money_Acc)
	{
		Time_Div_Info.Mon_Save_Money = Time_Div_Info.Mon_Discharge_Money_Acc - Time_Div_Info.Mon_Charge_Money_Acc;
	}
	else
	{
		Time_Div_Info.Mon_Save_Money = 0;
	}
	//年收益
	if (Time_Div_Info.Year_Discharge_Money_Acc > Time_Div_Info.Year_Charge_Money_Acc)
	{
		Time_Div_Info.Year_Save_Money = Time_Div_Info.Year_Discharge_Money_Acc - Time_Div_Info.Year_Charge_Money_Acc;
	}
	else
	{
		Time_Div_Info.Year_Save_Money = 0;
	}
	//总收益
	if (Time_Div_Info.Discharge_Money_Acc > Time_Div_Info.Charge_Money_Acc)
	{
		Time_Div_Info.Total_Save_Money = Time_Div_Info.Discharge_Money_Acc - Time_Div_Info.Charge_Money_Acc;
	}
	else
	{
		Time_Div_Info.Total_Save_Money = 0;
	}
}
//读取历史数据
void Read_History_Data()
{
	//uint8_t  def8=0;
	//uint32_t def32=0;
	//uint8_t buf[256] = {0};
//--------------------------------------------------------------------
	//IO板启动后第一要读取系统配置参数	
	//IO板第一次上电要恢复默认值    ccg 2013.4.27
	(void) read_eeprom(FIRST_START_ADDRESS, &System_Info.First_Start, 1);
	if (System_Info.First_Start != 0xA1)
	{
		//初次上电写入默认时间
		Com_Data.Year_Last = 1;	//	2001年
		Com_Data.Month_Last = 1;	//     1月
		Com_Data.Day_Last = 1;	//     1日
		Com_Data.Hour_Last = 0;	//     0时
		Com_Data.Minute_Last = 0;	//     0分
		Com_Data.Second_Last = 0;	//     0秒
		Load_Default_Process();
		Load_Default_Net_Process();
		System_Info.First_Start = 0xA1;
		write_eeprom(FIRST_START_ADDRESS, &System_Info.First_Start, 1);

		//初次上电写入MAC地址
		/*IAT_Info.Set_Info.MAC[0] = 0xAC19;
		IAT_Info.Set_Info.MAC[1] = 0x9F01;
		IAT_Info.Set_Info.MAC[2] = 0x0000;
		System_Info.Net.mac[0] = 0xAC;
		System_Info.Net.mac[1] = 0x19;
		System_Info.Net.mac[2] = 0x9F;
		System_Info.Net.mac[3] = 0x1;
		System_Info.Net.mac[4] = 0x0;
		System_Info.Net.mac[5] = 0x0;
		(void) write_eeprom(IAT_MAC_ADDRESS + 0, &IAT_Info.Set_Info.MAC[0], 2);
		(void) write_eeprom(IAT_MAC_ADDRESS + 2, &IAT_Info.Set_Info.MAC[1], 2);
		(void) write_eeprom(IAT_MAC_ADDRESS + 4, &IAT_Info.Set_Info.MAC[2], 2);
		IAT_Info.Set_Info.MAC[0] = 0xAC19;
		IAT_Info.Set_Info.MAC[1] = 0x9F10;
		IAT_Info.Set_Info.MAC[2] = 0x0000;
		System_Info.Net2.mac[0] = 0xAC;
		System_Info.Net2.mac[1] = 0x19;
		System_Info.Net2.mac[2] = 0x9F;
		System_Info.Net2.mac[3] = 0x0;
		System_Info.Net2.mac[4] = 0x0;
		System_Info.Net2.mac[5] = 0x0;
		(void) write_eeprom(IAT_MAC2_ADDRESS + 0, &IAT_Info.Set_Info.MAC2[0], 2);
		(void) write_eeprom(IAT_MAC2_ADDRESS + 2, &IAT_Info.Set_Info.MAC2[1], 2);
		(void) write_eeprom(IAT_MAC2_ADDRESS + 4, &IAT_Info.Set_Info.MAC2[2], 2);*/

		//初次上电写入默认配置
		System_Info.Config.BMS_Type = BMS_None;
		write_eeprom(BMS_TYPE_ADDRESS, &System_Info.Config.BMS_Type, 1);
		webPageShow.bms = 0;
		write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.bms, 1);

		System_Info.Config.PCS_Comm_Type = PCS_COMM_SERIAL;
		write_eeprom(PCS_COMM_ADDRESS, &System_Info.Config.PCS_Comm_Type, 1);

		System_Info.Config.PV_Comm_Type = PV_COMM_SERIAL;
		write_eeprom(PV_COMM_ADDRESS, &System_Info.Config.PV_Comm_Type, 1);

		System_Info.Config.PCS_Grid_Run_Mode = GRID_CONST_V_MODE;
		write_eeprom(PCS_GRID_RUN_MODE_ADDRESS, &System_Info.Config.PCS_Grid_Run_Mode, 1);

		System_Info.Config.Power_Distribution_Enable = ENABLE;
		write_eeprom(POWER_DISTRIBUTION_ENABLE_ADDRESS, &System_Info.Config.Power_Distribution_Enable, 1);

		System_Info.Config.DCDC_Type = 0;
		write_eeprom(DCDC_TYPE_ADDRESS, &System_Info.Config.DCDC_Type, 1);
		webPageShow.dcdc = 0;
		write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);

        System_Info.Config.Meter_Comm_Enable= DISABLE;        
        write_eeprom(METER_COMM_ENABLE_ADDRESS, &System_Info.Config.Meter_Comm_Enable, 1);
		webPageShow.ammeter = 1;
		write_eeprom(WEB_PAGE_AMMETER_ST_ADDRESS, &webPageShow.ammeter, 1);

		webPageShow.env = 0;
		write_eeprom(WEB_PAGE_ENV_ST_ADDRESS, &webPageShow.env, 1);

        wInverterAcVoltDQKp = 200;
        write_eeprom(KP_ADDRESS,&wInverterAcVoltDQKp,2);

        wInverterAcVoltDQKi = 16;
        write_eeprom(KI_ADDRESS,&wInverterAcVoltDQKi,2);

        wInverterAcVoltDQKp_PVLimit = 92;
        write_eeprom(KP_PVLIMIT_ADDRESS,&wInverterAcVoltDQKp_PVLimit,2);

        wInverterAcVoltDQKi_PVLimit = 8;
        write_eeprom(KI_PVLIMIT_ADDRESS,&wInverterAcVoltDQKi_PVLimit,2);

        Balancing_Kp = 10;
        write_eeprom(BALANCING_KP_ADDRESS,&Balancing_Kp,2);

        System_Info.Zero_V_Max = 100;
        write_eeprom(ZERO_V_MAX_ADDRESS,&System_Info.Zero_V_Max,2);

        Parameter_recovery_kp = 101;
        write_eeprom(Parameter_recovery_kp_ADDRESS,&Parameter_recovery_kp,2);
        
        Parameter_recovery_dalay_time = 9;
        write_eeprom(Parameter_recovery_dalay_time_ADDRESS,&Parameter_recovery_dalay_time,2);

        Max_anti_reflux_power = 0;
        write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);

        Send_Data_To_PV_Broadcast_Enable=DISABLE;
        write_eeprom(Send_Data_To_PV_Broadcast_Enable_ADDRESS,&Send_Data_To_PV_Broadcast_Enable,2);
	}

	//读上次掉电时间
	(void) read_eeprom(YEAR_LAST_ADDRESS, &Com_Data.Year_Last, 1);		//年
	(void) read_eeprom(MONTH_LAST_ADDRESS, &Com_Data.Month_Last, 1);	//月
	(void) read_eeprom(DATE_LAST_ADDRESS, &Com_Data.Day_Last, 1);		//日
	(void) read_eeprom(HOUR_LAST_ADDRESS, &Com_Data.Hour_Last, 1);		//时
	(void) read_eeprom(MINUTE_LAST_ADDRESS, &Com_Data.Minute_Last, 1);	//分
	(void) read_eeprom(SECOND_LAST_ADDRESS, &Com_Data.Second_Last, 1);	//秒

	//读历史运行数据
	Read_History_RunInfo_Data();
	//读当天曲线数据
	Read_Day_Curve_Array();

	//串口参数
	for (uint8_t i = 0; i < 6; i++)
	{
		read_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7, (uint8_t*)&comParam[i].baud, 2);
		if(comParam[i].baud != 4800 && comParam[i].baud != 9600 && comParam[i].baud != 19200 && comParam[i].baud != 38400)
		{
			comParam[i].baud = 9600;
			write_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7, &comParam[i].baud, 2);
		}
		read_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+4, (uint8_t*)&comParam[i].sId, 1);
		if(comParam[i].sId < 1 || comParam[i].sId > 247)
		{
			comParam[i].sId = 1;
			write_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+4, &comParam[i].sId, 1);
		}
		read_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+5, (uint8_t*)&comParam[i].eId, 1);
		if(comParam[i].eId < 1 || comParam[i].eId > 247)
		{
			comParam[i].eId = 247;
			write_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+5, &comParam[i].eId, 1);
		}
		read_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+6, (uint8_t*)&comParam[i].protocol, 1);
		if(comParam[i].protocol > SERIAL_PROTOCOL_MODBUS_NONE)
		{
			comParam[i].protocol = SERIAL_PROTOCOL_MODBUS_NONE;
			write_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+6, &comParam[i].protocol, 1);
		}
		read_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+2, (uint8_t*)&comParam[i].vmin, 2);
		if(comParam[i].vmin < 1 || comParam[i].vmin > 255)
		{
			comParam[i].vmin = 255;
			write_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+2, &comParam[i].vmin, 2);
		}
	}

	if(comParam[5].protocol == SERIAL_PROTOCOL_MODBUS_TOUCH) comParam[5].vmin = 8;
	else if(comParam[5].protocol == SERIAL_PROTOCOL_MODBUS_MONITOR) comParam[5].vmin = 255;

	//EMS参数
	//PCS地址
	for (uint8_t i = 0; i < PCS_NUM; i++)
	{
		read_eeprom(PCS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address, 1);
		if ((EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address < 1) || (EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address > 247))
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address = i + 1;
			write_eeprom(PCS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address, 1);
		}
	}
	//PV地址
	for (uint8_t i = 0; i < PV_NUM; i++)
	{
		read_eeprom(PV0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address, 1);
		if ((EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address < 1) || (EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address > 247))
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address = i + 1;
			write_eeprom(PV0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address, 1);
		}
	}
	//BMS地址
	for (uint8_t i = 0; i < BMS_NUM; i++)
	{
		read_eeprom(BMS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address, 1);
		if ((EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address < 1) || (EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address > 247))
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address = i + 1;
			write_eeprom(BMS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address, 1);
		}
	}
	//DCDC地址
	for (uint8_t i = 0; i < DCDC_NUM; i++)
	{
		read_eeprom(DCDC0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address, 1);
		if ((EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address < 1) || (EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address > 247))
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address = i + 1;
			write_eeprom(DCDC0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address, 1);
		}
	}
	//电表地址
	for (uint8_t i = 0; i < Meter_NUM; i++)
	{
		read_eeprom(AMMETER0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address, 1);
		if ((EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address < 1) || (EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address > 247))
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address = i + 1;
			write_eeprom(AMMETER0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address, 1);
		}
	}
	//汇流箱地址
	for (uint8_t i = 0; i < PVS_NUM; i++)
	{
		read_eeprom(PVS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address, 1);
		if ((EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address < 1) || (EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address > 247))
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address = i + 1;
			write_eeprom(PVS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address, 1);
		}
	}
	//环境仪地址
	for (uint8_t i = 0; i < ENV_NUM; i++)
	{
		read_eeprom(ENV0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i].Modubus_Address, 1);
		if ((EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i].Modubus_Address < 1) || (EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i].Modubus_Address > 247))
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i].Modubus_Address = i + 1;
			write_eeprom(ENV0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i].Modubus_Address, 1);
		}
	}
	//电表位置定义
	for (uint8_t i = 0; i < 3; i++)
	{
		read_eeprom(Meter_Position_Enable_ADDRESS0 + i, &System_Info.Config.Meter_Position_Enable[i], 1);
		if((System_Info.Config.Meter_Position_Enable[i]!=ENABLE)&&(System_Info.Config.Meter_Position_Enable[i]!=DISABLE))
		{
			System_Info.Config.Meter_Position_Enable[i] = ENABLE;
			write_eeprom(Meter_Position_Enable_ADDRESS0 + i, &System_Info.Config.Meter_Position_Enable[i], 1);
		}
	}

	//输入引脚定义
	for(uint8_t i=0;i<8;i++)
	{
		read_eeprom(InputPin_Function_ADDRESS0 + i, &System_Info.Config.InputPin_Function[i], 1);
		if((System_Info.Config.InputPin_Function[i]!=ALARM_PIN)&&(System_Info.Config.InputPin_Function[i]!=FAULT_PIN)&&
		   (System_Info.Config.InputPin_Function[i]!=NODEF_PIN))
		{
			System_Info.Config.InputPin_Function[i]=NODEF_PIN;
			write_eeprom(InputPin_Function_ADDRESS0 + i, &System_Info.Config.InputPin_Function[i], 1);
		}
	}

	//输入引脚常开常闭定义
	for(uint8_t i=0;i<8;i++)
	{
		read_eeprom(InputPin_NC_NO_ADDRESS0 + i, &System_Info.Config.InputPin_NC_NO[i], 1);
		if((System_Info.Config.InputPin_NC_NO[i]!=NORMAL_OPEN)&&(System_Info.Config.InputPin_NC_NO[i]!=NORMAL_CLOSE))
		{
			System_Info.Config.InputPin_NC_NO[i]=NORMAL_OPEN;
			write_eeprom(InputPin_NC_NO_ADDRESS0 + i, &System_Info.Config.InputPin_NC_NO[i], 1);
		}
	}

	//-----------------------------------------------------------------------------------------
	//系统配置参数设置
	//EMS运行模式
	read_eeprom(EMS_RUN_MODE_ADDRESS, &System_Info.Config.EMS_Run_Priority, 1);
	if ((System_Info.Config.EMS_Run_Priority != 0) && (System_Info.Config.EMS_Run_Priority != 1))
	{
		System_Info.Config.EMS_Run_Priority = 0;
		write_eeprom(EMS_RUN_MODE_ADDRESS, &System_Info.Config.EMS_Run_Priority, 1);
	}
	//EMS启动类型
	read_eeprom(EMS_START_TYPE_ADDRESS, &System_Info.Config.EMS_Start_Type, 1);
	if ((System_Info.Config.EMS_Start_Type != ACTIVE_MODE) && (System_Info.Config.EMS_Start_Type != REACTIVE_MODE))
	{
		System_Info.Config.EMS_Start_Type = 1; // reactive
		write_eeprom(EMS_START_TYPE_ADDRESS, &System_Info.Config.EMS_Start_Type, 1);
	}
	//PCS协议类型
	read_eeprom(PCS_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.PCS_Protocol_Type, 1);
	if ((System_Info.Config.PCS_Protocol_Type != NEW_PROTOCOL) && (System_Info.Config.PCS_Protocol_Type != OLD_PROTOCOL))
	{
		System_Info.Config.PCS_Protocol_Type = NEW_PROTOCOL;
		write_eeprom(PCS_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.PCS_Protocol_Type, 1);
	}
	//电表协议类型
	read_eeprom(AMMETER_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.Ammeter_Protocol_Type, 1);
	if ((System_Info.Config.Ammeter_Protocol_Type != 0) && (System_Info.Config.Ammeter_Protocol_Type != 1))
	{
		System_Info.Config.Ammeter_Protocol_Type = 0;
		write_eeprom(AMMETER_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.Ammeter_Protocol_Type, 1);
	}
	//母线类型
	read_eeprom(BUS_TYPE_ADDRESS, &System_Info.Config.Bus_Type, 1);
	if ((System_Info.Config.Bus_Type != 0) && (System_Info.Config.Bus_Type != 1))
	{
		System_Info.Config.Bus_Type = 0;
		write_eeprom(BUS_TYPE_ADDRESS, &System_Info.Config.Bus_Type, 1);
	}
	if(System_Info.Config.Bus_Type == 1){  //交流母线
		webPageShow.pcs = 1;
		webPageShow.pv = 1;
		write_eeprom(WEB_PAGE_PCS_ST_ADDRESS, &webPageShow.pcs, 1);
		write_eeprom(WEB_PAGE_PV_ST_ADDRESS, &webPageShow.pv, 1);
	}else{
		webPageShow.pcs = 1;
		webPageShow.pv = 0;
		write_eeprom(WEB_PAGE_PCS_ST_ADDRESS, &webPageShow.pcs, 1);
		write_eeprom(WEB_PAGE_PV_ST_ADDRESS, &webPageShow.pv, 1);
	}
	//是否并网类型
	read_eeprom(GRID_CONNECT_TYPE_ADDRESS, &Grid_Connect_Type, 1);
	if (Grid_Connect_Type != 0 && Grid_Connect_Type != 1)
	{
		Grid_Connect_Type = 0;
		write_eeprom(GRID_CONNECT_TYPE_ADDRESS, &Grid_Connect_Type, 1);
	}
	//是否并网类型
	read_eeprom(DC_BUS_CHARGE_PERMIT_ADDRESS, &System_Info.Config.DC_Bus_Charge_Permit, 1);
	if ((System_Info.Config.DC_Bus_Charge_Permit != 0) && (System_Info.Config.DC_Bus_Charge_Permit != 1))
	{
		System_Info.Config.DC_Bus_Charge_Permit = 0;
		write_eeprom(DC_BUS_CHARGE_PERMIT_ADDRESS, &System_Info.Config.DC_Bus_Charge_Permit, 1);
	}
	//系统功率设置
	read_eeprom(SYS_POWER_SET_ADDRESS, &System_Info.Config.System_Power_Set, 2);
	//if((System_Info.Config.System_Power_Set<SYS_POWER_SET_MIN)||(System_Info.Config.System_Power_Set>SYS_POWER_SET_MAX))
	if (System_Info.Config.System_Power_Set > SYS_POWER_SET_MAX)
	{
		System_Info.Config.System_Power_Set = SYS_POWER_SET_DFT;
		write_eeprom(SYS_POWER_SET_ADDRESS, &System_Info.Config.System_Power_Set, 2);
	}
	//PCS类型选择
    (void)read_eeprom(SYS_PCS_POWER_TYPE_ADDRESS,&System_Info.Config.PCS_Type,1);
	if((System_Info.Config.PCS_Type!=PCS_Power_SC30_50)&&(System_Info.Config.PCS_Type!=PCS_Power_SC100)	&& (System_Info.Config.PCS_Type!=PCS_Power_SC250)
	 &&(System_Info.Config.PCS_Type!=PCS_Power_SC500)&&(System_Info.Config.PCS_Type!=PCS_Power_SC500TL)	&& (System_Info.Config.PCS_Type!=PCS_Power_SC1000))
	{
		System_Info.Config.PCS_Type = PCS_Power_SC30_50;
		write_eeprom(SYS_PCS_POWER_TYPE_ADDRESS, &System_Info.Config.PCS_Type, 1);
	}
	//BMS类型选择
	read_eeprom(BMS_TYPE_ADDRESS, &System_Info.Config.BMS_Type, 1);
	if ((System_Info.Config.BMS_Type != 0) && (System_Info.Config.BMS_Type != 1) && (System_Info.Config.BMS_Type != 2))
	{
		System_Info.Config.BMS_Type = 0;
		write_eeprom(BMS_TYPE_ADDRESS, &System_Info.Config.BMS_Type, 1);
	}
	if(System_Info.Config.BMS_Type == 0)
	{
		webPageShow.bms = 0;
		write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.bms, 1);
	}
	else
	{
		webPageShow.bms = 1;
		write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.bms, 1);
	}

	//PCS通讯类型选择
	read_eeprom(PCS_COMM_ADDRESS, &System_Info.Config.PCS_Comm_Type, 1);
	if ((System_Info.Config.PCS_Comm_Type != PCS_COMM_SERIAL) && (System_Info.Config.PCS_Comm_Type != PCS_COMM_LAN))
	{
		System_Info.Config.PCS_Comm_Type = PCS_COMM_SERIAL;
		write_eeprom(PCS_COMM_ADDRESS, &System_Info.Config.PCS_Comm_Type, 1);
	}

	//PV通讯类型选择
	read_eeprom(PV_COMM_ADDRESS, &System_Info.Config.PV_Comm_Type, 1);
	if ((System_Info.Config.PV_Comm_Type != PV_COMM_SERIAL) && (System_Info.Config.PV_Comm_Type != PV_COMM_LAN))
	{
		System_Info.Config.PV_Comm_Type = PV_COMM_SERIAL;
		write_eeprom(PV_COMM_ADDRESS, &System_Info.Config.PV_Comm_Type, 1);
	}

	//PCS并网运行模式选择
	read_eeprom(PCS_GRID_RUN_MODE_ADDRESS,&  System_Info.Config.PCS_Grid_Run_Mode,1);
	if((System_Info.Config.PCS_Grid_Run_Mode!=GRID_CONST_I_MODE)   &&(System_Info.Config.PCS_Grid_Run_Mode!=GRID_CONST_V_MODE)
	&& (System_Info.Config.PCS_Grid_Run_Mode!=GRID_CONST_P_DC_MODE)&&(System_Info.Config.PCS_Grid_Run_Mode!=GRID_CONST_P_AC_MODE)
	&& (System_Info.Config.PCS_Grid_Run_Mode!=GRID_STANDBY_MODE))
	{
		System_Info.Config.PCS_Grid_Run_Mode = GRID_CONST_V_MODE;
		write_eeprom(PCS_GRID_RUN_MODE_ADDRESS, &System_Info.Config.PCS_Grid_Run_Mode, 1);
	}

	//配电功能使能/禁止
	read_eeprom(POWER_DISTRIBUTION_ENABLE_ADDRESS, &System_Info.Config.Power_Distribution_Enable, 1);
	if (System_Info.Config.Power_Distribution_Enable != 0xAA)
	{
		System_Info.Config.Power_Distribution_Enable = 0x55;
		write_eeprom(POWER_DISTRIBUTION_ENABLE_ADDRESS, &System_Info.Config.Power_Distribution_Enable, 1);
	}
	//DCDC通讯使能/禁止
	read_eeprom(DCDC_TYPE_ADDRESS, &System_Info.Config.DCDC_Type, 1);
	if ((System_Info.Config.DCDC_Type != 0) && (System_Info.Config.DCDC_Type != 1) && (System_Info.Config.DCDC_Type != 2))
	{
		System_Info.Config.DCDC_Type = 0;
		write_eeprom(DCDC_TYPE_ADDRESS, &System_Info.Config.DCDC_Type, 1);
	}
	if(System_Info.Config.DCDC_Type == 0)
	{
		webPageShow.dcdc = 0;
		write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);
	}
	else
	{
		webPageShow.dcdc = 1;
		write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);
	}
	//Meter通讯使能/禁止
	read_eeprom(METER_COMM_ENABLE_ADDRESS, &System_Info.Config.Meter_Comm_Enable, 1);
	if (System_Info.Config.Meter_Comm_Enable != 0xAA)
	{
		System_Info.Config.Meter_Comm_Enable = 0x55;
		write_eeprom(METER_COMM_ENABLE_ADDRESS, &System_Info.Config.Meter_Comm_Enable, 1);
	}
	//----------------------------------------------------------------------------------------------------------------
	//PCS总台数
	read_eeprom(TOTAL_PCS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number < P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number > P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number = P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Dft;
		write_eeprom(TOTAL_PCS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number, 2);
	}
	//PV总台数
	read_eeprom(TOTAL_PV_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number < P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number > P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number = P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Dft;
		write_eeprom(TOTAL_PV_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number, 2);
	}
	//BMS总台数
	read_eeprom(TOTAL_BMS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number < P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number > P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number = P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Dft;
		write_eeprom(TOTAL_BMS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number, 2);
	}
	//DCDC总台数
	read_eeprom(TOTAL_DCDC_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number < P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number > P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number = P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Dft;
		write_eeprom(TOTAL_DCDC_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number, 2);
	}

	//读电表总台数
	read_eeprom(TOTAL_AMMETER_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number < P_Range[System_Info.Config.PCS_Type].AMMETER_Total_Num.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number > P_Range[System_Info.Config.PCS_Type].AMMETER_Total_Num.Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number = P_Range[System_Info.Config.PCS_Type].AMMETER_Total_Num.Dft;
		write_eeprom(TOTAL_AMMETER_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number, 2);
	}

	//读汇流箱总台数
	read_eeprom(TOTAL_PVS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number < P_Range[System_Info.Config.PCS_Type].PVS_Total_Num.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number > P_Range[System_Info.Config.PCS_Type].PVS_Total_Num.Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number = P_Range[System_Info.Config.PCS_Type].PVS_Total_Num.Dft;
		write_eeprom(TOTAL_PVS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number, 2);
	}

	//环境仪总数
	EMS_Info.EMS4x.AGC_Info.SysParameter.Total_ENV_Number = 1;

	//电压保护参数KP
	read_eeprom(KP_ADDRESS,&wInverterAcVoltDQKp,2);
    if((wInverterAcVoltDQKp>6000)||(0==wInverterAcVoltDQKp))
    {
        wInverterAcVoltDQKp = 540;
        write_eeprom(KP_ADDRESS,&wInverterAcVoltDQKp,2);
    }
	//电压保护参数KI
	read_eeprom(KI_ADDRESS,&wInverterAcVoltDQKi,2);
    if(wInverterAcVoltDQKi>6000)
    {
        wInverterAcVoltDQKi= 6;
        write_eeprom(KI_ADDRESS,&wInverterAcVoltDQKi,2);
    }    
	//电压保护参数 pv KP
	read_eeprom(KP_PVLIMIT_ADDRESS,&wInverterAcVoltDQKp_PVLimit,2);
    if((wInverterAcVoltDQKp_PVLimit>6000)||(0==wInverterAcVoltDQKp_PVLimit))
    {
        wInverterAcVoltDQKp_PVLimit = 675;
        write_eeprom(KP_PVLIMIT_ADDRESS,&wInverterAcVoltDQKp_PVLimit,2);
    }
	//电压保护参数 pv KI
	read_eeprom(KI_PVLIMIT_ADDRESS,&wInverterAcVoltDQKi_PVLimit,2);
    if(wInverterAcVoltDQKi_PVLimit>6000)
    {
        wInverterAcVoltDQKi_PVLimit = 12;
        write_eeprom(KI_PVLIMIT_ADDRESS,&wInverterAcVoltDQKi_PVLimit,2);
    }
	//自动均衡KP
	read_eeprom(BALANCING_KP_ADDRESS,&Balancing_Kp,2);
    if((Balancing_Kp>1000)||(0==Balancing_Kp))
    {
        Balancing_Kp= 10;
        write_eeprom(BALANCING_KP_ADDRESS,&Balancing_Kp,2);
    }  
    //零电压最大值
	read_eeprom(ZERO_V_MAX_ADDRESS,&System_Info.Zero_V_Max,2);
	if((System_Info.Zero_V_Max<50)||(System_Info.Zero_V_Max>500))
    {
        System_Info.Zero_V_Max = 100;
        write_eeprom(ZERO_V_MAX_ADDRESS,&System_Info.Zero_V_Max,2);
    }
	//回调比例
	read_eeprom(Parameter_recovery_kp_ADDRESS,&Parameter_recovery_kp,2);
    if((Parameter_recovery_kp>10000)||(Parameter_recovery_kp<101))
    {
        Parameter_recovery_kp= 101;
        write_eeprom(Parameter_recovery_kp_ADDRESS,&Parameter_recovery_kp,2);
    }  
	//回调延时
	read_eeprom(Parameter_recovery_dalay_time_ADDRESS,&Parameter_recovery_dalay_time,2);
    if((Parameter_recovery_dalay_time>100)||(Parameter_recovery_dalay_time<1))
    {
        Parameter_recovery_dalay_time= 9; //10*300 3s
        write_eeprom(Parameter_recovery_dalay_time_ADDRESS,&Parameter_recovery_dalay_time,2);
    }  
	//最大逆流功率
	read_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
    if((Max_anti_reflux_power>(int16_t)(30000))||(Max_anti_reflux_power<0))
    {
        Max_anti_reflux_power= 0; 
        write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
    }  

    //光伏广播使能
	read_eeprom(Send_Data_To_PV_Broadcast_Enable_ADDRESS,&Send_Data_To_PV_Broadcast_Enable,2);
	if(Send_Data_To_PV_Broadcast_Enable!=ENABLE)
	{
		Balancing_Kp = 10;
		write_eeprom(BALANCING_KP_ADDRESS, &Balancing_Kp, 2);
        Send_Data_To_PV_Broadcast_Enable=DISABLE;
        write_eeprom(Send_Data_To_PV_Broadcast_Enable_ADDRESS,&Send_Data_To_PV_Broadcast_Enable,2);
	}
	//soc均衡使能/禁止
	read_eeprom(BALANCING_ENABLE_ADDRESS, &System_Info.Config.Balancing_Enable, 1);
	if (System_Info.Config.Balancing_Enable != ENABLE)
	{
		System_Info.Config.Balancing_Enable = DISABLE;
		write_eeprom(BALANCING_ENABLE_ADDRESS, &System_Info.Config.Balancing_Enable, 1);
	}
	//PCS启动策略
	read_eeprom(PCS_ISLAND_CTRL_MODE_ADDRESS, &System_Info.Config.PCS_Island_Ctrl_Mode, 1);
	if ((System_Info.Config.PCS_Island_Ctrl_Mode != 0) && (System_Info.Config.PCS_Island_Ctrl_Mode != 1))
	{
		System_Info.Config.PCS_Island_Ctrl_Mode = 0;
		write_eeprom(PCS_ISLAND_CTRL_MODE_ADDRESS, &System_Info.Config.PCS_Island_Ctrl_Mode, 1);
	}
	//启动微网电压
	read_eeprom(MG_VOLTAGE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage < P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage > P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage = P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Dft;
		write_eeprom(MG_VOLTAGE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage, 2);
	}
	//启动微网频率
	read_eeprom(MG_FREQUENCY_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency < P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency > P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency = P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Dft;
		write_eeprom(MG_FREQUENCY_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency, 2);
	}
	//并网运行电压上限
	read_eeprom(CONST_V_MODE_V_MAX_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max < P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max > P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max;
		write_eeprom(CONST_V_MODE_V_MAX_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max, 2);
	}
	//并网运行电压下限
	read_eeprom(CONST_V_MODE_V_MIN_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min < P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min > P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min >= EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min;
		write_eeprom(CONST_V_MODE_V_MIN_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min, 2);
	}
	//并网运行功率设置
	read_eeprom(EMS_RUN_POWER_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set, 2);
	if ((EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set < -(int16_t)((float)System_Info.Config.System_Power_Set * 1.1))
	 || (EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set >  (int16_t)((float)System_Info.Config.System_Power_Set * 1.1)))
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set = 0;
		write_eeprom(EMS_RUN_POWER_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set, 2);
	}
	//恒压模式电压
	read_eeprom(CONST_V_MODE_V_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V < P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V > P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Dft;
		write_eeprom(CONST_V_MODE_V_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V, 2);
	}
	//恒压模式电流
	read_eeprom(CONST_V_MODE_I_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I, 2);
	if (EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I < P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Min
			|| EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I > P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Max)
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Dft;
		write_eeprom(CONST_V_MODE_I_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I, 2);
	}
	//无功调节方式
	read_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);
	if ((EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode != QP) && (EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode != PF) && (EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode != SHUT_OFF))
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode = SHUT_OFF;
		write_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);
	}
	for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
	{
		//PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode=EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
		PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode = EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
		Send_Data_To_PCS_0x10_02[i] = true;
	}
	//功率因数
	read_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
	if ((abs(EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor) < P_Range[System_Info.Config.PCS_Type].Power_Factor.Min)
			|| (abs(EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor) > P_Range[System_Info.Config.PCS_Type].Power_Factor.Max))
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor = P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft;
		write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
	}
	for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
	{
		PCS_Ctrl_Data[i].Power_Factor = EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
		Send_Data_To_PCS_0x10_02[i] = true;
	}
	//无功功率
	read_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
	if ((EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set < -(int16_t)((float)System_Info.Config.System_Power_Set * 0.3))
	 || (EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set >  (int16_t)((float)System_Info.Config.System_Power_Set * 0.3)))
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set = 0;
		write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
	}

	//远程本地模式
	read_eeprom(EMS_RL_MODE_ADDRESS, &EMS_RL_Mode, 2);
	if (EMS_RL_Mode != RE_LO_MODE && EMS_RL_Mode != REMOTE_MODE && EMS_RL_Mode != LOCAL_MODE)
	{
		EMS_RL_Mode = RE_LO_MODE;
		write_eeprom(EMS_RL_MODE_ADDRESS, &EMS_RL_Mode, 1);
	}

    //低电量节点1输出模式    
	read_eeprom(Low_Battery_Output1_Mode_ADDRESS,&EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode,1);
    if(EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode!=NORMAL_OPEN)
    {
        EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode=NORMAL_CLOSE;
        write_eeprom(Low_Battery_Output1_Mode_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode, 1);
    }

    //低电量节点2输出模式    
	read_eeprom(Low_Battery_Output2_Mode_ADDRESS,&EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode,1);
    if(EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode!=NORMAL_OPEN)
    {
        EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode=NORMAL_CLOSE;
        write_eeprom(Low_Battery_Output2_Mode_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode, 1);
    }
	
	//SOC控制使能/禁止
	read_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable != ENABLE)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable = DISABLE;
		write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable = ENABLE;
		write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
	}
    //SOC上限2级
	read_eeprom(SOC_UPLIMIT2_ADDRESS,&EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2,2);
	if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2<P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Min || 
	   EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2>P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Max)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2 = P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Dft;
		write_eeprom(SOC_UPLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2, 2);
	}
	//SOC上限1级
	read_eeprom(SOC_UPLIMIT1_ADDRESS,&EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1,2);
	if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1<P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Min || 
	   EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1>P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Max ||
	   EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1 = P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Dft;
		write_eeprom(SOC_UPLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1, 2);
	}
	//SOC下限1级
	read_eeprom(SOC_LOWLIMIT1_ADDRESS,&EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1,2);
	if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1<P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Min || 
	   EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1>P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Max ||
	   EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1 ||
	   EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1 = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Dft;
		write_eeprom(SOC_LOWLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1, 2);
	}
	//SOC下限2级
	read_eeprom(SOC_LOWLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2 < P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2 > P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Max
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2 > EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2 = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Dft;
		write_eeprom(SOC_LOWLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2, 2);
	}
	//SOC下限1级恢复值
	read_eeprom(SOC_LOWLIMIT1_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover < P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover > P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Max
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover < EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Dft;
		write_eeprom(SOC_LOWLIMIT1_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover, 2);
	}
	//SOC下限2级恢复值
	read_eeprom(SOC_LOWLIMIT2_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover < P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover > P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Max
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover < EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Dft;
		write_eeprom(SOC_LOWLIMIT2_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover, 2);
	}
	//SOC上限滞环控制值
	read_eeprom(SOC_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis == 0)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis = 10; //
		write_eeprom(SOC_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis, 2);
	}
	//SOC响应时间
	read_eeprom(SOC_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay < P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay > P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Max)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay = P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Dft;
		write_eeprom(SOC_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay, 2);
	}
	//SOC恢复时间
	read_eeprom(SOC_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay < P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay > P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Max)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay = P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Dft;
		write_eeprom(SOC_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay, 2);
	}

	//电池电压控制使能/禁止
	read_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable != ENABLE)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable = DISABLE;
		write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
		EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable = ENABLE;
		write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
	}
	//电压上限2级
	read_eeprom(VBAT_UPLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2 < P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2 > P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Max)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2 = P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Dft;
		write_eeprom(VBAT_UPLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2, 2);
	}
	//电压上限1级
	read_eeprom(VBAT_UPLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1 < P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1 > P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Max
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1 > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1 < EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1 = P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Dft;
		write_eeprom(VBAT_UPLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1, 2);
	}
	//电压下限1级
	read_eeprom(VBAT_LOWLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 < P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 > P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Max
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 < EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Dft;
		write_eeprom(VBAT_LOWLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1, 2);
	}
	//电压下限2级
	read_eeprom(VBAT_LOWLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2 < P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2 > P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Max
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2 > EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2 = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Dft;
		write_eeprom(VBAT_LOWLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2, 2);
	}
	//电压下限1级恢复
	read_eeprom(VBAT_LOWLIMIT1_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover < P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover > P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Max
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover < EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Dft;
		write_eeprom(VBAT_LOWLIMIT1_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover, 2);
	}
	//电压下限2级恢复
	read_eeprom(VBAT_LOWLIMIT2_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover < P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover > P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Max
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover < EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Dft;
		write_eeprom(VBAT_LOWLIMIT2_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover, 2);
	}
	//电压上限滞环控制
	read_eeprom(VBAT_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis == 0)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis = 50;
		write_eeprom(VBAT_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis, 2);
	}
	//电压控制延时
	read_eeprom(VBAT_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay < P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay > P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Max)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay = P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Dft;
		write_eeprom(VBAT_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay, 2);
	}
	//电压恢复时间
	read_eeprom(VBAT_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay, 2);
	if (EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay < P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Min
			|| EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay > P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Max)
	{
		EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay = P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Dft;
		write_eeprom(VBAT_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay, 2);
	}

	//--------------------------------------------------------------------
	//分时电价设置点表
	//分时功能选择
	read_eeprom(TIME_DIVIDE_FUNCTION_ADDRESS, &System_Info.Config.Time_Divide_Function, 2);
	if ((System_Info.Config.Time_Divide_Function != 0) && (System_Info.Config.Time_Divide_Function != 1) && (System_Info.Config.Time_Divide_Function != 2))
	{
		System_Info.Config.Time_Divide_Function = 0;
		write_eeprom(TIME_DIVIDE_FUNCTION_ADDRESS, &System_Info.Config.Time_Divide_Function, 2);
	}

	for(uint8_t i=0; i<10; i++)
	{
		//设置0
		//起始时间0(时)
		read_eeprom(TIME_DIV_START_TIME_HOUR_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].Start_time.hour, 1);
		if (Time_Div_Info.Time_Div_Set_Info[i].Start_time.hour > 23)
		{
			Time_Div_Info.Time_Div_Set_Info[i].Start_time.hour = 0;
			write_eeprom(TIME_DIV_START_TIME_HOUR_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].Start_time.hour, 1);
		}
		//起始时间0(分)
		read_eeprom(TIME_DIV_START_TIME_MIN_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].Start_time.min, 1);
		if (Time_Div_Info.Time_Div_Set_Info[i].Start_time.min > 59)
		{
			Time_Div_Info.Time_Div_Set_Info[i].Start_time.min = 0;
			write_eeprom(TIME_DIV_START_TIME_MIN_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].Start_time.min, 1);
		}
		//结束时间0(时)
		read_eeprom(TIME_DIV_END_TIME_HOUR_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].End_time.hour, 1);
		if (Time_Div_Info.Time_Div_Set_Info[i].End_time.hour > 23)
		{
			Time_Div_Info.Time_Div_Set_Info[i].End_time.hour = 0;
			write_eeprom(TIME_DIV_END_TIME_HOUR_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].End_time.hour, 1);
		}
		//结束时间0(分)
		read_eeprom(TIME_DIV_END_TIME_MIN_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].End_time.min, 1);
		if (Time_Div_Info.Time_Div_Set_Info[i].End_time.min > 59)
		{
			Time_Div_Info.Time_Div_Set_Info[i].End_time.min = 0;
			write_eeprom(TIME_DIV_END_TIME_MIN_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].End_time.min, 1);
		}
		//运行模式
		read_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].Run_Mode, 1);
		if ((Time_Div_Info.Time_Div_Set_Info[i].Run_Mode != GRID_MODE) && (Time_Div_Info.Time_Div_Set_Info[i].Run_Mode != ISLAND_MODE) && (Time_Div_Info.Time_Div_Set_Info[i].Run_Mode != GRID_ISLAND_MODE))
		{
			Time_Div_Info.Time_Div_Set_Info[i].Run_Mode = GRID_ISLAND_MODE;
			write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[0].Run_Mode, 1);
		}
		//功率控制模式
		read_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode, 1);
		if(Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode >20)
		{
			Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode = 0;
			write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode, 1);
		}
		//运行功率设定
		read_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+i*2, &Time_Div_Info.Time_Div_Set_Info[i].Run_Power, 2);
		if(Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode != PEAK_LOAD_SHIFTING &&
			Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode != ACSIDE_PVLIMIT &&
				Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode != RAMP_RATE_CONTROL &&
					Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode != FM)
		{
			if ((Time_Div_Info.Time_Div_Set_Info[i].Run_Power < -(int16_t)((float)System_Info.Config.System_Power_Set * 1.1)) ||
				(Time_Div_Info.Time_Div_Set_Info[i].Run_Power >  (int16_t)((float)System_Info.Config.System_Power_Set * 1.1)))
			{
				Time_Div_Info.Time_Div_Set_Info[i].Run_Power = 0;
				write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+i*2, &Time_Div_Info.Time_Div_Set_Info[i].Run_Power, 2);
			}
		}
		//离网运行功率设定
		read_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+i*2, &Time_Div_Info.Time_Div_Set_Info[i].Mgrid_Run_Power, 2);
		if ((Time_Div_Info.Time_Div_Set_Info[i].Mgrid_Run_Power < -(int16_t)((float)System_Info.Config.System_Power_Set * 1.1)) ||
			(Time_Div_Info.Time_Div_Set_Info[i].Mgrid_Run_Power >  (int16_t)((float)System_Info.Config.System_Power_Set * 1.1)))
		{
			Time_Div_Info.Time_Div_Set_Info[i].Mgrid_Run_Power = 0;
			write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+i*2, &Time_Div_Info.Time_Div_Set_Info[i].Mgrid_Run_Power, 2);
		}
	}

	for(uint8_t i=0; i<7; i++)
	{
		//分时星期0
		read_eeprom(TIME_DIV_WEEK_0_ADDRESS+i, &Time_Div_Info.Time_Div_Set_Info[i].Week, 1);
		if (Time_Div_Info.Time_Div_Set_Info[i].Week != 1+i)
		{
			Time_Div_Info.Time_Div_Set_Info[i].Week = 1+i;
			write_eeprom(TIME_DIV_WEEK_0_ADDRESS+i, &Time_Div_Info.Time_Div_Set_Info[i].Week, 1);
		}
	}

	//削锋填谷
	//谷值
	read_eeprom(PEAK_VALLEY_1_START_CHARGE_POWER_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power, 2);
	if ((Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power < 0) ||
		(Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power > (int16_t)((float)System_Info.Config.System_Power_Set * 1.1)))
	{
		Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power = 0;
		write_eeprom(PEAK_VALLEY_1_START_CHARGE_POWER_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power, 2);
	}
	//峰值
	read_eeprom(PEAK_VALLEY_1_START_DISCHARGE_POWER_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power, 2);
	if ((Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power < 0) ||
		(Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power > (int16_t)((float)System_Info.Config.System_Power_Set * 1.1)))
	{
		Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power = 0;
		write_eeprom(PEAK_VALLEY_1_START_DISCHARGE_POWER_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power, 2);
	}
	//更新周期
	read_eeprom(PEAK_VALLEY_1_UPDATE_CYCLE_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle, 2);
	if ((Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle < 300) || (Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle > 30000))
	{
		Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle = 6900;
		write_eeprom(PEAK_VALLEY_1_UPDATE_CYCLE_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle, 2);
	}

	//平滑上网
	//额定光伏功率
	read_eeprom(RAMP_RATE_CTRL_PV_RATED_POWER_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power,4);
	if((Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power <= 0)||(Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power >= 10000000))
	{
		Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power = 1200;
		write_eeprom(RAMP_RATE_CTRL_PV_RATED_POWER_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power,4);
	}
	//斜率
	read_eeprom(RAMP_RATE_CTRL_RAMP_RATIO_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio,2);
	if((Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio <= 0)||(Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio >100))
	{
		Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio = 10;
		write_eeprom(RAMP_RATE_CTRL_RAMP_RATIO_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio,2);
	}
	//斜率时间
	read_eeprom(RAMP_RATE_CTRL_RAMP_TIME_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Ramp_time,2);
	if((Time_Div_Info.Time_Div_Set_Info[0].Ramp_time <= 0)||(Time_Div_Info.Time_Div_Set_Info[0].Ramp_time > 60))
	{
		Time_Div_Info.Time_Div_Set_Info[0].Ramp_time = 1;
		write_eeprom(RAMP_RATE_CTRL_RAMP_TIME_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Ramp_time,2);
	}

	//SOC上限
	read_eeprom(RAMP_RATE_CTRL_SOC_UPLIMIT_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit,2);
	if((Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit<=0)
	 ||(Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit>1000)
	 ||(Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit<=Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit))
	{
		Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit = 500;
		write_eeprom(RAMP_RATE_CTRL_SOC_UPLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit, 2);
	}

	//SOC下限
	read_eeprom(RAMP_RATE_CTRL_SOC_LOWLIMIT_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit,2);
	if((Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit<=0)
	 ||(Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit>1000)
	 ||(Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit<=Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit))
	{
		Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit = 450;
		write_eeprom(RAMP_RATE_CTRL_SOC_LOWLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit, 2);
	}

	//Vbat上限
	read_eeprom(RAMP_RATE_CTRL_VBAT_UPLIMIT_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit,2);
	if((Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit< P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Min)
	 ||(Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit> P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Max)
	 ||(Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit<=Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit))
	{
		Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit = 6500;
		write_eeprom(RAMP_RATE_CTRL_VBAT_UPLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit, 2);
	}

	//Vbat下限
	read_eeprom(RAMP_RATE_CTRL_VBAT_LOWLIMIT_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit,2);
	if((Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit<P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Min)
	 ||(Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit>P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Max)
	 ||(Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit<=Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit))
	{
		Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit = 6000;
		write_eeprom(RAMP_RATE_CTRL_VBAT_LOWLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit, 2);
	}

	//网口下发周期
	read_eeprom(PCS_NET_SETPARA_DELAY_ADDRESS,&EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay,2);
	if((EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay < 0)||(EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay > 2000))
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay = 1500;
		write_eeprom(PCS_NET_SETPARA_DELAY_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay, 2);
	}

	read_eeprom(PCS_NET_SET_CYCLE_ADDRESS,&EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle,2);
	if((EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle < 1)||(EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle > 50))
	{
		EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle = 11;
		write_eeprom(PCS_NET_SET_CYCLE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle, 2);
	}

	//调频
	//模拟数字调频
	read_eeprom(FM_ANALOG_DIGITAL_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital,2);
	if((Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital != 0) && (Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital != 1))
	{
		Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital = 0;
		write_eeprom(FM_ANALOG_DIGITAL_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital, 2);
	}


	//模拟上限
	read_eeprom(FM_ANALOG_VMAX_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax,2);
	if((Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax >10)
     ||(Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax <= Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin))
	{
		Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax = 10;
		write_eeprom(FM_ANALOG_VMAX_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax, 2);
	}

	//模拟下限
	read_eeprom(FM_ANALOG_VMIN_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin,2);
	if((Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin < 0)
     ||(Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax <= Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin))
	{
		Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin = 0;
		write_eeprom(FM_ANALOG_VMIN_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin, 2);
	}

	//自充电功率
	read_eeprom(RAMP_RATE_CTRL_PCS_AUTOCHARGE_POWER,&Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power,2);
	if ((Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power < -(int16_t)((float)System_Info.Config.System_Power_Set * 1.1)) ||
		(Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power >  (int16_t)((float)System_Info.Config.System_Power_Set * 1.1)))
	{
		Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power = 0;
		write_eeprom(RAMP_RATE_CTRL_PCS_AUTOCHARGE_POWER, &Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power, 2);
	}


//--------------------------------------------------------------------
	//Modbus地址(上位机)
	read_eeprom(MODBUS_ADDRESS_ADDRESS, &System_Info.Modbus_Address, 1);
	if (System_Info.Modbus_Address < 1 || System_Info.Modbus_Address > 247)
	{
		System_Info.Modbus_Address = PC_RS485_COM_ADDR_DEFAULT;
		write_eeprom(MODBUS_ADDRESS_ADDRESS, &System_Info.Modbus_Address, 1);
	}

	read_eeprom(BAUD_RATE_ADDRESS, &System_Info.Baud_Rate, 1);
	if(System_Info.Baud_Rate < 0 || System_Info.Baud_Rate > 4)
	{
		System_Info.Baud_Rate = 2;
		write_eeprom(BAUD_RATE_ADDRESS, &System_Info.Baud_Rate, 1);
	}

	//触摸屏硬件版本
	(void) read_eeprom(TOUCH_HARDWARE_EDITION_ADDRESS, &Com_Data.Touch_Hardware_Edition, 1);
	if ((Com_Data.Touch_Hardware_Edition == 0) || (Com_Data.Touch_Hardware_Edition > 100))
	{
		Com_Data.Touch_Hardware_Edition = 1;
		(void) write_eeprom(TOUCH_HARDWARE_EDITION_ADDRESS, &Com_Data.Touch_Hardware_Edition, 1);
	}
	//触摸屏软件版本
	(void) read_eeprom(TOUCH_SOFTWARE_EDITION_ADDRESS, &Com_Data.Touch_Software_Edition, 1);
	if ((Com_Data.Touch_Software_Edition == 0) || (Com_Data.Touch_Software_Edition > 26))
	{
		Com_Data.Touch_Software_Edition = 1;
		(void) write_eeprom(TOUCH_SOFTWARE_EDITION_ADDRESS, &Com_Data.Touch_Software_Edition, 1);
	}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
	//DHCP	//ccg 2013.5.9
	read_eeprom(NET_DHCP_ADDRESS, &System_Info.Net.dhcp_flag, 1);
	if ((System_Info.Net.dhcp_flag != D_DHCP_OFF) && (System_Info.Net.dhcp_flag != D_DHCP_ON))
	{
		System_Info.Net.dhcp_flag = D_DHCP_OFF;
		write_eeprom(NET_DHCP_ADDRESS, &System_Info.Net.dhcp_flag, 1);
	}
	Com_Data.DHCP_State_Last = System_Info.Net.dhcp_flag;
	//端口号
	read_eeprom(NET_PORT_ADDRESS, &System_Info.Modbus_Port, 2);
	if (System_Info.Modbus_Port > UINT16_MAX || System_Info.Modbus_Port == 0 || System_Info.Modbus_Port == WEBSERVER_PORT || System_Info.Modbus_Port == IEC104_PORT)
	{
		System_Info.Modbus_Port = MODBUS_PORT;
		write_eeprom(NET_PORT_ADDRESS, &System_Info.Modbus_Port, 2);
	}
	//IP地址[0]
	read_eeprom(NET_IP_0_ADDRESS, &System_Info.Net.ip[0], 1);
	if (System_Info.Net.ip[0] > UINT8_MAX)
	{
		System_Info.Net.ip[0] = 192;
		write_eeprom(NET_IP_0_ADDRESS, &System_Info.Net.ip[0], 1);
	}
	//IP地址[1]
	read_eeprom(NET_IP_1_ADDRESS, &System_Info.Net.ip[1], 1);
	if (System_Info.Net.ip[1] > UINT8_MAX)
	{
		System_Info.Net.ip[1] = 168;
		write_eeprom(NET_IP_1_ADDRESS, &System_Info.Net.ip[1], 1);	//lixia;2013.5.13;
	}
	//IP地址[2]
	read_eeprom(NET_IP_2_ADDRESS, &System_Info.Net.ip[2], 1);
	if (System_Info.Net.ip[2] > UINT8_MAX)
	{
		System_Info.Net.ip[2] = 1;
		write_eeprom(NET_IP_2_ADDRESS, &System_Info.Net.ip[2], 1);	//lixia;2013.5.13;
	}
	//IP地址[3]
	read_eeprom(NET_IP_3_ADDRESS, &System_Info.Net.ip[3], 1);
	if (System_Info.Net.ip[3] > UINT8_MAX)
	{
		System_Info.Net.ip[3] = 100;
		write_eeprom(NET_IP_3_ADDRESS, &System_Info.Net.ip[3], 1);	//lixia;2013.5.13;
	}
	//IP地址不能是0.0.0.0
	if (System_Info.Net.ip[0] == 0 && System_Info.Net.ip[1] == 0 && System_Info.Net.ip[2] == 0 && System_Info.Net.ip[3] == 0)	//lixia;2013.5.13;
	{
		System_Info.Net.ip[0] = 192;
		System_Info.Net.ip[1] = 168;
		System_Info.Net.ip[2] = 1;
		System_Info.Net.ip[3] = 100;
		write_eeprom(NET_IP_0_ADDRESS, &System_Info.Net.ip[0], 1);
		write_eeprom(NET_IP_1_ADDRESS, &System_Info.Net.ip[1], 1);
		write_eeprom(NET_IP_2_ADDRESS, &System_Info.Net.ip[2], 1);
		write_eeprom(NET_IP_3_ADDRESS, &System_Info.Net.ip[3], 1);
	}

	//子网掩码[0]
	read_eeprom(NET_SUBMASK_0_ADDRESS, &System_Info.Net.submask[0], 1);
	if (System_Info.Net.submask[0] > UINT8_MAX)
	{
		System_Info.Net.submask[0] = 255;
		write_eeprom(NET_SUBMASK_0_ADDRESS, &System_Info.Net.submask[0], 1);
	}
	//子网掩码[1]
	read_eeprom(NET_SUBMASK_1_ADDRESS, &System_Info.Net.submask[1], 1);
	if (System_Info.Net.submask[1] > UINT8_MAX)
	{
		System_Info.Net.submask[1] = 255;
		write_eeprom(NET_SUBMASK_1_ADDRESS, &System_Info.Net.submask[1], 1);
	}
	//子网掩码[2]
	read_eeprom(NET_SUBMASK_2_ADDRESS, &System_Info.Net.submask[2], 1);
	if (System_Info.Net.submask[2] > UINT8_MAX)
	{
		System_Info.Net.submask[2] = 255;
		write_eeprom(NET_SUBMASK_2_ADDRESS, &System_Info.Net.submask[2], 1);
	}
	//子网掩码[3]
	read_eeprom(NET_SUBMASK_3_ADDRESS, &System_Info.Net.submask[3], 1);
	if (System_Info.Net.submask[3] > UINT8_MAX)
	{
		System_Info.Net.submask[3] = 0;
		write_eeprom(NET_SUBMASK_3_ADDRESS, &System_Info.Net.submask[3], 1);
	}
	//子网掩码不能是0.0.0.0
	if(System_Info.Net.submask[0]==0 && System_Info.Net.submask[1]==0 &&
	   System_Info.Net.submask[2]==0 && System_Info.Net.submask[3]==0)//lixia;2013.5.13;
	{
		System_Info.Net.submask[0] = 255;
		System_Info.Net.submask[1] = 255;
		System_Info.Net.submask[2] = 255;
		System_Info.Net.submask[3] = 0;
		write_eeprom(NET_SUBMASK_0_ADDRESS, &System_Info.Net.submask[0], 1);
		write_eeprom(NET_SUBMASK_1_ADDRESS, &System_Info.Net.submask[1], 1);
		write_eeprom(NET_SUBMASK_2_ADDRESS, &System_Info.Net.submask[2], 1);
		write_eeprom(NET_SUBMASK_3_ADDRESS, &System_Info.Net.submask[3], 1);
	}
	//网关[0]
	read_eeprom(NET_GATEWAY_0_ADDRESS, &System_Info.Net.gateway[0], 1);
	if (System_Info.Net.gateway[0] > UINT8_MAX)
	{
		System_Info.Net.gateway[0] = 192;
		write_eeprom(NET_GATEWAY_0_ADDRESS, &System_Info.Net.gateway[0], 1);
	}
	//网关[1]
	read_eeprom(NET_GATEWAY_1_ADDRESS, &System_Info.Net.gateway[1], 1);
	if (System_Info.Net.gateway[1] > UINT8_MAX)
	{
		System_Info.Net.gateway[1] = 168;
		write_eeprom(NET_GATEWAY_1_ADDRESS, &System_Info.Net.gateway[1], 1);
	}
	//网关[2]
	read_eeprom(NET_GATEWAY_2_ADDRESS, &System_Info.Net.gateway[2], 1);
	if (System_Info.Net.gateway[2] > UINT8_MAX)
	{
		System_Info.Net.gateway[2] = 1;
		write_eeprom(NET_GATEWAY_2_ADDRESS, &System_Info.Net.gateway[2], 1);
	}
	//网关[3]
	(void) read_eeprom(NET_GATEWAY_3_ADDRESS, &System_Info.Net.gateway[3], 1);
	if (System_Info.Net.gateway[3] > UINT8_MAX)
	{
		System_Info.Net.gateway[3] = 1;
		write_eeprom(NET_GATEWAY_3_ADDRESS, &System_Info.Net.gateway[3], 1);
	}
	//网关地址不能是0.0.0.0
	if(System_Info.Net.gateway[0]==0 && System_Info.Net.gateway[1]==0 &&
	   System_Info.Net.gateway[2]==0 && System_Info.Net.gateway[3]==0)//lixia;2013.5.13;
	{
		System_Info.Net.gateway[0] = 192;
		System_Info.Net.gateway[1] = 168;
		System_Info.Net.gateway[2] = 1;
		System_Info.Net.gateway[3] = 1;
		write_eeprom(NET_GATEWAY_0_ADDRESS, &System_Info.Net.gateway[0], 1);
		write_eeprom(NET_GATEWAY_1_ADDRESS, &System_Info.Net.gateway[1], 1);
		write_eeprom(NET_GATEWAY_2_ADDRESS, &System_Info.Net.gateway[2], 1);
		write_eeprom(NET_GATEWAY_3_ADDRESS, &System_Info.Net.gateway[3], 1);
	}
	//DNS1[0]
	read_eeprom(NET_DNS1_0_ADDRESS, &System_Info.Net.dns1[0], 1);
	if (System_Info.Net.dns1[0] > UINT8_MAX)
	{
		System_Info.Net.dns1[0] = 1;
		write_eeprom(NET_DNS1_0_ADDRESS, &System_Info.Net.dns1[0], 1);
	}
	//DNS1[1]
	read_eeprom(NET_DNS1_1_ADDRESS, &System_Info.Net.dns1[1], 1);
	if (System_Info.Net.dns1[1] > UINT8_MAX)
	{
		System_Info.Net.dns1[1] = 2;
		write_eeprom(NET_DNS1_1_ADDRESS, &System_Info.Net.dns1[1], 1);
	}
	//DNS1[2]
	read_eeprom(NET_DNS1_2_ADDRESS, &System_Info.Net.dns1[2], 1);
	if (System_Info.Net.dns1[2] > UINT8_MAX)
	{
		System_Info.Net.dns1[2] = 4;
		write_eeprom(NET_DNS1_2_ADDRESS, &System_Info.Net.dns1[2], 1);
	}
	//DNS1[3]
	read_eeprom(NET_DNS1_3_ADDRESS, &System_Info.Net.dns1[3], 1);
	if (System_Info.Net.dns1[3] > UINT8_MAX)
	{
		System_Info.Net.dns1[3] = 8;
		write_eeprom(NET_DNS1_3_ADDRESS, &System_Info.Net.dns1[3], 1);
	}
	//DNS不能是0.0.0.0
	if(System_Info.Net.dns1[0]==0 && System_Info.Net.dns1[1]==0 &&
	   System_Info.Net.dns1[2]==0 && System_Info.Net.dns1[3]==0)//lixia;2013.5.13;
	{
		System_Info.Net.dns1[0] = 1;
		System_Info.Net.dns1[1] = 2;
		System_Info.Net.dns1[2] = 4;
		System_Info.Net.dns1[3] = 8;
		write_eeprom(NET_DNS1_0_ADDRESS, &System_Info.Net.dns1[0], 1);
		write_eeprom(NET_DNS1_1_ADDRESS, &System_Info.Net.dns1[1], 1);
		write_eeprom(NET_DNS1_2_ADDRESS, &System_Info.Net.dns1[2], 1);
		write_eeprom(NET_DNS1_3_ADDRESS, &System_Info.Net.dns1[3], 1);
	}
	//DNS2[0]
	read_eeprom(NET_DNS2_0_ADDRESS, &System_Info.Net.dns2[0], 1);
	if (System_Info.Net.dns2[0] > UINT8_MAX)
	{
		System_Info.Net.dns2[0] = 8;
		write_eeprom(NET_DNS2_0_ADDRESS, &System_Info.Net.dns2[0], 1);
	}
	//DNS2[1]
	read_eeprom(NET_DNS2_1_ADDRESS, &System_Info.Net.dns2[1], 1);
	if (System_Info.Net.dns2[1] > UINT8_MAX)
	{
		System_Info.Net.dns2[1] = 8;
		write_eeprom(NET_DNS2_1_ADDRESS, &System_Info.Net.dns2[1], 1);
	}
	//DNS2[2]
	read_eeprom(NET_DNS2_2_ADDRESS, &System_Info.Net.dns2[2], 1);
	if (System_Info.Net.dns2[2] > UINT8_MAX)
	{
		System_Info.Net.dns2[2] = 8;
		write_eeprom(NET_DNS2_2_ADDRESS, &System_Info.Net.dns2[2], 1);
	}
	//DNS2[3]
	read_eeprom(NET_DNS2_3_ADDRESS, &System_Info.Net.dns2[3], 1);
	if (System_Info.Net.dns2[3] > UINT8_MAX)
	{
		System_Info.Net.dns2[3] = 8;
		write_eeprom(NET_DNS2_3_ADDRESS, &System_Info.Net.dns2[3], 1);
	}
	//DNS不能是0.0.0.0
	if(System_Info.Net.dns2[0]==0 && System_Info.Net.dns2[1]==0 &&
	   System_Info.Net.dns2[2]==0 && System_Info.Net.dns2[3]==0)//lixia;2013.5.13;
	{
		System_Info.Net.dns2[0] = 8;
		System_Info.Net.dns2[1] = 8;
		System_Info.Net.dns2[2] = 8;
		System_Info.Net.dns2[3] = 8;
		write_eeprom(NET_DNS2_0_ADDRESS, &System_Info.Net.dns2[0], 1);
		write_eeprom(NET_DNS2_1_ADDRESS, &System_Info.Net.dns2[1], 1);
		write_eeprom(NET_DNS2_2_ADDRESS, &System_Info.Net.dns2[2], 1);
		write_eeprom(NET_DNS2_3_ADDRESS, &System_Info.Net.dns2[3], 1);
	}

	//IP地址[0]
	read_eeprom(NET2_IP_0_ADDRESS, &System_Info.Net2.ip[0], 1);
	if (System_Info.Net2.ip[0] > UINT8_MAX)
	{
		System_Info.Net2.ip[0] = 192;
		write_eeprom(NET2_IP_0_ADDRESS, &System_Info.Net2.ip[0], 1);
	}
	//IP地址[1]
	read_eeprom(NET2_IP_1_ADDRESS, &System_Info.Net2.ip[1], 1);
	if (System_Info.Net2.ip[1] > UINT8_MAX)
	{
		System_Info.Net2.ip[1] = 168;
		write_eeprom(NET2_IP_1_ADDRESS, &System_Info.Net2.ip[1], 1);
	}
	//IP地址[2]
	read_eeprom(NET2_IP_2_ADDRESS, &System_Info.Net2.ip[2], 1);
	if (System_Info.Net2.ip[2] > UINT8_MAX)
	{
		System_Info.Net2.ip[2] = 0;
		write_eeprom(NET2_IP_2_ADDRESS, &System_Info.Net2.ip[2], 1);
	}
	//IP地址[3]
	read_eeprom(NET2_IP_3_ADDRESS, &System_Info.Net2.ip[3], 1);
	if (System_Info.Net2.ip[3] > UINT8_MAX)
	{
		System_Info.Net2.ip[3] = 100;
		write_eeprom(NET2_IP_3_ADDRESS, &System_Info.Net2.ip[3], 1);
	}
	//IP地址不能是0.0.0.0
	if (System_Info.Net2.ip[0] == 0 && System_Info.Net2.ip[1] == 0 && System_Info.Net2.ip[2] == 0 && System_Info.Net2.ip[3] == 0)
	{
		System_Info.Net2.ip[0] = 192;
		System_Info.Net2.ip[1] = 168;
		System_Info.Net2.ip[2] = 0;
		System_Info.Net2.ip[3] = 100;
		write_eeprom(NET2_IP_0_ADDRESS, &System_Info.Net2.ip[0], 1);
		write_eeprom(NET2_IP_1_ADDRESS, &System_Info.Net2.ip[1], 1);
		write_eeprom(NET2_IP_2_ADDRESS, &System_Info.Net2.ip[2], 1);
		write_eeprom(NET2_IP_3_ADDRESS, &System_Info.Net2.ip[3], 1);
	}
	//子网掩码[0]
	read_eeprom(NET2_SUBMASK_0_ADDRESS, &System_Info.Net2.submask[0], 1);
	if (System_Info.Net2.submask[0] > UINT8_MAX)
	{
		System_Info.Net2.submask[0] = 255;
		write_eeprom(NET2_SUBMASK_0_ADDRESS, &System_Info.Net2.submask[0], 1);
	}
	//子网掩码[1]
	read_eeprom(NET2_SUBMASK_1_ADDRESS, &System_Info.Net2.submask[1], 1);
	if (System_Info.Net2.submask[1] > UINT8_MAX)
	{
		System_Info.Net2.submask[1] = 255;
		write_eeprom(NET2_SUBMASK_1_ADDRESS, &System_Info.Net2.submask[1], 1);
	}
	//子网掩码[2]
	read_eeprom(NET2_SUBMASK_2_ADDRESS, &System_Info.Net2.submask[2], 1);
	if (System_Info.Net2.submask[2] > UINT8_MAX)
	{
		System_Info.Net2.submask[2] = 255;
		write_eeprom(NET2_SUBMASK_2_ADDRESS, &System_Info.Net2.submask[2], 1);
	}
	//子网掩码[3]
	read_eeprom(NET2_SUBMASK_3_ADDRESS, &System_Info.Net2.submask[3], 1);
	if (System_Info.Net2.submask[3] > UINT8_MAX)
	{
		System_Info.Net2.submask[3] = 0;
		write_eeprom(NET2_SUBMASK_3_ADDRESS, &System_Info.Net2.submask[3], 1);
	}
	//子网掩码不能是0.0.0.0
	if(System_Info.Net2.submask[0]==0 && System_Info.Net2.submask[1]==0 &&
	   System_Info.Net2.submask[2]==0 && System_Info.Net2.submask[3]==0)//lixia;2013.5.13;
	{
		System_Info.Net2.submask[0] = 255;
		System_Info.Net2.submask[1] = 255;
		System_Info.Net2.submask[2] = 255;
		System_Info.Net2.submask[3] = 0;
		write_eeprom(NET2_SUBMASK_0_ADDRESS, &System_Info.Net2.submask[0], 1);
		write_eeprom(NET2_SUBMASK_1_ADDRESS, &System_Info.Net2.submask[1], 1);
		write_eeprom(NET2_SUBMASK_2_ADDRESS, &System_Info.Net2.submask[2], 1);
		write_eeprom(NET2_SUBMASK_3_ADDRESS, &System_Info.Net2.submask[3], 1);
	}
	//网关[0]
	read_eeprom(NET2_GATEWAY_0_ADDRESS, &System_Info.Net2.gateway[0], 1);
	if (System_Info.Net2.gateway[0] > UINT8_MAX)
	{
		System_Info.Net2.gateway[0] = 192;
		write_eeprom(NET2_GATEWAY_0_ADDRESS, &System_Info.Net2.gateway[0], 1);
	}
	//网关[1]
	read_eeprom(NET2_GATEWAY_1_ADDRESS, &System_Info.Net2.gateway[1], 1);
	if (System_Info.Net2.gateway[1] > UINT8_MAX)
	{
		System_Info.Net2.gateway[1] = 168;
		write_eeprom(NET2_GATEWAY_1_ADDRESS, &System_Info.Net2.gateway[1], 1);
	}
	//网关[2]
	read_eeprom(NET2_GATEWAY_2_ADDRESS, &System_Info.Net2.gateway[2], 1);
	if (System_Info.Net2.gateway[2] > UINT8_MAX)
	{
		System_Info.Net2.gateway[2] = 0;
		write_eeprom(NET2_GATEWAY_2_ADDRESS, &System_Info.Net2.gateway[2], 1);
	}
	//网关[3]
	(void) read_eeprom(NET2_GATEWAY_3_ADDRESS, &System_Info.Net2.gateway[3], 1);
	if (System_Info.Net2.gateway[3] > UINT8_MAX)
	{
		System_Info.Net2.gateway[3] = 1;
		write_eeprom(NET2_GATEWAY_3_ADDRESS, &System_Info.Net2.gateway[3], 1);
	}
	//网关地址不能是0.0.0.0
	if(System_Info.Net2.gateway[0]==0 && System_Info.Net2.gateway[1]==0 &&
	   System_Info.Net2.gateway[2]==0 && System_Info.Net2.gateway[3]==0)//lixia;2013.5.13;
	{
		System_Info.Net2.gateway[0] = 192;
		System_Info.Net2.gateway[1] = 168;
		System_Info.Net2.gateway[2] = 0;
		System_Info.Net2.gateway[3] = 1;
		write_eeprom(NET2_GATEWAY_0_ADDRESS, &System_Info.Net2.gateway[0], 1);
		write_eeprom(NET2_GATEWAY_1_ADDRESS, &System_Info.Net2.gateway[1], 1);
		write_eeprom(NET2_GATEWAY_2_ADDRESS, &System_Info.Net2.gateway[2], 1);
		write_eeprom(NET2_GATEWAY_3_ADDRESS, &System_Info.Net2.gateway[3], 1);
	}

	//DNS1[0]
	read_eeprom(NET2_DNS1_0_ADDRESS, &System_Info.Net2.dns1[0], 1);
	if (System_Info.Net2.dns1[0] > UINT8_MAX)
	{
		System_Info.Net2.dns1[0] = 1;
		write_eeprom(NET2_DNS1_0_ADDRESS, &System_Info.Net2.dns1[0], 1);
	}
	//DNS1[1]
	read_eeprom(NET2_DNS1_1_ADDRESS, &System_Info.Net2.dns1[1], 1);
	if (System_Info.Net2.dns1[1] > UINT8_MAX)
	{
		System_Info.Net2.dns1[1] = 2;
		write_eeprom(NET2_DNS1_1_ADDRESS, &System_Info.Net2.dns1[1], 1);
	}
	//DNS1[2]
	read_eeprom(NET2_DNS1_2_ADDRESS, &System_Info.Net2.dns1[2], 1);
	if (System_Info.Net2.dns1[2] > UINT8_MAX)
	{
		System_Info.Net2.dns1[2] = 4;
		write_eeprom(NET2_DNS1_2_ADDRESS, &System_Info.Net2.dns1[2], 1);
	}
	//DNS1[3]
	read_eeprom(NET2_DNS1_3_ADDRESS, &System_Info.Net2.dns1[3], 1);
	if (System_Info.Net2.dns1[3] > UINT8_MAX)
	{
		System_Info.Net2.dns1[3] = 8;
		write_eeprom(NET2_DNS1_3_ADDRESS, &System_Info.Net2.dns1[3], 1);
	}
	//DNS不能是0.0.0.0
	if(System_Info.Net2.dns1[0]==0 && System_Info.Net2.dns1[1]==0 &&
	   System_Info.Net2.dns1[2]==0 && System_Info.Net2.dns1[3]==0)//lixia;2013.5.13;
	{
		System_Info.Net2.dns1[0] = 1;
		System_Info.Net2.dns1[1] = 2;
		System_Info.Net2.dns1[2] = 4;
		System_Info.Net2.dns1[3] = 8;
		write_eeprom(NET2_DNS1_0_ADDRESS, &System_Info.Net2.dns1[0], 1);
		write_eeprom(NET2_DNS1_1_ADDRESS, &System_Info.Net2.dns1[1], 1);
		write_eeprom(NET2_DNS1_2_ADDRESS, &System_Info.Net2.dns1[2], 1);
		write_eeprom(NET2_DNS1_3_ADDRESS, &System_Info.Net2.dns1[3], 1);
	}
	//DNS2[0]
	read_eeprom(NET2_DNS2_0_ADDRESS, &System_Info.Net2.dns2[0], 1);
	if (System_Info.Net2.dns2[0] > UINT8_MAX)
	{
		System_Info.Net2.dns2[0] = 8;
		write_eeprom(NET2_DNS2_0_ADDRESS, &System_Info.Net2.dns2[0], 1);
	}
	//DNS2[1]
	read_eeprom(NET2_DNS2_1_ADDRESS, &System_Info.Net2.dns2[1], 1);
	if (System_Info.Net2.dns2[1] > UINT8_MAX)
	{
		System_Info.Net2.dns2[1] = 8;
		write_eeprom(NET2_DNS2_1_ADDRESS, &System_Info.Net2.dns2[1], 1);
	}
	//DNS2[2]
	read_eeprom(NET2_DNS2_2_ADDRESS, &System_Info.Net2.dns2[2], 1);
	if (System_Info.Net2.dns2[2] > UINT8_MAX)
	{
		System_Info.Net2.dns2[2] = 8;
		write_eeprom(NET2_DNS2_2_ADDRESS, &System_Info.Net2.dns2[2], 1);
	}
	//DNS2[3]
	read_eeprom(NET2_DNS2_3_ADDRESS, &System_Info.Net2.dns2[3], 1);
	if (System_Info.Net2.dns2[3] > UINT8_MAX)
	{
		System_Info.Net2.dns2[3] = 8;
		write_eeprom(NET2_DNS2_3_ADDRESS, &System_Info.Net2.dns2[3], 1);
	}
	//DNS不能是0.0.0.0
	if(System_Info.Net2.dns2[0]==0 && System_Info.Net2.dns2[1]==0 &&
	   System_Info.Net2.dns2[2]==0 && System_Info.Net2.dns2[3]==0)//lixia;2013.5.13;
	{
		System_Info.Net2.dns2[0] = 8;
		System_Info.Net2.dns2[1] = 8;
		System_Info.Net2.dns2[2] = 8;
		System_Info.Net2.dns2[3] = 8;
		write_eeprom(NET2_DNS2_0_ADDRESS, &System_Info.Net2.dns2[0], 1);
		write_eeprom(NET2_DNS2_1_ADDRESS, &System_Info.Net2.dns2[1], 1);
		write_eeprom(NET2_DNS2_2_ADDRESS, &System_Info.Net2.dns2[2], 1);
		write_eeprom(NET2_DNS2_3_ADDRESS, &System_Info.Net2.dns2[3], 1);
	}

	///NTP参数
	//IP地址[0]
	read_eeprom(NTP_IP_0_ADDRESS, &System_Info.NTP.ip[0], 1);
	if (System_Info.NTP.ip[0] > UINT8_MAX)
	{
		System_Info.NTP.ip[0] = 5;
		write_eeprom(NTP_IP_0_ADDRESS, &System_Info.NTP.ip[0], 1);
	}
	//IP地址[1]
	read_eeprom(NTP_IP_1_ADDRESS, &System_Info.NTP.ip[1], 1);
	if (System_Info.NTP.ip[1] > UINT8_MAX)
	{
		System_Info.NTP.ip[1] = 79;
		write_eeprom(NTP_IP_1_ADDRESS, &System_Info.NTP.ip[1], 1);
	}
	//IP地址[2]
	read_eeprom(NTP_IP_2_ADDRESS, &System_Info.NTP.ip[2], 1);
	if (System_Info.NTP.ip[2] > UINT8_MAX)
	{
		System_Info.NTP.ip[2] = 108;
		write_eeprom(NTP_IP_2_ADDRESS, &System_Info.NTP.ip[2], 1);
	}
	//IP地址[3]
	read_eeprom(NTP_IP_3_ADDRESS, &System_Info.NTP.ip[3], 1);
	if (System_Info.NTP.ip[3] > UINT8_MAX)
	{
		System_Info.NTP.ip[3] = 34;
		write_eeprom(NTP_IP_3_ADDRESS, &System_Info.NTP.ip[3], 1);
	}
	//IP地址不能是0.0.0.0
	if (System_Info.NTP.ip[0] == 0 && System_Info.NTP.ip[1] == 0 && System_Info.NTP.ip[2] == 0 && System_Info.NTP.ip[3] == 0)
	{
		System_Info.NTP.ip[0] = 5;
		System_Info.NTP.ip[1] = 79;
		System_Info.NTP.ip[2] = 108;
		System_Info.NTP.ip[3] = 34;
		write_eeprom(NTP_IP_0_ADDRESS, &System_Info.NTP.ip[0], 1);
		write_eeprom(NTP_IP_1_ADDRESS, &System_Info.NTP.ip[1], 1);
		write_eeprom(NTP_IP_2_ADDRESS, &System_Info.NTP.ip[2], 1);
		write_eeprom(NTP_IP_3_ADDRESS, &System_Info.NTP.ip[3], 1);
	}

	read_eeprom(NTP_PORT_ADDRESS, &System_Info.NTP.port, 2);
	if (System_Info.NTP.port > UINT16_MAX)
	{
		System_Info.NTP.port = 123;
		write_eeprom(NTP_PORT_ADDRESS, &System_Info.NTP.port, 2);
	}

	read_eeprom(NTP_ENABLE_ADDRESS, &System_Info.NTP.enable, 1);
	if(System_Info.NTP.enable!=0x55&&System_Info.NTP.enable!=0xAA)
	{
		System_Info.NTP.enable = 0xAA;
		write_eeprom(NTP_ENABLE_ADDRESS, &System_Info.NTP.enable, 1);
	}

	//目标服务器参数
	//服务器0
	//IP地址[0]
	for(int i = 0; i < PCS_NUM; i++)
	{
		read_eeprom(SERVER_0_IP_0_ADDRESS+i*4, &System_Info.Server[i].ip[0], 1);
		if (System_Info.Server[i].ip[0] > UINT8_MAX)
		{
			System_Info.Server[i].ip[0] = 192;
			write_eeprom(SERVER_0_IP_0_ADDRESS+i*4, &System_Info.Server[i].ip[0], 1);
		}
		//IP地址[1]
		read_eeprom(SERVER_0_IP_0_ADDRESS+i*4+1, &System_Info.Server[i].ip[1], 1);
		if (System_Info.Server[i].ip[1] > UINT8_MAX)
		{
			System_Info.Server[i].ip[1] = 168;
			write_eeprom(SERVER_0_IP_0_ADDRESS+i*4+1, &System_Info.Server[i].ip[1], 1);
		}
		//IP地址[2]
		read_eeprom(SERVER_0_IP_0_ADDRESS+i*4+2, &System_Info.Server[i].ip[2], 1);
		if (System_Info.Server[i].ip[2] > UINT8_MAX)
		{
			System_Info.Server[i].ip[2] = 0;
			write_eeprom(SERVER_0_IP_0_ADDRESS+i*4+2, &System_Info.Server[i].ip[2], 1);
		}
		//IP地址[3]
		read_eeprom(SERVER_0_IP_0_ADDRESS+i*4+3, &System_Info.Server[i].ip[3], 1);
		if (System_Info.Server[i].ip[3] > UINT8_MAX)
		{
			System_Info.Server[i].ip[3] = 1+i;
			write_eeprom(SERVER_0_IP_0_ADDRESS+i*4+3, &System_Info.Server[i].ip[3], 1);
		}
		//端口号
		read_eeprom(SERVER_0_PORT_ADDRESS+i*2, &System_Info.Server_Modbus_Port[i], 2);
		if (System_Info.Server_Modbus_Port[i] > UINT16_MAX || System_Info.Server_Modbus_Port[i] == 0||
		System_Info.Server_Modbus_Port[i] == WEBSERVER_PORT ||
		System_Info.Server_Modbus_Port[i] == IEC104_PORT)
		{
			System_Info.Server_Modbus_Port[i] = 502;
			write_eeprom(SERVER_0_PORT_ADDRESS+i*2, &System_Info.Server_Modbus_Port[i], 2);
		}
		//IP地址不能是0.0.0.0
		if (System_Info.Server[i].ip[0] == 0 && System_Info.Server[i].ip[1] == 0 && System_Info.Server[i].ip[2] == 0 && System_Info.Server[i].ip[3] == 0)
		{
			System_Info.Server[i].ip[0] = 192;
			System_Info.Server[i].ip[1] = 168;
			System_Info.Server[i].ip[2] = 0;
			System_Info.Server[i].ip[3] = 1+i;
			write_eeprom(SERVER_0_IP_0_ADDRESS+i*4, &System_Info.Server[i].ip[0], 1);
			write_eeprom(SERVER_0_IP_0_ADDRESS+i*4+1, &System_Info.Server[i].ip[1], 1);
			write_eeprom(SERVER_0_IP_0_ADDRESS+i*4+2, &System_Info.Server[i].ip[2], 1);
			write_eeprom(SERVER_0_IP_0_ADDRESS+i*4+3, &System_Info.Server[i].ip[3], 1);
		}
	}

	for(int i = 0; i < PV_NUM; i++)
	{
		read_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4, &System_Info.PVServer[i].ip[0], 1);
		if (System_Info.PVServer[i].ip[0] > UINT8_MAX)
		{
			System_Info.PVServer[i].ip[0] = 192;
			write_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4, &System_Info.PVServer[i].ip[0], 1);
		}
		//IP地址[1]
		read_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4+1, &System_Info.PVServer[i].ip[1], 1);
		if (System_Info.PVServer[i].ip[1] > UINT8_MAX)
		{
			System_Info.PVServer[i].ip[1] = 168;
			write_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4+1, &System_Info.PVServer[i].ip[1], 1);
		}
		//IP地址[2]
		read_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4+2, &System_Info.PVServer[i].ip[2], 1);
		if (System_Info.PVServer[i].ip[2] > UINT8_MAX)
		{
			System_Info.PVServer[i].ip[2] = 0;
			write_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4+2, &System_Info.PVServer[i].ip[2], 1);
		}
		//IP地址[3]
		read_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4+3, &System_Info.PVServer[i].ip[3], 1);
		if (System_Info.PVServer[i].ip[3] > UINT8_MAX)
		{
			System_Info.PVServer[i].ip[3] = 101+i;
			write_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4+3, &System_Info.PVServer[i].ip[3], 1);
		}
		//端口号
		read_eeprom(SERVER_PV_0_PORT_ADDRESS+i*2, &System_Info.PVServer_Modbus_Port[i], 2);
		if (System_Info.PVServer_Modbus_Port[i] > UINT16_MAX || System_Info.PVServer_Modbus_Port[i] == 0||
		System_Info.PVServer_Modbus_Port[i] == WEBSERVER_PORT ||
		System_Info.PVServer_Modbus_Port[i] == IEC104_PORT)
		{
			System_Info.PVServer_Modbus_Port[i] = 502;
			write_eeprom(SERVER_PV_0_PORT_ADDRESS+i*2, &System_Info.PVServer_Modbus_Port[i], 2);
		}
		//IP地址不能是0.0.0.0
		if (System_Info.PVServer[i].ip[0] == 0 && System_Info.PVServer[i].ip[1] == 0 && System_Info.PVServer[i].ip[2] == 0 && System_Info.PVServer[i].ip[3] == 0)
		{
			System_Info.PVServer[i].ip[0] = 192;
			System_Info.PVServer[i].ip[1] = 168;
			System_Info.PVServer[i].ip[2] = 0;
			System_Info.PVServer[i].ip[3] = 101+i;
			write_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4, &System_Info.PVServer[i].ip[0], 1);
			write_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4+1, &System_Info.PVServer[i].ip[1], 1);
			write_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4+2, &System_Info.PVServer[i].ip[2], 1);
			write_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4+3, &System_Info.PVServer[i].ip[3], 1);
		}
	}

	//IAT
	//MAC地址锁定
	IAT_Info.Set_Info.MAC_Lock = 1;
	//read_eeprom(IAT_MAC_LOCK_ADDRESS, &IAT_Info.Set_Info.MAC_Lock, 2);
	if (IAT_Info.Set_Info.MAC_Lock != 1)
	{
		IAT_Info.Set_Info.MAC_Lock = 1;
		write_eeprom(IAT_MAC_LOCK_ADDRESS, &IAT_Info.Set_Info.MAC_Lock, 2);
	}

	//MAC地址 ccg 2013.5.11
	/*read_eeprom(IAT_MAC_ADDRESS + 0, &IAT_Info.Set_Info.MAC[0], 2);
	read_eeprom(IAT_MAC_ADDRESS + 2, &IAT_Info.Set_Info.MAC[1], 2);
	read_eeprom(IAT_MAC_ADDRESS + 4, &IAT_Info.Set_Info.MAC[2], 2);

	System_Info.Net.mac[0] = HIBYTE(IAT_Info.Set_Info.MAC[0]);
	System_Info.Net.mac[1] = LOBYTE(IAT_Info.Set_Info.MAC[0]);
	System_Info.Net.mac[2] = HIBYTE(IAT_Info.Set_Info.MAC[1]);
	System_Info.Net.mac[3] = LOBYTE(IAT_Info.Set_Info.MAC[1]);
	System_Info.Net.mac[4] = HIBYTE(IAT_Info.Set_Info.MAC[2]);
	System_Info.Net.mac[5] = LOBYTE(IAT_Info.Set_Info.MAC[2]);

	if (!Mac_Valid(System_Info.Net.mac))
	{
		IAT_Info.Set_Info.MAC_Lock = 1;
		write_eeprom(IAT_MAC_LOCK_ADDRESS, &IAT_Info.Set_Info.MAC_Lock, 2);

		IAT_Info.Set_Info.MAC[0] = 0xAC19;
		IAT_Info.Set_Info.MAC[1] = 0x9F10;
		IAT_Info.Set_Info.MAC[2] = 0x0000;

		System_Info.Net.mac[0] = HIBYTE(IAT_Info.Set_Info.MAC[0]);
		System_Info.Net.mac[1] = LOBYTE(IAT_Info.Set_Info.MAC[0]);
		System_Info.Net.mac[2] = HIBYTE(IAT_Info.Set_Info.MAC[1]);
		System_Info.Net.mac[3] = LOBYTE(IAT_Info.Set_Info.MAC[1]);
		System_Info.Net.mac[4] = HIBYTE(IAT_Info.Set_Info.MAC[2]);
		System_Info.Net.mac[5] = LOBYTE(IAT_Info.Set_Info.MAC[2]);

		write_eeprom(IAT_MAC_ADDRESS + 0, &IAT_Info.Set_Info.MAC[0], 2);
		write_eeprom(IAT_MAC_ADDRESS + 2, &IAT_Info.Set_Info.MAC[1], 2);
		write_eeprom(IAT_MAC_ADDRESS + 4, &IAT_Info.Set_Info.MAC[2], 2);
	}

	read_eeprom(IAT_MAC2_ADDRESS + 0, &IAT_Info.Set_Info.MAC2[0], 2);
	read_eeprom(IAT_MAC2_ADDRESS + 2, &IAT_Info.Set_Info.MAC2[1], 2);
	read_eeprom(IAT_MAC2_ADDRESS + 4, &IAT_Info.Set_Info.MAC2[2], 2);

	System_Info.Net2.mac[0] = HIBYTE(IAT_Info.Set_Info.MAC2[0]);
	System_Info.Net2.mac[1] = LOBYTE(IAT_Info.Set_Info.MAC2[0]);
	System_Info.Net2.mac[2] = HIBYTE(IAT_Info.Set_Info.MAC2[1]);
	System_Info.Net2.mac[3] = LOBYTE(IAT_Info.Set_Info.MAC2[1]);
	System_Info.Net2.mac[4] = HIBYTE(IAT_Info.Set_Info.MAC2[2]);
	System_Info.Net2.mac[5] = LOBYTE(IAT_Info.Set_Info.MAC2[2]);

	if (!Mac_Valid(System_Info.Net2.mac))
	{
		IAT_Info.Set_Info.MAC_Lock = 1;
		write_eeprom(IAT_MAC_LOCK_ADDRESS, &IAT_Info.Set_Info.MAC_Lock, 2);

		IAT_Info.Set_Info.MAC2[0] = 0xAC19;
		IAT_Info.Set_Info.MAC2[1] = 0x9F00;
		IAT_Info.Set_Info.MAC2[2] = 0x0000;

		System_Info.Net2.mac[0] = HIBYTE(IAT_Info.Set_Info.MAC2[0]);
		System_Info.Net2.mac[1] = LOBYTE(IAT_Info.Set_Info.MAC2[0]);
		System_Info.Net2.mac[2] = HIBYTE(IAT_Info.Set_Info.MAC2[1]);
		System_Info.Net2.mac[3] = LOBYTE(IAT_Info.Set_Info.MAC2[1]);
		System_Info.Net2.mac[4] = HIBYTE(IAT_Info.Set_Info.MAC2[2]);
		System_Info.Net2.mac[5] = LOBYTE(IAT_Info.Set_Info.MAC2[2]);

		write_eeprom(IAT_MAC2_ADDRESS + 0, &IAT_Info.Set_Info.MAC2[0], 2);
		write_eeprom(IAT_MAC2_ADDRESS + 2, &IAT_Info.Set_Info.MAC2[1], 2);
		write_eeprom(IAT_MAC2_ADDRESS + 4, &IAT_Info.Set_Info.MAC2[2], 2);
	}*/

	//获取eth0，eth1网卡MAC
	Get_Local_Mac(&System_Info.Net, (char*)"eth0");
	Get_Local_Mac(&System_Info.Net2, (char*)"eth1");
	IAT_Info.Set_Info.MAC[0] = (System_Info.Net.mac[0]<<8) | System_Info.Net.mac[1];
	IAT_Info.Set_Info.MAC[1] = (System_Info.Net.mac[2]<<8) | System_Info.Net.mac[3];
	IAT_Info.Set_Info.MAC[2] = (System_Info.Net.mac[4]<<8) | System_Info.Net.mac[5];

	//序列号使能开关  ccg 2013.8.8
	read_eeprom(IAT_SN_ENABLE_ADDRESS, &IAT_Info.Set_Info.SN_Enable, 2);
	if (IAT_Info.Set_Info.SN_Enable != SN_ENABLE && IAT_Info.Set_Info.SN_Enable != SN_DISABLE)
	{
		IAT_Info.Set_Info.SN_Enable = SN_ENABLE;
		write_eeprom(IAT_SN_ENABLE_ADDRESS, &IAT_Info.Set_Info.SN_Enable, 2);
		memset(IAT_Info.Set_Info.SN_Number, 0, 20);
	}
	//SN序列号 ccg 2013.5.23
	(void) read_eeprom(IAT_SN_NUMBER_ADDRESS, &IAT_Info.Set_Info.SN_Number, 20);
	if (Check_Sn(IAT_Info.Set_Info.SN_Number, Sn_Num(IAT_Info.Set_Info.SN_Number, sizeof(IAT_Info.Set_Info.SN_Number))) == false)    //ccg 2013.8.6
	{
		memset(IAT_Info.Set_Info.SN_Number, 0, 20);
		write_eeprom(IAT_SN_NUMBER_ADDRESS, &IAT_Info.Set_Info.SN_Number, 20);
		IAT_Info.Set_Info.SN_Enable = SN_ENABLE;
		write_eeprom(IAT_SN_ENABLE_ADDRESS, &IAT_Info.Set_Info.SN_Enable, 2);
	}
	//IEC104
	read_eeprom(IEC104_ADDRESS_ADDRESS, &System_Info.IEC104_Address, 2);
	if (System_Info.IEC104_Address < 1 || System_Info.IEC104_Address > 65535)
	{
		System_Info.IEC104_Address = 1;
		write_eeprom(IEC104_ADDRESS_ADDRESS, &System_Info.IEC104_Address, 2);
	}
	//IEC104
	read_eeprom(IEC104_T1_ADDRESS, &System_Info.IEC104_T1, 2);
	//IEC104
	read_eeprom(IEC104_T3_ADDRESS, &System_Info.IEC104_T3, 2);
	if (System_Info.IEC104_T1 < 1 || System_Info.IEC104_T3 > 255 || System_Info.IEC104_T1 > System_Info.IEC104_T3)
	{
		System_Info.IEC104_T1 = 20;
		write_eeprom(IEC104_T1_ADDRESS, &System_Info.IEC104_T1, 2);
		System_Info.IEC104_T3 = 25;
		write_eeprom(IEC104_T3_ADDRESS, &System_Info.IEC104_T3, 2);
	}
	//IEC104
	read_eeprom(IEC104_K_ADDRESS, &System_Info.IEC104_K, 2);
	//IEC104
	read_eeprom(IEC104_W_ADDRESS, &System_Info.IEC104_W, 2);
	if (System_Info.IEC104_W < 1 || System_Info.IEC104_K > 32767 || System_Info.IEC104_W > ((uint16_t) System_Info.IEC104_K * 2 / 3))
	{
		System_Info.IEC104_K = 8;
		write_eeprom(IEC104_K_ADDRESS, &System_Info.IEC104_K, 2);
		System_Info.IEC104_W = 5;
		write_eeprom(IEC104_W_ADDRESS, &System_Info.IEC104_W, 2);
	}

	//webserver
	//页面语言
	/*read_eeprom(WEBSERVER_LANGUAGE_ADDRESS, &Webserver_Language, 1);
	 if(Webserver_Language != CHINESE && Webserver_Language != ENGLISH)
	 {
	 Webserver_Language = CHINESE;
	 write_eeprom(WEBSERVER_LANGUAGE_ADDRESS, &Webserver_Language, 1);
	 }*/
}

//恢复默认值
void Load_Default_Process(void)
{
	//历史复位
	write_reset_record.ptr = 0;  //事件记录指针
	write_reset_record.total_nub = 0;  //一共有多少条历史事件记录
	(void) write_eeprom(HISRESETRECORD_PTR_ADDRESS, &write_reset_record.ptr, 2);
	(void) write_eeprom(HISRESETRECORD_NB_ADDRESS, &write_reset_record.total_nub, 2);
	read_reset_record.total_nub = write_reset_record.total_nub;  //显示一共有多少条历史事件记录
	read_reset_record.ptr = write_reset_record.ptr;  //读事件记录指针
	read_reset_record.current_page_nub = 1; //当前显示的页码 第一次进入当前页为第一页
	read_reset_record.read_reset_record_flash_flag = true;
	//历史事件
	write_event_record.ptr = 0;  //事件记录指针
	write_event_record.total_nub = 0;  //一共有多少条历史事件记录
	(void) write_eeprom(HISEVENTRECORD_PTR_ADDRESS, &write_event_record.ptr, 2);
	(void) write_eeprom(HISEVENTRECORD_NB_ADDRESS, &write_event_record.total_nub, 2);
	read_event_record.total_nub = write_event_record.total_nub;  //显示一共有多少条历史事件记录
	read_event_record.ptr = write_event_record.ptr;  //读事件记录指针
	read_event_record.current_page_nub = 1; //当前显示的页码 第一次进入当前页为第一页
	read_event_record.read_event_record_flash_flag = true;
	// 历史故障
	write_fault_record.ptr = 0;  // 故障指针
	write_fault_record.total_nub = 0;  //一共有多少条历史故障记录
	(void) write_eeprom(HISFAULTRECORD_PTR_ADDRESS, &write_fault_record.ptr, 2);
	(void) write_eeprom(HISFAULTRECORD_NB_ADDRESS, &write_fault_record.total_nub, 2);
	read_fault_record.total_nub = write_fault_record.total_nub;  //显示一共有多少条历史故障记录
	read_fault_record.ptr = write_fault_record.ptr;  //读故障记录指针
	read_fault_record.current_page_nub = 1; //当前显示的页码 第一次进入当前页为第一页
	read_fault_record.read_fault_record_flash_flag = true;
	// 历史告警
	write_alarm_record.ptr = 0;  // 故障指针
	write_alarm_record.total_nub = 0;  //一共有多少条历史故障记录
	(void) write_eeprom(HISALARMRECORD_PTR_ADDRESS, &write_alarm_record.ptr, 2);
	(void) write_eeprom(HISALARMRECORD_NB_ADDRESS, &write_alarm_record.total_nub, 2);
	read_alarm_record.total_nub = write_alarm_record.total_nub;  //显示一共有多少条历史告警记录
	read_alarm_record.ptr = write_alarm_record.ptr;  //读告警记录指针
	read_alarm_record.current_page_nub = 1; //当前显示的页码 第一次进入当前页为第一页
	read_alarm_record.read_alarm_record_flash_flag = true;
	//重新初始化曲线日志属性(清运行信息，历史曲线)
	resume_all_log(g_logattr);
	//加载默认运行信息数据
	Load_Default_RunInfo_Data();

	//串口1~5默认参数
	for (uint8_t i = 0; i < 5; i++)
	{
		comParam[i].baud = 9600;
		write_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7, &comParam[i].baud, 2);
		comParam[i].vmin = 255;
		write_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+2, &comParam[i].vmin, 2);
		comParam[i].sId = 1;
		write_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+4, &comParam[i].sId, 1);
		comParam[i].eId = 247;
		write_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+5, &comParam[i].eId, 1);
		comParam[i].protocol = SERIAL_PROTOCOL_MODBUS_NONE;
		write_eeprom(COM1_PARAM_BAUD_ADDRESS+i*7+6, &comParam[i].protocol, 1);
	}
	//串口6默认接屏
	comParam[5].baud = 9600;
	comParam[5].vmin = 8;
	comParam[5].protocol = SERIAL_PROTOCOL_MODBUS_TOUCH;
	comParam[5].sId = 1;
	comParam[5].eId = 247;

	//EMS参数
	for (uint8_t i = 0; i < PCS_NUM; i++)
	{
		EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address = i + 1;
		write_eeprom(PCS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address, 1);
	}
	for (uint8_t i = 0; i < PV_NUM; i++)
	{
		EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address = i + 1;
		write_eeprom(PV0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address, 1);
	}
	for (uint8_t i = 0; i < BMS_NUM; i++)
	{
		EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address = i + 1;
		write_eeprom(BMS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address, 1);
	}
	for (uint8_t i = 0; i < DCDC_NUM; i++)
	{
		EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address = i + 1;
		write_eeprom(DCDC0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address, 1);
	}
	for (uint8_t i = 0; i < Meter_NUM; i++)
	{
		EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address = i + 1;
		write_eeprom(AMMETER0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address, 1);
	}
	for (uint8_t i = 0; i < PVS_NUM; i++)
	{
		EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address = i + 1;
		write_eeprom(PVS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address, 1);
	}
	for (uint8_t i = 0; i < ENV_NUM; i++)
	{
		EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i].Modubus_Address = i + 1;
		write_eeprom(ENV0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i].Modubus_Address, 1);
	}
	//电表位置定义
	for (uint8_t i = 0; i < 3; i++)
	{
		System_Info.Config.Meter_Position_Enable[i] = ENABLE;
		write_eeprom(Meter_Position_Enable_ADDRESS0 + i, &System_Info.Config.Meter_Position_Enable[i], 1);
	}
	EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number = 3;

	//输入引脚定义
	for(uint8_t i=0;i<8;i++)
	{
		System_Info.Config.InputPin_Function[i]=NODEF_PIN;
		write_eeprom(InputPin_Function_ADDRESS0 + i, &System_Info.Config.InputPin_Function[i], 1);
	}

	//输入引脚常开常闭定义
	for(uint8_t i=0;i<8;i++)
	{
		System_Info.Config.InputPin_NC_NO[i]=NORMAL_OPEN;
		write_eeprom(InputPin_NC_NO_ADDRESS0 + i, &System_Info.Config.InputPin_NC_NO[i], 1);
	}

	//运行模式 电网优先
	System_Info.Config.EMS_Run_Priority = 0;
	write_eeprom(EMS_RUN_MODE_ADDRESS, &System_Info.Config.EMS_Run_Priority, 1);
	//启动类型 被动模式
	System_Info.Config.EMS_Start_Type = 1; // reactive
	write_eeprom(EMS_START_TYPE_ADDRESS, &System_Info.Config.EMS_Start_Type, 1);
	//协议类型 新协议
	System_Info.Config.PCS_Protocol_Type = NEW_PROTOCOL;
	write_eeprom(PCS_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.PCS_Protocol_Type, 1);
	System_Info.Config.Ammeter_Protocol_Type = 0;
	write_eeprom(AMMETER_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.Ammeter_Protocol_Type, 1);
	//母线类型,默认直流母线
	//System_Info.Config.Bus_Type=0;
	//write_eeprom(BUS_TYPE_ADDRESS, &System_Info.Config.Bus_Type, 1);
	webPageShow.pcs = 1;
	webPageShow.pv = 0;
	write_eeprom(WEB_PAGE_PCS_ST_ADDRESS, &webPageShow.pcs, 1);
	write_eeprom(WEB_PAGE_PV_ST_ADDRESS, &webPageShow.pv, 1);
	//并网类型选择：并网
	//Grid_Connect_Type=1;
	//write_eeprom(GRID_CONNECT_TYPE_ADDRESS, &Grid_Connect_Type, 1);
	//系统功率设置
	System_Info.Config.System_Power_Set = SYS_POWER_SET_DFT;
	write_eeprom(SYS_POWER_SET_ADDRESS, &System_Info.Config.System_Power_Set, 2);
	//最大逆流功率
    Max_anti_reflux_power = 0;
    write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
	//PCS台数
	EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number = P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Dft;
	write_eeprom(TOTAL_PCS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number, 2);
	//PV台数
	EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number = P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Dft;
	write_eeprom(TOTAL_PV_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number, 2);
	//BMS台数	
	EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number = P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Dft;
	write_eeprom(TOTAL_BMS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number, 2);
	//DCDC台数	
	EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number = P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Dft;
	write_eeprom(TOTAL_DCDC_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number, 2);
	//电表台数
	EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number = P_Range[System_Info.Config.PCS_Type].AMMETER_Total_Num.Dft;
	write_eeprom(TOTAL_AMMETER_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number, 2);
	//汇流箱台数
	EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number = P_Range[System_Info.Config.PCS_Type].PVS_Total_Num.Dft;
	write_eeprom(TOTAL_PVS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number, 2);
	//微网电压
	EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage = P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Dft;
	write_eeprom(MG_VOLTAGE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage, 2);
	//微网频率
	EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency = P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Dft;
	write_eeprom(MG_FREQUENCY_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency, 2);
	//并网运行电压上限
	EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max;
	write_eeprom(CONST_V_MODE_V_MAX_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max, 2);
	//并网运行电压下限
	EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min;
	write_eeprom(CONST_V_MODE_V_MIN_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min, 2);
	//并网运行功率设置
	EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set = 0;
	write_eeprom(EMS_RUN_POWER_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set, 2);
	//恒压模式电压
	EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Dft;
	write_eeprom(CONST_V_MODE_V_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V, 2);
	//恒压模式电流
	EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Dft;
	write_eeprom(CONST_V_MODE_I_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I, 2);
	//无功调节方式
	EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode = SHUT_OFF;
	write_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);
	for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
	{
		PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode = EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
		Send_Data_To_PCS_0x10_02[i] = true;
	}
	//功率因数
	EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor = P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft;
	write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
	for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
	{
		PCS_Ctrl_Data[i].Power_Factor = EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
		Send_Data_To_PCS_0x10_02[i] = true;
	}
	//无功功率
	EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set = 0;
	write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
	//远程本地模式
	EMS_RL_Mode = RE_LO_MODE;
	write_eeprom(EMS_RL_MODE_ADDRESS,&EMS_RL_Mode,1);
    //低电量节点1输出模式    
    EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode=NORMAL_OPEN;
    write_eeprom(Low_Battery_Output1_Mode_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output1_Mode, 1);    
    //低电量节点2输出模式    
    EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode=NORMAL_OPEN;
    write_eeprom(Low_Battery_Output2_Mode_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.Low_Battery_Output2_Mode, 1);
	//SOC控制
	EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable = DISABLE;
	write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2 = P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Dft;
	write_eeprom(SOC_UPLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1 = P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Dft;
	write_eeprom(SOC_UPLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1 = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Dft;
	write_eeprom(SOC_LOWLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2 = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Dft;
	write_eeprom(SOC_LOWLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Dft;
	write_eeprom(SOC_LOWLIMIT1_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Dft;
	write_eeprom(SOC_LOWLIMIT2_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis = 10;
	write_eeprom(SOC_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay = P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Dft;
	write_eeprom(SOC_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay = P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Dft;
	write_eeprom(SOC_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay, 2);

	//电池电压控制
	EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable = ENABLE;
	write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2 = P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Dft;
	write_eeprom(VBAT_UPLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1 = P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Dft;
	write_eeprom(VBAT_UPLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Dft;
	write_eeprom(VBAT_LOWLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2 = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Dft;
	write_eeprom(VBAT_LOWLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Dft;
	write_eeprom(VBAT_LOWLIMIT1_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Dft;
	write_eeprom(VBAT_LOWLIMIT2_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis = 50;
	write_eeprom(VBAT_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay = P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Dft;
	write_eeprom(VBAT_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay, 2);

	EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay = P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Dft;
	write_eeprom(VBAT_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay, 2);

//-------------------------------------------------------------------------------------------------------------------------------------------

	//分时电价设置点表
	//设置0~9
	for(uint8_t i=0; i<10; i++)
	{
		//起始时间0(时)
		Time_Div_Info.Time_Div_Set_Info[i].Start_time.hour = 0;
		write_eeprom(TIME_DIV_START_TIME_HOUR_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].Start_time.hour, 1);
		//起始时间0(分)
		Time_Div_Info.Time_Div_Set_Info[i].Start_time.min = 0;
		write_eeprom(TIME_DIV_START_TIME_MIN_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].Start_time.min, 1);
		//结束时间0(时)
		Time_Div_Info.Time_Div_Set_Info[i].End_time.hour = 0;
		write_eeprom(TIME_DIV_END_TIME_HOUR_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].End_time.hour, 1);
		//结束时间0(分)
		Time_Div_Info.Time_Div_Set_Info[i].End_time.min = 0;
		write_eeprom(TIME_DIV_END_TIME_MIN_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].End_time.min, 1);
		//
		Time_Div_Info.Time_Div_Set_Info[i].Run_Mode = GRID_ISLAND_MODE;
		write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].Run_Mode, 1);
		//电价0
		Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode = 0;
		write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS+7*i, &Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode, 1);
		//运行功率设定0
		Time_Div_Info.Time_Div_Set_Info[i].Run_Power = 0;
		write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+i*2, &Time_Div_Info.Time_Div_Set_Info[i].Run_Power, 2);
		// 离网运行功率设定0
		Time_Div_Info.Time_Div_Set_Info[i].Mgrid_Run_Power = 0;
		write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+i*2, &Time_Div_Info.Time_Div_Set_Info[i].Mgrid_Run_Power, 2);
	}

	//分星期时间0~6
	for(uint8_t i=0; i<7; i++)
	{
		Time_Div_Info.Time_Div_Set_Info[0].Week = 1+i;
		write_eeprom(TIME_DIV_WEEK_0_ADDRESS+i, &Time_Div_Info.Time_Div_Set_Info[i].Week, 1);
	}

	//消峰填谷
	Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power = 0;
	write_eeprom(PEAK_VALLEY_1_START_CHARGE_POWER_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power, 2);

	Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power = 0;
	write_eeprom(PEAK_VALLEY_1_START_DISCHARGE_POWER_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power, 2);

	Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle = 6900;
	write_eeprom(PEAK_VALLEY_1_UPDATE_CYCLE_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle, 2);

	//平滑上网
	//额定光伏功率
	Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power = 1200;
	write_eeprom(RAMP_RATE_CTRL_PV_RATED_POWER_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power,4);

	//斜率
	Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio = 10;
	write_eeprom(RAMP_RATE_CTRL_RAMP_RATIO_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio,2);

	//斜率时间
	Time_Div_Info.Time_Div_Set_Info[0].Ramp_time = 1;
	write_eeprom(RAMP_RATE_CTRL_RAMP_TIME_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Ramp_time,2);

	//SOC上限
	Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit = 500;
	write_eeprom(RAMP_RATE_CTRL_SOC_UPLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit, 2);

	//SOC下限
	Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit = 450;
	write_eeprom(RAMP_RATE_CTRL_SOC_LOWLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit, 2);

	//vbat上限
	Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit = 6500;
	write_eeprom(RAMP_RATE_CTRL_VBAT_UPLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit, 2);

	//vbat下限
	Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit = 6000;
	write_eeprom(RAMP_RATE_CTRL_VBAT_LOWLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit, 2);

	//网口下发周期
	EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay = 70;
	write_eeprom(PCS_NET_SETPARA_DELAY_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay, 2);

	EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle = 11;
	write_eeprom(PCS_NET_SET_CYCLE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle, 2);

	//调频
	//模拟数字调频
	Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital = 0;
	write_eeprom(FM_ANALOG_DIGITAL_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital, 2);

	//模拟上限
	Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax = 10;
	write_eeprom(FM_ANALOG_VMAX_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax, 2);

	//模拟下限
	Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin = 0;
	write_eeprom(FM_ANALOG_VMIN_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin, 2);

	//PCS自充电功率
	Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power = 0;
	write_eeprom(RAMP_RATE_CTRL_PCS_AUTOCHARGE_POWER, &Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power, 2);


//-------------------------------------------------------------------------------------------------------------------------------------------
	//Modbus地址(上位机)
	System_Info.Modbus_Address = PC_RS485_COM_ADDR_DEFAULT;
	write_eeprom(MODBUS_ADDRESS_ADDRESS, &System_Info.Modbus_Address, 1);

	//Modbus波特率(上位机)
	System_Info.Baud_Rate = 2;
	write_eeprom(BAUD_RATE_ADDRESS, &System_Info.Baud_Rate, 1);

	//Modbus地址(BMS)
	//System_Info.BMS_Address=BMS_RS485_COM_ADDR_DEFAULT;
	//write_eeprom(BMS_ADDRESS_ADDRESS,&System_Info.BMS_Address,1);
	////触摸屏硬件版本
	//(void)read_eeprom(TOUCH_HARDWARE_EDITION_ADDRESS,&Com_Data.Touch_Hardware_Edition,1);
	//if((Com_Data.Touch_Hardware_Edition == 0) || (Com_Data.Touch_Hardware_Edition > 100))
	//{
	//    Com_Data.Touch_Hardware_Edition = 1;
	//    (void)write_eeprom(TOUCH_HARDWARE_EDITION_ADDRESS,&Com_Data.Touch_Hardware_Edition,1);
	//}
	////触摸屏软件版本
	//(void)read_eeprom(TOUCH_SOFTWARE_EDITION_ADDRESS,&Com_Data.Touch_Software_Edition,1);
	//if((Com_Data.Touch_Software_Edition == 0) || (Com_Data.Touch_Software_Edition > 26))
	//{
	//    Com_Data.Touch_Software_Edition = 1;
	//    (void)write_eeprom(TOUCH_SOFTWARE_EDITION_ADDRESS,&Com_Data.Touch_Software_Edition,1);
	//}

	//NTP
	//IP地址
	System_Info.NTP.ip[0] = 5;
	System_Info.NTP.ip[1] = 79;
	System_Info.NTP.ip[2] = 108;
	System_Info.NTP.ip[3] = 34;
	(void) write_eeprom(NTP_IP_0_ADDRESS, System_Info.NTP.ip, 4);

	System_Info.NTP.port = 123;
	(void) write_eeprom(NTP_PORT_ADDRESS, &System_Info.NTP.port, 2);

	System_Info.NTP.enable = 0xAA;
	write_eeprom(NTP_ENABLE_ADDRESS, &System_Info.NTP.enable, 1);

	//目标服务器参数
	for(int i = 0; i < PCS_NUM; i++)
	{
		//IP地址
		System_Info.Server[i].ip[0] = 192;
		System_Info.Server[i].ip[1] = 168;
		System_Info.Server[i].ip[2] = 0;
		System_Info.Server[i].ip[3] = 1+i;
		write_eeprom(SERVER_0_IP_0_ADDRESS+i*4, &System_Info.Server[i].ip[0], 4);	//;
		//端口号
		System_Info.Server_Modbus_Port[i] = 502;
		write_eeprom(SERVER_0_PORT_ADDRESS+i*2, &System_Info.Server_Modbus_Port[i], 2);
	}

	char ipStr[16]={0};
	for(int i=0; i<PCS_NUM; i++)
	{
		memset(ipStr, 0x00, 16);
		sprintf(ipStr,"%d.%d.%d.%d", System_Info.Server[i].ip[0],System_Info.Server[i].ip[1],System_Info.Server[i].ip[2],System_Info.Server[i].ip[3]);
		ctrl.Client[i].SetServer(ipStr, System_Info.Server_Modbus_Port[i]);
		ctrl.Client[i].SetProtocol(CLIENT_PROTOCOL_PCS_TCPMODBUS);
	}

	//目标服务器参数
	for(int i = 0; i < PV_NUM; i++)
	{
		//IP地址
		System_Info.PVServer[i].ip[0] = 192;
		System_Info.PVServer[i].ip[1] = 168;
		System_Info.PVServer[i].ip[2] = 0;
		System_Info.PVServer[i].ip[3] = 101+i;
		write_eeprom(SERVER_PV_0_IP_0_ADDRESS+i*4, &System_Info.PVServer[i].ip[0], 4);	//;
		//端口号
		System_Info.PVServer_Modbus_Port[i] = 502;
		write_eeprom(SERVER_PV_0_PORT_ADDRESS+i*2, &System_Info.PVServer_Modbus_Port[i], 2);
	}

	for(int i=0; i<PV_NUM; i++)
	{
		memset(ipStr, 0x00, 16);
		sprintf(ipStr,"%d.%d.%d.%d", System_Info.PVServer[i].ip[0],System_Info.PVServer[i].ip[1],System_Info.PVServer[i].ip[2],System_Info.PVServer[i].ip[3]);
		ctrl.PvClient[i].SetServer(ipStr, System_Info.PVServer_Modbus_Port[i]);
		ctrl.PvClient[i].SetProtocol(CLIENT_PROTOCOL_PV_TCPMODBUS);
	}

	//IEC104通讯地址
	System_Info.IEC104_Address = 1;
	(void) write_eeprom(IEC104_ADDRESS_ADDRESS, &System_Info.IEC104_Address, 2);
	//IEC104
	System_Info.IEC104_T1 = 20;
	(void) write_eeprom(IEC104_T1_ADDRESS, &System_Info.IEC104_T1, 2);
	//IEC104
	System_Info.IEC104_T3 = 25;
	(void) write_eeprom(IEC104_T3_ADDRESS, &System_Info.IEC104_T3, 2);
	//IEC104
	System_Info.IEC104_K = 8;
	(void) write_eeprom(IEC104_K_ADDRESS, &System_Info.IEC104_K, 2);
	//IEC104
	System_Info.IEC104_W = 5;
	(void) write_eeprom(IEC104_W_ADDRESS, &System_Info.IEC104_W, 2);

	//webserver
	Webserver_Language = CHINESE;
	(void) write_eeprom(WEBSERVER_LANGUAGE_ADDRESS, &Webserver_Language, 1);

	for(uint8_t i=0;i<4;i++){operator_passwd[i]='1';}
	for(uint8_t i=4;i<8;i++){operator_passwd[i]=0;}
	(void)write_eeprom(OPERATOR_PASSWORD_ADDRESS, operator_passwd, 8);

	for(uint8_t i=0;i<4;i++){administrator_passwd[i]='2';}
	for(uint8_t i=4;i<8;i++){administrator_passwd[i]=0;}
	(void)write_eeprom(ADMINISTRATOR_PASSWORD_ADDRESS, administrator_passwd, 8);

	for(uint8_t i=0;i<4;i++){maintainer_passwd[i]='3';}
	for(uint8_t i=4;i<8;i++){maintainer_passwd[i]=0;}
	(void)write_eeprom(MAINTRAINER_PASSWORD_ADDRESS, maintainer_passwd, 8);
}

//恢复控制网口参数
void Load_Default_Net_Process(void)
{
	//DHCP关     ccg 2013.6.3
	System_Info.Net.dhcp_flag = D_DHCP_OFF;
	(void) write_eeprom(NET_DHCP_ADDRESS, &System_Info.Net.dhcp_flag, 1);
	System_Info.Net2.dhcp_flag = D_DHCP_OFF;
	(void) write_eeprom(NET2_DHCP_ADDRESS, &System_Info.Net2.dhcp_flag, 1);
	//TCP端口号
	System_Info.Modbus_Port = MODBUS_PORT;
	(void) write_eeprom(NET_PORT_ADDRESS, &System_Info.Modbus_Port, 2);
	//IP地址
	System_Info.Net.ip[0] = 192;
	System_Info.Net.ip[1] = 168;
	System_Info.Net.ip[2] = 1;
	System_Info.Net.ip[3] = 100;
	(void) write_eeprom(NET_IP_0_ADDRESS, System_Info.Net.ip, 4);
	//子网掩码
	System_Info.Net.submask[0] = 255;
	System_Info.Net.submask[1] = 255;
	System_Info.Net.submask[2] = 255;
	System_Info.Net.submask[3] = 0;
	(void) write_eeprom(NET_SUBMASK_0_ADDRESS, System_Info.Net.submask, 4);
	//网关
	System_Info.Net.gateway[0] = 192;
	System_Info.Net.gateway[1] = 168;
	System_Info.Net.gateway[2] = 1;
	System_Info.Net.gateway[3] = 1;
	(void) write_eeprom(NET_GATEWAY_0_ADDRESS, System_Info.Net.gateway, 4);
	//DNS1
	System_Info.Net.dns1[0] = 1;
	System_Info.Net.dns1[1] = 2;
	System_Info.Net.dns1[2] = 4;
	System_Info.Net.dns1[3] = 8;
	(void) write_eeprom(NET_DNS1_0_ADDRESS, System_Info.Net.dns1, 4);
	//DNS2
	System_Info.Net.dns2[0] = 8;
	System_Info.Net.dns2[1] = 8;
	System_Info.Net.dns2[2] = 8;
	System_Info.Net.dns2[3] = 8;
	(void) write_eeprom(NET_DNS2_0_ADDRESS, System_Info.Net.dns2, 4);

	//IP地址
	System_Info.Net2.ip[0] = 192;
	System_Info.Net2.ip[1] = 168;
	System_Info.Net2.ip[2] = 0;
	System_Info.Net2.ip[3] = 100;
	(void) write_eeprom(NET2_IP_0_ADDRESS, System_Info.Net2.ip, 4);
	//子网掩码
	System_Info.Net2.submask[0] = 255;
	System_Info.Net2.submask[1] = 255;
	System_Info.Net2.submask[2] = 255;
	System_Info.Net2.submask[3] = 0;
	(void) write_eeprom(NET2_SUBMASK_0_ADDRESS, System_Info.Net2.submask, 4);
	//网关
	System_Info.Net2.gateway[0] = 192;
	System_Info.Net2.gateway[1] = 168;
	System_Info.Net2.gateway[2] = 0;
	System_Info.Net2.gateway[3] = 1;
	(void) write_eeprom(NET2_GATEWAY_0_ADDRESS, System_Info.Net2.gateway, 4);
	//DNS1
	System_Info.Net2.dns1[0] = 1;
	System_Info.Net2.dns1[1] = 2;
	System_Info.Net2.dns1[2] = 4;
	System_Info.Net2.dns1[3] = 8;
	(void) write_eeprom(NET2_DNS1_0_ADDRESS, System_Info.Net2.dns1, 4);
	//DNS2
	System_Info.Net2.dns2[0] = 8;
	System_Info.Net2.dns2[1] = 8;
	System_Info.Net2.dns2[2] = 8;
	System_Info.Net2.dns2[3] = 8;
	(void) write_eeprom(NET2_DNS2_0_ADDRESS, System_Info.Net2.dns2, 4);

	Get_Local_Mac(&System_Info.Net, (char*)"eth0");
	Get_Local_Mac(&System_Info.Net2, (char*)"eth1");
	SetNetParam(System_Info.Net, (char*)"eth0");
	SetNetParam(System_Info.Net2, (char*)"eth1");
	IAT_Info.Set_Info.MAC[0] = (System_Info.Net.mac[0]<<8) | System_Info.Net.mac[1];
	IAT_Info.Set_Info.MAC[1] = (System_Info.Net.mac[2]<<8) | System_Info.Net.mac[3];
	IAT_Info.Set_Info.MAC[2] = (System_Info.Net.mac[4]<<8) | System_Info.Net.mac[5];
}

//保存曲线数据
void Save_Power_To_Buffer(int32_t *regbuf1, int32_t buf1, int32_t *regbuf2, int32_t buf2, int32_t *regbuf3, int32_t buf3,int32_t *regbuf4, int32_t buf4)                                                       //加载充电曲线
{
	uint32_t i=0, j=0;

	i = showTime.tm_hour*3600+showTime.tm_min*60+showTime.tm_sec;
	j = i / Time_900_seconds;    //15min记一次
	if(j>=1440)
	{
		//printf("array overflow\n");
		return;
	}
	regbuf1[j]=buf1;
	regbuf2[j]=buf2;
	regbuf3[j]=buf3;
	regbuf4[j]=buf4;
}

//记录当天曲线, from eeprom
void Write_Day_Curve_Array(void)
{
	//printf("+++ write day curve +++\n");
	//原始功率曲线
	(void)write_eeprom(CURVE_ARRAY_BASE_ADDRESS, (char*)&PV_Power_Curve_Buffer, 1440*4*sizeof(char));
	//实际功率曲线
	(void)write_eeprom(CURVE_ARRAY_BASE_ADDRESS+1440*4, (char*)&Grid_Power_Curve_Buffer, 1440*4*sizeof(char));
	//储能功率曲线
	(void)write_eeprom(CURVE_ARRAY_BASE_ADDRESS+1440*8, (char*)&PCS_Power_Curve_Buffer, 1440*4*sizeof(char));
	//负载功率曲线
	(void)write_eeprom(CURVE_ARRAY_BASE_ADDRESS+1440*12, (char*)&Load_Power_Curve_Buffer, 1440*4*sizeof(char));
}
//读当前曲线, from eeprom
void Read_Day_Curve_Array(void)
{
	//printf("+++ read day curve +++\n");
	//原始功率曲线
	(void)read_eeprom(CURVE_ARRAY_BASE_ADDRESS, (char*)&PV_Power_Curve_Buffer, 1440*4*sizeof(char));
	//实际功率曲线
	(void)read_eeprom(CURVE_ARRAY_BASE_ADDRESS+1440*4, (char*)&Grid_Power_Curve_Buffer, 1440*4*sizeof(char));
	//储能功率曲线
	(void)read_eeprom(CURVE_ARRAY_BASE_ADDRESS+1440*8, (char*)&PCS_Power_Curve_Buffer, 1440*4*sizeof(char));
	//负载功率曲线
	(void)read_eeprom(CURVE_ARRAY_BASE_ADDRESS+1440*12, (char*)&Load_Power_Curve_Buffer, 1440*4*sizeof(char));
}
//清空当天曲线
void Clear_Day_Curve_Array(void)
{
	for(int i=0; i<1440; i++)
	{
		PV_Power_Curve_Buffer[i]=0;
		Grid_Power_Curve_Buffer[i]=0;
		PCS_Power_Curve_Buffer[i]=0;
		Load_Power_Curve_Buffer[i]=0;
	}
	//光伏功率曲线
	(void)write_eeprom(CURVE_ARRAY_BASE_ADDRESS, (char*)&PV_Power_Curve_Buffer, 1440*4*sizeof(char));
	//并网点功率曲线
	(void)write_eeprom(CURVE_ARRAY_BASE_ADDRESS+1440*4, (char*)&Grid_Power_Curve_Buffer, 1440*4*sizeof(char));
	//储能功率曲线
	(void)write_eeprom(CURVE_ARRAY_BASE_ADDRESS+1440*8, (char*)&PCS_Power_Curve_Buffer, 1440*4*sizeof(char));
	//负载功率曲线
	(void)write_eeprom(CURVE_ARRAY_BASE_ADDRESS+1440*12, (char*)&Load_Power_Curve_Buffer, 1440*4*sizeof(char));
}
//写运行记录
static uint8_t run_buf[EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN] = {0};  //超过linux默认堆栈大小8196,改成全局变量
void Write_Histroy_RunRecord(void)
{
     //uint8_t run_buf[EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN] = {0};  //超过linux默认堆栈大小8196,改成全局变量
     uint8_t *l_data=run_buf;

	 l_data = devices_savedata(l_data, (uint16_t) System_Info.Config.PCS_Type);										//储能型号
     l_data = devices_savedata(l_data, (uint16_t) System_Info.Config.System_Power_Set);								//系统额定功率
	 l_data = devices_savedata(l_data, (uint16_t) Grid_Connect_Type);												//并网使能
	 l_data = devices_savedata(l_data, (uint16_t) System_Info.Config.Bus_Type);										//总线类型
     l_data = devices_savedata(l_data, (uint16_t) System_Info.Config.Time_Divide_Function);							//分时使能
     l_data = devices_savedata(l_data, (uint16_t)(FAULT ? 6 :(EMGSTOP ? 9 :
    		 Run_Status(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status))));							//工作状态
     l_data = devices_savedata(l_data, (uint16_t)(Grid_Exist ? 1 : 0));         									//电网状态
     l_data = devices_savedata(l_data, (uint16_t)((Meter_Info[0].VAB+Meter_Info[0].VBC+Meter_Info[0].VCA)/3));		//电网电压   电表
     l_data = devices_savedata(l_data, Meter_Info[0].Freq);															//电网频率
     l_data = devices_savedata(l_data,(int32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power);					//电网功率
     l_data = devices_savedata(l_data, (uint16_t)DIN(Grid_Contacter));	  											//电网侧接触器状态
     l_data = devices_savedata(l_data, (uint16_t)GRID_CONTACTOR_FAULT);												//电网侧接触器故障状态
     l_data = devices_savedata(l_data, (uint16_t)DIN(MGrid_Contacter));												//微网侧接触器状态
     l_data = devices_savedata(l_data, (uint16_t)MGRID_CONTACTOR_FAULT);											//微网侧接触器故障状态
     l_data = devices_savedata(l_data,(uint16_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode);    //运行模式
     l_data = devices_savedata(l_data,(uint16_t)Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode);//功率控制模式
     l_data = devices_savedata(l_data,(uint32_t)EMS_Run_Power_Active);   											//运行功率
     l_data = devices_savedata(l_data, (uint16_t) System_Info.Config.PCS_Grid_Run_Mode);							//PCS并网工作模式
     l_data = devices_savedata(l_data, ((uint32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power)&0xffff);		//PCS有功功率
     l_data = devices_savedata(l_data, ((uint32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power)>>16);			//PCS有功功率
     l_data = devices_savedata(l_data, EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage);								//微网电压
     l_data = devices_savedata(l_data, EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency);								//微网频率
     l_data = devices_savedata(l_data, EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power&0xffff);					//光伏逆变器有功功率
     l_data = devices_savedata(l_data, (EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power)>>16);						//光伏逆变器有功功率
     l_data = devices_savedata(l_data, EMS_Info.EMS3x.SOC_Info.RMeasure.PV_DC_V_Aver);								//光伏逆变器直流电压
     l_data = devices_savedata(l_data, ((uint32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power)&0xffff);		//负载功率 电表采集
     l_data = devices_savedata(l_data, ((uint32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power)>>16);         //负载功率 电表采集
     l_data = devices_savedata(l_data, EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Voltage);                              //负载电压 电表采集
     l_data = devices_savedata(l_data, EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Max);							//电池电压最大值
     l_data = devices_savedata(l_data, EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_Voltage_Min);							//电池电压最小值
     l_data = devices_savedata(l_data, EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max);								//电池SOC最大值
     l_data = devices_savedata(l_data, EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Min);								//电池SOC最小值

     store_record(E_RUNRECORD_LOG, run_buf, EVERY_RUNRECORD_DATA_LEN, EVERY_RUNRECORD_BUF_LEN);
}
//写历史曲线数据
static uint8_t curve_buf[EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN] = {0};   //超过linux默认堆栈大小8196,改成全局变量
void Write_Histroy_Curve_Data(void)
{
    //uint8_t curve_buf[EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN] = {0};   //超过linux默认堆栈大小8196,改成全局变量

	if(Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode < 10)	//分时电价
	{
		if(System_Info.Config.Time_Divide_Function != 0)
		{
			memcpy((uint8_t*)curve_buf, (uint8_t*)PV_Power_Curve_Buffer, 96*4*sizeof(char));
			memcpy((uint8_t*)&curve_buf[96*4], (uint8_t*)Grid_Power_Curve_Buffer, 96*4*sizeof(char));
			memcpy((uint8_t*)&curve_buf[96*8], (uint8_t*)PCS_Power_Curve_Buffer, 96*4*sizeof(char));
			store_record(E_TIME_OF_USE_CURVE_LOG, curve_buf, EVERY_DAY_TIME_OF_USE_DATA_LEN, EVERY_DAY_TIME_OF_USE_CURVE_BUF_LEN);
		}
		else
		{
			memcpy((uint8_t*)curve_buf, (uint8_t*)PV_Power_Curve_Buffer, 96*4*sizeof(char));
			memcpy((uint8_t*)&curve_buf[96*4], (uint8_t*)Grid_Power_Curve_Buffer, 96*4*sizeof(char));
			memcpy((uint8_t*)&curve_buf[96*8], (uint8_t*)PCS_Power_Curve_Buffer, 96*4*sizeof(char));
			store_record(E_MGRID_CURVE_LOG, curve_buf, EVERY_DAY_MGRID_CURVE_DATA_LEN, EVERY_DAY_MGRID_CURVE_BUF_LEN);
		}
	}
	else if(Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode==PEAK_LOAD_SHIFTING)		//消峰填谷
	{
		memcpy((uint8_t*)curve_buf, (uint8_t*)Load_Power_Curve_Buffer, 96*4*sizeof(char));
		memcpy((uint8_t*)&curve_buf[96*4], (uint8_t*)Grid_Power_Curve_Buffer, 96*4*sizeof(char));
		memcpy((uint8_t*)&curve_buf[96*8], (uint8_t*)PCS_Power_Curve_Buffer, 96*4*sizeof(char));
		store_record(E_PEAKVALLEY_CURVE_LOG, curve_buf, EVERY_DAY_PEAKVALLEY_CURVE_DATA_LEN, EVERY_DAY_PEAKVALLEY_CURVE_BUF_LEN);
	}
	else if(Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode==ACSIDE_PVLIMIT)     //交流限发
	{
		memcpy((uint8_t*)curve_buf, (uint8_t*)PV_Power_Curve_Buffer, 96*4*sizeof(char));
		memcpy((uint8_t*)&curve_buf[96*4], (uint8_t*)Grid_Power_Curve_Buffer, 96*4*sizeof(char));
		memcpy((uint8_t*)&curve_buf[96*8], (uint8_t*)PCS_Power_Curve_Buffer, 96*4*sizeof(char));
		store_record(E_AC_LIMITED_PRODUCE_CURVE_LOG, curve_buf, EVERY_DAY_ACLIMIT_CURVE_DATA_LEN, EVERY_DAY_ACLIMIT_CURVE_BUF_LEN);
	}
	else if(Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode==RAMP_RATE_CONTROL)	//输出平滑
	{
		memcpy((uint8_t*)curve_buf, (uint8_t*)PV_Power_Curve_Buffer, 1440*4*sizeof(char));
		memcpy((uint8_t*)&curve_buf[1440*4], (uint8_t*)Grid_Power_Curve_Buffer, 1440*4*sizeof(char));
		memcpy((uint8_t*)&curve_buf[1440*8], (uint8_t*)PCS_Power_Curve_Buffer, 1440*4*sizeof(char));
		store_record(E_OUTPUT_SMOOTH_CURVE_LOG, curve_buf, EVERY_DAY_OUTPUT_SMOOTH_CURVE_DATA_LEN, EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN);
	}
	else if(Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode==FM)   //调频
	{
		memcpy((uint8_t*)curve_buf, (uint8_t*)PV_Power_Curve_Buffer, 96*4*sizeof(char));
		memcpy((uint8_t*)&curve_buf[96*4], (uint8_t*)Grid_Power_Curve_Buffer, 96*4*sizeof(char));
		memcpy((uint8_t*)&curve_buf[96*8], (uint8_t*)PCS_Power_Curve_Buffer, 96*4*sizeof(char));
		store_record(E_FM_CURVE_LOG, curve_buf, EVERY_DAY_FM_CURVE_DATA_LEN, EVERY_FM_BUF_LEN);
	}
}

void Set_Dev_ID(SERIAL_PROTOCOL_TYPE protocol, int stID)
{
	if(protocol == SERIAL_PROTOCOL_MODBUS_PCS)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address = i + stID;
			write_eeprom(PCS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address, 1);
		}
	}
	else if(protocol == SERIAL_PROTOCOL_MODBUS_PV)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address = i + stID;
			write_eeprom(PV0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address, 1);
		}
	}
	else if(protocol == SERIAL_PROTOCOL_MODBUS_BMS)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address = i + stID;
			write_eeprom(BMS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address, 1);
		}
	}
	else if(protocol == SERIAL_PROTOCOL_MODBUS_DCDC)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number; i++)
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address = i + stID;
			write_eeprom(DCDC0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address, 1);
		}
	}
	else if(protocol == SERIAL_PROTOCOL_AMMETER)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number; i++)
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address = i + stID;
			write_eeprom(AMMETER0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address, 1);
		}
	}
	else if(protocol == SERIAL_PROTOCOL_MODBUS_PVS)
	{
		for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number; i++)
		{
			EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address = i + stID;
			write_eeprom(PVS0_ADDRESS + i, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address, 1);
		}
	}
}
