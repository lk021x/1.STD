/*
 * com_io_ammeter.h
 *
 *  Created on: 2017-2-24
 *      Author: root
 */

#ifndef COM_IO_AMMETER_H_
#define COM_IO_AMMETER_H_

//#ifndef __DL_T645_H__
//#define __DL_T645_H__

//-----------------------------------------------------------------------------------
//DL/T645
//协议帧
#define FRAME_START		0x68		//帧起始
#define INVALID_ADDR	0xAA		//无效地址填充
#define FRAME_END		0x16		//帧结束
#define DB_SEPARATOR	0xAA		//数据块分隔符
#define BAUDRATE_ERR	0xff		//否认更改速率


//几大重要数据标识编码

#define PWR_Direct_IDENTIFIER   0xC020
#define MEASURE_IDENTIFIER		0xB6FF
#define ACTIVE_P_IDENTIFIER		0x9010
#define REACTIVE_Q_IDENTIFIER	0x9110
#define PARA_PT_IDENTIFIER		0xC517
#define PARA_CT_IDENTIFIER		0xC518

//----------------------------------------

typedef struct//S_Meter_Info_T
{
	//common info
	uint8_t  Slave_addr[6]; 		//DL/T645协议

	//read info
	uint16_t VA;			        //
	uint16_t VB;
	uint16_t VC;

	uint16_t IA;
	uint16_t IB;
	uint16_t IC;

	int32_t Total_active_P;
	int32_t Active_PA;
	int32_t Active_PB;
	int32_t Active_PC;

	int32_t Total_Q;
	int32_t QA;
	int32_t QB;
	int32_t QC;

	uint16_t VAB;
	uint16_t VBC;
	uint16_t VCA;

	int16_t Total_P_factor;
	int16_t A_P_factor;
	int16_t B_P_factor;
	int16_t C_P_factor;

	int32_t Total_apparent_P;
	int32_t Apparent_PA;
	int32_t Apparent_PB;
	int32_t Apparent_PC;

	uint16_t Freq;

	//PT
	uint32_t Positive_active_Pt;
	uint32_t Positive_reactive_Qt;

	//Set info
	uint32_t PT;
	uint32_t CT;
}S_Meter_Info_T;


//---------------------------------------
//功能码
//CONTROL CODE (BIT4~BIT0)
enum __func_code
{
	Reserve_code 			= 0x00,	//预留
	Read_data_code			= 0x01, //读数据
	Read_follow_data_code 	= 0x02, //读后续数据
	Reread_data_code		= 0x03, //重读数据
	Write_data_code			= 0x04, //写数据
	Broadcast_timing_code	= 0x08, //广播校时
	Write_device_addr		= 0x0a, //写设备地址
	Change_baudrate			= 0x0c, //更改通信速率
	Change_password			= 0x0f, //修改密码
	Clear_max_demand_code	= 0x10  //最大需量清零
};
//主从站发送标志CONTROL CODE (BIT7)
#define Master_Send		0x00			//主站发送
#define Slave_Send		0x80			//从站发送
//从站应答CONTROL CODE (BIT6)
#define Slave_ack_ok	0x00			//从站正确应答
#define Slave_ack_err	0x40			//从站对异常信息的应答
//有无后续数据帧CONTROL CODE (BIT5)
#define No_follow_data_frame	0x00	//无后续数据帧
#define With_follow_data_frame	0x20	//有后续数据帧
//----------------------------------------



//----------------------------------------
//数据标识结构及编码
//DI1H
#define DI1H_ELEC_ENERGY		0x90		//电能量
#define DI1H_MAX_DEMAND			0xA0		//最大需量
#define DI1H_VARIABLE			0xB0		//变量
#define DI1H_PARAMETER			0xC0		//参变量
#define DI1H_LOAD_CURVE			0xD0		//负荷曲线
#define DI1H_FACTORY_EXT		0xE0		//厂家功能扩展
#define DI1H_RESERVE			0xF0		//保留
//DI1L


//----------------------------------------
//B1电表运行状态字
//B2电网运行状态字
//B3周休日状态字
//B4波特率特征字
//B5从站异常应答帧

//------------------------------------------------------------------------------------------------------------
//Meter
extern S_Meter_Info_T Meter_Info[Meter_NUM];

//函数声明
//------------------------------------------------------------------------------------------------------------
//DLT645
//-------------------------------------------------------------------------------------------------------------
void Meter_System_Init(uint8_t i);
void EMS_Process_Data_From_Ammeter_By_UART_For_DLT645(uint8_t *data,uint8_t len);
uint8_t EMS_Perpare_To_Send_Data_To_Ammeter_For_DLT645(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID);

//end of DL/T645
//-------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------
//Modbus
//-------------------------------------------------------------------------------------------------------------
void EMS_Process_Data_From_Ammeter_By_UART_For_Modbus(uint8_t *data,uint32_t len);
uint8_t EMS_Perpare_To_Send_Data_To_Ammeter_For_Modbus(uint8_t id, uint8_t *buf, uint8_t stID, uint8_t endID);

#endif /* COM_IO_AMMETER_H_ */





