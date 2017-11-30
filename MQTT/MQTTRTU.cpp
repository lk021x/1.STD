/******************************************************************************
 * Copyright (C), 1997-2011, SUNGROW POWER SUPPLY CO., LTD.
 * File name      : MQTTRTU.c
 * Author         : xusa
 * Date           : 2014.12.22
 * Description    : mqtt协议的服务器接口部分，包括通讯层、协议解析层、应用服务层
 * Others         : None
 *******************************************************************************
 * History        :
 *------------------------------------------------------------------------------
 * 2014.12.22 : 1.0.0 : xusa
 * Modification   : 初始代码编写
 ******************************************************************************/
#include "includes.h"
#include "MQTTRTU.h"

#define UDP_MQTT_TEST 0

//链接情况
MQTT_LINK_STR MQTTLink;
 

static void MQTT_Sent_Data(uint8_t lMQTT_CMD);

const char *MQTT_user = "sys@monitor";
const char *MQTT_pass = "sys12@Smart";

const char *MQTT_PubishTopic1 = "Logger/Offline";
const char *MQTT_PubishTopic2 = "Logger/RunData";
const char *MQTT_PubishTopic3 = "Logger/FaultData";
const char *MQTT_PubishTopic4 = "Logger/HistroyData";
const char *MQTT_PubishTopic5 = "Logger/PropertyData";
const char *MQTT_PubishTopic6 = "Logger/FaultWaveData";
const char *MQTT_PubishTopic7 = "Logger/UpgradeData";
const char *MQTT_PubishTopic8 = "Logger/DiagnosisData";
const char *MQTT_PubishTopic9 = "Logger/SetPara";

//Topic1 对时
//Topic2 参数设置
//Topic3 密码设置
//Topic4 PC掉线
char MQTT_SubscribeTopic1[32];
char MQTT_SubscribeTopic2[32];
char MQTT_SubscribeTopic3[32];
char MQTT_SubscribeTopic4[32];
char MQTT_SubscribeTopic5[32];

char MQTT_PCSetParaID[64];

char MQTT_ClientID[32];
char MQTT_WillMessge[32];

// uint8_t MQTT_skey[16] = {"Sun#0*2Pow925Mdk"}; //AES128-公钥

/*******************************************以下为数据处理部分******************************************/
//计算封装为MQTT后的数据包长度
uint16_t MQTT_Encode_Length(uint16_t length)
{
	uint16_t outlen = 0;
	
	if(length < 128)
	{
		outlen = length;
	}
	else
	{
		outlen = (length%128)|0x80;
		outlen = ((length/128)<<8)|outlen;
	}
	return outlen;
}


//计算解析MQTT后的数据包长度
uint16_t MQTT_Decode_Length(uint8_t *pbuf, uint16_t length)
{
	uint16_t outlen = 0;
	
	if(length < 130)
	{
		outlen = length-2;
	}
	else
	{
		outlen = pbuf[1]&0x7F;
		outlen = outlen + ((pbuf[2]&0x7F)<<7);
	}
	return outlen;
}


//AES-128加密
void MQTT_pkg_encrypt(uint8_t *buf, uint16_t size, uint8_t *skey)
{
    int ret = -1;
    aes_encrypt_ctx g_ctx[1];
    uint8_t uOutBuf[16] = {0};

    aes_encrypt_key((const uint8_t *)skey, 16, g_ctx);
    for (uint8_t i = 0; i<(size >> 4); ++i)
    {
        ret = aes_encrypt(buf + (i << 4), uOutBuf, g_ctx);
        if(ret != 0){
        	//printf("aes encrypt err\n");
        }else{
        	memcpy(buf + (i << 4), uOutBuf, 16);
        }
    }
}

//AES-128解密
void MQTT_pkg_decrypt(uint8_t *buf, uint16_t size, uint8_t *skey)
{
    int ret = -1;
    aes_decrypt_ctx g_ctx[1];
    uint8_t uOutBuf[16] = {0};

    aes_decrypt_key((const uint8_t *)skey, 16, g_ctx);
    for (uint8_t i = 0; i<(size >> 4); ++i)
    {
        ret = aes_decrypt(buf + (i << 4), uOutBuf, g_ctx);
        if(ret != 0){
        	//printf("aes decrypt err\n");
        }else{
        	memcpy(buf + (i << 4), uOutBuf, 16);
        }
    }
}

int32_t check_data_vaild(uint8_t *pbuf)
{
	int32_t ret = 0;
	
	if((((pbuf[0]<<8)|pbuf[1]) == 0xFFFF) || (((pbuf[0]<<8)|pbuf[1]) == 0x7FFF))
	{
		ret = -1;
	}
	else
	{
		ret = 0;
	}
	
	return ret;
}

//发布设备数据
uint16_t MQTT_PUBISH_DEVICEDATA(uint8_t *pbuf)
{
	uint16_t buflen = 0, messagelen = 0, mesdatalength = 0, crc = 0, datatype = 0, datalen = 0,data_16bit = 0;
	uint8_t index = 0, data_8bit = 0;
    char devnamebuf[32] = {0};
	float  data = 0.0;
	uint32_t *rdata = (uint32_t *)&data;
	uint32_t data_32bit = 0;

	//数据封装  | 数据标签 2B | 数据类型 6b | 数量 10b |  具体数据 类型*数量 |
	uint16_t addr = 0; //数据标签
	uint16_t type = 0; //传递类型 数据类型 数量
	
	uint32_t DevStaus = 0;
	int32_t ret = 0;
	
	if(MQTTLink.linkStatus == MQTT_LINK_PUBLISH_RUNDATA) //发布实时数据
	{
		pbuf[0] = 0x32;    //发布消息 QOS = 1 retean = 1; 
		pbuf[1] = 0;       //消息长度 待定
		pbuf[2] = 0;
		
		messagelen = strlen(MQTT_PubishTopic2);
		
		pbuf[3] = 0;
		pbuf[4] = messagelen; 
		memcpy(&pbuf[5], MQTT_PubishTopic2, messagelen);
		
		pbuf[5+messagelen] = 0;
		pbuf[6+messagelen] = 1; 
						
		//数据包头
		memcpy(&pbuf[7+messagelen], gGp.devSN, 15);
		memset(&pbuf[22+messagelen], 0, 9);
		
		pbuf[31+messagelen] = 1;   //加密类型AES-128
		pbuf[32+messagelen] = 0;   //测点长度 后补
		pbuf[33+messagelen] = 0;   
		pbuf[34+messagelen] = 0;   //数据包CRC校验 后补
		pbuf[35+messagelen]= 0;
		messagelen = messagelen + 36;
		mesdatalength = 0;
		
		//数据点  50000  "SolarInfo LoggerV3"
		datatype = (MQTT_STRING_DATA << 10)&0xFC00;
		datalen  = (strlen("SolarInfo LoggerV3"))&0x03FF;		
		addr = 50000; 
		type = (datatype | datalen)&0xFFFF;	
		pbuf[messagelen + 0 + mesdatalength] =  addr&0xFF;  
		pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;	
		pbuf[messagelen + 2 + mesdatalength] =  type&0xFF;  
		pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
		memcpy(&pbuf[messagelen + 4 + mesdatalength], "SolarInfo LoggerV3", strlen("SolarInfo LoggerV3"));
		mesdatalength = mesdatalength + strlen("SolarInfo LoggerV3") + 4;
		
		
		//数据点 50001 Logger序列号
		datatype = (MQTT_STRING_DATA << 10)&0xFC00;
		datalen  = (strlen((const char*)gGp.devSN))&0x03FF;
		addr = 50001; 
		type = (datatype | datalen)&0xFFFF;
		pbuf[messagelen + 0 + mesdatalength] =  addr&0xFF;  
		pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
		pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
		pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
		memcpy(&pbuf[messagelen + 4 + mesdatalength], (const char*)gGp.devSN, strlen((const char*)gGp.devSN));
		mesdatalength = mesdatalength + strlen((const char*)gGp.devSN) + 4;
		
		
		//数据点50002 Logger当前上传时间
		datatype = (MQTT_SIMPLETIME_DATA << 10)&0xFC00;
		datalen  = 1;
		addr = 50002; 
		type = (datatype | datalen)&0xFFFF;
		pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
		pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
		pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
		pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
		pbuf[messagelen + 4 + mesdatalength] = ((2000+gGp.newtime[0])>>8);  
		pbuf[messagelen + 5 + mesdatalength] =  (2000+gGp.newtime[0])&0xFF;
		pbuf[messagelen + 6 + mesdatalength] = gGp.newtime[1];
		pbuf[messagelen + 7 + mesdatalength] = gGp.newtime[2];
		pbuf[messagelen + 8 + mesdatalength] = gGp.newtime[3];
		pbuf[messagelen + 9 + mesdatalength] = gGp.newtime[4];
		pbuf[messagelen + 10+ mesdatalength] = gGp.newtime[5];
		mesdatalength = mesdatalength + 11;
		
		//增加逻辑 
// 		if(((gGp.newtime[3]<= 4) || (gGp.newtime[3] >= 22)) && (gMpara.gcalval.TolcurP <= 1000)) 
// 		{
// 			MQTTLink.publishIndex = gMpara.sTable.devNum;
// 		}
		
		//下面上传各个设备的数据 首先传输所接入设备的数据 最后一包传输Logger自身数据
		for(uint8_t n = MQTTLink.publishIndex;  n<=gMpara.sTable.devNum;  n++)
		{
			MQTTLink.publishIndex++;
			
			if(n == gMpara.sTable.devNum) //此时传输Logger自身数据
			{
				//数据点 50003 50004 传输设备类型 及所传输设备ID
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				//设备类型
				pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_LOGGER;  
				//设备ID
				pbuf[messagelen + 5 + mesdatalength] = g_LogRead.LogAddr;
				mesdatalength = mesdatalength + 6;
				
				//数据点50005
				datatype = (MQTT_STRING_DATA << 10)&0xFC00;
				datalen  = strlen("SolarInfo LoggerV3");
				addr = 50005; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				memcpy(&pbuf[messagelen + 4 + mesdatalength], "SolarInfo LoggerV3", strlen("SolarInfo LoggerV3"));
				mesdatalength = mesdatalength + strlen("SolarInfo LoggerV3") + 4;
				
				//Logger测点 10007 10008
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 10007; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				//温度
				data = (float)(int16_t)g_LogRead.LogTemp/10;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//湿度
				data = (float)g_LogRead.LogHumi/10;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//接入设备总数 故障设备总数
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 10037; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;

				pbuf[messagelen + 0 + mesdatalength] = gMpara.gcalval.NormalDevNum;  
				pbuf[messagelen + 1 + mesdatalength] = gMpara.gcalval.FaultNum;
				mesdatalength = mesdatalength + 2;
				
				//以下为汇总遥信信息
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 1;
				addr = 10100; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//通讯状态
				pbuf[messagelen + 0 + mesdatalength] =  1;  
				mesdatalength = mesdatalength + 1;
				
				//if(gGp.Transmode == GPRS_SEND_ALSO)
				{
					//遥信测点
					datatype = (MQTT_BITSTRING_DATA << 10)&0xFC00;
					datalen  = 24;
					addr = 10101; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//数字输入1 -16
					data_16bit = ReadDIN();
					pbuf[messagelen + 0 + mesdatalength] =  data_16bit & 0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (data_16bit >> 8) &0xFF;  
					
					//数字输入16，数字输出1 - 8
					data_8bit = ReadDOUT();
					data_8bit = (data_8bit<<1)&0x06;
					pbuf[messagelen + 2 + mesdatalength] = data_8bit;  
					mesdatalength = mesdatalength + 3;
				}
				break;
			}
			else
			{
				if(gMpara.devTable[n].IsInvild == 1)   //设备在线
				{
					index = GetDevIndex(gMpara.devTable[n].DevAddr); 
					if(index != 200)
					{
						//50005所传输的设备名称
						memset(devnamebuf,0,sizeof(devnamebuf));
						devnamebuf[0] = gdevdata[index*Each_DEV_Byte+0];
						devnamebuf[1] = gdevdata[index*Each_DEV_Byte+1];
						SearchDevType(devnamebuf, INV_NAME_UNKNOW);
						
						//数据点50005
						datatype = (MQTT_STRING_DATA << 10)&0xFC00;
						datalen  = strlen(devnamebuf);
						addr = 50005; 
						type = (datatype | datalen)&0xFFFF;
						pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
						pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
						pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
						pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
						
						memcpy(&pbuf[messagelen + 4 + mesdatalength], devnamebuf, strlen(devnamebuf));
						mesdatalength = mesdatalength + strlen(devnamebuf) + 4;
						
						if(gMpara.devTable[n].DevType == DEV_INT) //逆变器数据上传信息点
						{
							//数据点 50003 50004 传输设备类型 及所传输设备ID
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 50003; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							//设备类型
							pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_INT;  
							//设备ID
							pbuf[messagelen + 5 + mesdatalength] = gMpara.devTable[n].DevAddr;
							mesdatalength = mesdatalength + 6;
							
							//逆变器测点2 3 4 5 6 7 8 9 10
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 10;
							addr = 1; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//今日发电
							data = (float)((gdevdata[index*Each_DEV_Byte+6] << 8) | gdevdata[index*Each_DEV_Byte+6])*100;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//总发电量
							data = (float)((gdevdata[index*Each_DEV_Byte+8] << 8) | gdevdata[index*Each_DEV_Byte+9] | (gdevdata[index*Each_DEV_Byte+10] << 24) | (gdevdata[index*Each_DEV_Byte+11] << 16))*1000;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//总运行时间
							data = (float)((gdevdata[index*Each_DEV_Byte+12] << 8) | gdevdata[index*Each_DEV_Byte+13] | (gdevdata[index*Each_DEV_Byte+14] << 24) | (gdevdata[index*Each_DEV_Byte+15] << 16));
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//机内空气温度
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+16] << 8) | gdevdata[index*Each_DEV_Byte+17]))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电压1
							data = (float)((gdevdata[index*Each_DEV_Byte+22] << 8) | gdevdata[index*Each_DEV_Byte+23])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电流1
							data = (float)((gdevdata[index*Each_DEV_Byte+24] << 8) | gdevdata[index*Each_DEV_Byte+25])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电压2
							data = (float)((gdevdata[index*Each_DEV_Byte+26] << 8) | gdevdata[index*Each_DEV_Byte+27])/10.0;
							
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+26]);
							if(ret < 0)
							{
								data = 0.0;
							}
							
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电流2
							data = (float)((gdevdata[index*Each_DEV_Byte+28] << 8) | gdevdata[index*Each_DEV_Byte+29])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+28]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电压3
							data = (float)((gdevdata[index*Each_DEV_Byte+30] << 8) | gdevdata[index*Each_DEV_Byte+31])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+30]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电流3
							data = (float)((gdevdata[index*Each_DEV_Byte+32] << 8) | gdevdata[index*Each_DEV_Byte+33])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+32]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//逆变器测点14
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 1;
							addr = 14; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							//总直流功率
							data = (float)((gdevdata[index*Each_DEV_Byte+34] << 8) | gdevdata[index*Each_DEV_Byte+35] | (gdevdata[index*Each_DEV_Byte+36] << 24) | (gdevdata[index*Each_DEV_Byte+37] << 16));
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							if((gdevdata[index*Each_DEV_Byte+4] == 0) && (gdevdata[index*Each_DEV_Byte+5] == 2)) //2 - 三相三线
							{
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 3;
								addr = 15; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
							}
							else
							{
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 3;
								addr = 18; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
							}
							
							//AB线电压
							data = (float)((gdevdata[index*Each_DEV_Byte+38] << 8) | gdevdata[index*Each_DEV_Byte+39])/10.0;
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//BC线电压
							data = (float)((gdevdata[index*Each_DEV_Byte+40] << 8) | gdevdata[index*Each_DEV_Byte+41])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+40]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//CA线电压
							data = (float)((gdevdata[index*Each_DEV_Byte+42] << 8) | gdevdata[index*Each_DEV_Byte+43])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+42]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//逆变器测点21 22 23 24 25 26 27 28
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 8;
							addr = 21; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							//A相电流
							data = (float)((gdevdata[index*Each_DEV_Byte+44] << 8) | gdevdata[index*Each_DEV_Byte+45])/10.0;
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//B相电流
							data = (float)((gdevdata[index*Each_DEV_Byte+46] << 8) | gdevdata[index*Each_DEV_Byte+47])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+46]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//C相电流
							data = (float)((gdevdata[index*Each_DEV_Byte+48] << 8) | gdevdata[index*Each_DEV_Byte+49])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+48]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//总有功功率
							data = (float)((gdevdata[index*Each_DEV_Byte+62] << 8) | gdevdata[index*Each_DEV_Byte+63] | (gdevdata[index*Each_DEV_Byte+64] << 24) | (gdevdata[index*Each_DEV_Byte+65] << 16));
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//总无功功率
							data = (float)((int32_t)((gdevdata[index*Each_DEV_Byte+66] << 8) | gdevdata[index*Each_DEV_Byte+67] | (gdevdata[index*Each_DEV_Byte+68] << 24) | (gdevdata[index*Each_DEV_Byte+69] << 16)));
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//总功率因素
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+70] << 8) | gdevdata[index*Each_DEV_Byte+71]))/1000;
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//电网频率
							data = (float)((gdevdata[index*Each_DEV_Byte+72] << 8) | gdevdata[index*Each_DEV_Byte+73])/10.0;
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//逆变器效率
							data = (float)((gdevdata[index*Each_DEV_Byte+74] << 8) | gdevdata[index*Each_DEV_Byte+75])/1000.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+74]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//逆变器测点39 - 50
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 12;
							addr = 39; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
// 							//模块温度1
// 							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+126] << 8) | gdevdata[index*Each_DEV_Byte+127]))/10.0;
// 							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 							mesdatalength = mesdatalength + 4;
// 							
// 							//模块温度2
// 							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+128] << 8) | gdevdata[index*Each_DEV_Byte+129]))/10.0;
// 							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 							mesdatalength = mesdatalength + 4;
// 							
// 							//模块温度3
// 							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+130] << 8) | gdevdata[index*Each_DEV_Byte+131]))/10.0;
// 							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 							mesdatalength = mesdatalength + 4;
// 							
// 							//模块温度4
// 							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+132] << 8) | gdevdata[index*Each_DEV_Byte+133]))/10.0;
// 							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 							mesdatalength = mesdatalength + 4;
// 							
// 							//模块温度5
// 							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+134] << 8) | gdevdata[index*Each_DEV_Byte+135]))/10.0;
// 							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 							mesdatalength = mesdatalength + 4;
// 							
// 							//模块温度6
// 							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+136] << 8) | gdevdata[index*Each_DEV_Byte+137]))/10.0;
// 							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 							mesdatalength = mesdatalength + 4;

							//正极对地阻抗
							data = (float)((gdevdata[index*Each_DEV_Byte+146] << 8) | gdevdata[index*Each_DEV_Byte+147] | (gdevdata[index*Each_DEV_Byte+148] << 24) | (gdevdata[index*Each_DEV_Byte+149] << 16))*10.0;
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//负极对地阻抗
							data = (float)((gdevdata[index*Each_DEV_Byte+150] << 8) | gdevdata[index*Each_DEV_Byte+151] | (gdevdata[index*Each_DEV_Byte+152] << 24) | (gdevdata[index*Each_DEV_Byte+153] << 16))*10.0;
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//限功率实际值
							data = (float)((gdevdata[index*Each_DEV_Byte+154] << 8) | gdevdata[index*Each_DEV_Byte+155] | (gdevdata[index*Each_DEV_Byte+156] << 24) | (gdevdata[index*Each_DEV_Byte+157] << 16));
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//无功调节实际值
							data = (float)((int32_t)((gdevdata[index*Each_DEV_Byte+158] << 8) | gdevdata[index*Each_DEV_Byte+159] | (gdevdata[index*Each_DEV_Byte+160] << 24) | (gdevdata[index*Each_DEV_Byte+161] << 16)));
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//视在功率
							data = (float)((gdevdata[index*Each_DEV_Byte+174] << 8) | gdevdata[index*Each_DEV_Byte+175] | (gdevdata[index*Each_DEV_Byte+176] << 24) | (gdevdata[index*Each_DEV_Byte+177] << 16));
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//日运行时间
							data = (float)((gdevdata[index*Each_DEV_Byte+226] << 8) | gdevdata[index*Each_DEV_Byte+227]);
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+226]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电压4
							data = (float)((gdevdata[index*Each_DEV_Byte+230] << 8) | gdevdata[index*Each_DEV_Byte+231])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+230]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电流4
							data = (float)((gdevdata[index*Each_DEV_Byte+232] << 8) | gdevdata[index*Each_DEV_Byte+233])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+232]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电压5
							data = (float)((gdevdata[index*Each_DEV_Byte+234] << 8) | gdevdata[index*Each_DEV_Byte+235])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+234]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电流5
							data = (float)((gdevdata[index*Each_DEV_Byte+236] << 8) | gdevdata[index*Each_DEV_Byte+237])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+236]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电压6
							data = (float)((gdevdata[index*Each_DEV_Byte+238] << 8) | gdevdata[index*Each_DEV_Byte+239])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+238]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//直流电流6
							data = (float)((gdevdata[index*Each_DEV_Byte+240] << 8) | gdevdata[index*Each_DEV_Byte+241])/10.0;
							ret = check_data_vaild(&gdevdata[index*Each_DEV_Byte+240]);
							if(ret < 0)
							{
								data = 0.0;
							}
							pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//逆变器测点70 - 79
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 16;
							addr = 70; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//支路电流
							for(uint8_t i = 0; i<16; i++)
							{
								if(((gMpara.devTable[index].InvCur[i*2] << 8) | gMpara.devTable[index].InvCur[i*2+1]) != 0xFFFF)
								{
									if(((gdevdata[index*Each_DEV_Byte+2]<<8)|gdevdata[index*Each_DEV_Byte+3]) >= 1000)//额定功率大于100kW
									{	
										data = (float)((gMpara.devTable[index].InvCur[i*2] << 8) | gMpara.devTable[index].InvCur[i*2+1])/10.0;
									}
									else
									{
										data = (float)((gMpara.devTable[index].InvCur[i*2] << 8) | gMpara.devTable[index].InvCur[i*2+1])/100.0;
									}
								}
								else
								{
									data = 0;
								}
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
							}
							
							//增加跟踪轴数据
							if(gGp.Track_type == TRAKE_ARC) 
							{
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 11;
								addr = 3000; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架配置数
								data = (float)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+12]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+13]); 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//风速
								data = (float)(((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+20]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+21])); 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//风向
								data = (float)(((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+22]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+23])); 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//目标倾角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+38]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+39]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//目标转角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+40]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+41]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架1倾角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+48]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+49]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架1转角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+50]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+51]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架2倾角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+52]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+53]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架2转角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+54]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+55]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架3倾角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+56]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+57]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架3转角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+58]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+59]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
							}
							else if(gGp.Track_type == TRAKE_SUN) 
							{
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 9;
								addr = 3011; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//设置跟踪方位角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+56]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+57]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//设置跟踪高度角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+58]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+59]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//保留
								data = (float)(((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+60]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+61]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//X电机实时值
								data = (float)(gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+62])/10.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//Y电机实时值
								data = (float)(gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+66])/10.0;  
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架实时方位角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+64]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+65]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架实时高度角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+68]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+69]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//太阳方位角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+70]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+71]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//太阳高度角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+72]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+73]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
							}

							//以下为汇总遥信信息
							//逆变器测点301 - 303
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 3;
							addr = 301; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//通讯状态
							pbuf[messagelen + 0 + mesdatalength] =  1;  
							
							//启停状态 设备故障状态
							DevStaus  = (gdevdata[index*Each_DEV_Byte+76]<<8) | gdevdata[index*Each_DEV_Byte+77];

							if((DevStaus == 0x0000)||(DevStaus == 0x5200)||(DevStaus == 0x9100)||(DevStaus == 0x8100)
							 ||(DevStaus == 0x8200)||(DevStaus == 0x4033)||(DevStaus == 0x4037)||(DevStaus == 0x8400))
							{
								pbuf[messagelen + 1 + mesdatalength] = 1;  
							}
							else
							{
								pbuf[messagelen + 1 + mesdatalength] = 0;  
							}
	
							if((DevStaus == 0x2500) || (DevStaus == 0x3600) || (DevStaus == 0x5500))
							{
								pbuf[messagelen + 2 + mesdatalength] = 1;  
							}
							else
							{
								pbuf[messagelen + 2 + mesdatalength] = 0;  
							}
							mesdatalength = mesdatalength + 3;
							
							//其他遥信测点
							datatype = (MQTT_BITSTRING_DATA << 10)&0xFC00;
							datalen  = 446;
							addr = 2455; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//大功率逆变器遥信点 故障状态1、2、工作状态、告警状态1、故障状态3
							if(((gdevdata[index*Each_DEV_Byte+2]<<8)|gdevdata[index*Each_DEV_Byte+3]) >= 1000)
							{
								//故障状态1
								data_32bit = (gdevdata[index*Each_DEV_Byte+100]<<8)|gdevdata[index*Each_DEV_Byte+101]
											|(gdevdata[index*Each_DEV_Byte+102]<<24)|(gdevdata[index*Each_DEV_Byte+103]<<16);
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//故障状态2
								data_32bit = (gdevdata[index*Each_DEV_Byte+104]<<8)|gdevdata[index*Each_DEV_Byte+105]
											|(gdevdata[index*Each_DEV_Byte+106]<<24)|(gdevdata[index*Each_DEV_Byte+107]<<16);
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//工作状态
								data_32bit = (gdevdata[index*Each_DEV_Byte+162]<<8)|gdevdata[index*Each_DEV_Byte+163]
											|(gdevdata[index*Each_DEV_Byte+164]<<24)|(gdevdata[index*Each_DEV_Byte+165]<<16);
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//告警状态
								data_32bit = (gdevdata[index*Each_DEV_Byte+182]<<8)|gdevdata[index*Each_DEV_Byte+183]
											|(gdevdata[index*Each_DEV_Byte+184]<<24)|(gdevdata[index*Each_DEV_Byte+185]<<16);
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//故障状态3
								data_32bit = (gdevdata[index*Each_DEV_Byte+108]<<8)|gdevdata[index*Each_DEV_Byte+109]
											|(gdevdata[index*Each_DEV_Byte+110]<<24)|(gdevdata[index*Each_DEV_Byte+111]<<16);
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//故障状态4
								data_32bit = (gdevdata[index*Each_DEV_Byte+112]<<8)|gdevdata[index*Each_DEV_Byte+113]
											|(gdevdata[index*Each_DEV_Byte+114]<<24)|(gdevdata[index*Each_DEV_Byte+115]<<16);
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//其余测点均为0 共有 254个点
								data_32bit = 0;
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
							}
							else
							{
								//故障状态1
								data_32bit = 0;
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//故障状态2
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//工作状态
								data_32bit = 0;
								data_16bit = (gdevdata[index*Each_DEV_Byte+76] << 8) | gdevdata[index*Each_DEV_Byte+77];
								if(data_16bit == 0) //运行
								{
									data_32bit=0x20001;
								}
								else if(data_16bit == 0x8000) //停机
								{
									data_32bit=0x0002;
								}
								else if(data_16bit == 0x1300) //按键关机
								{
									data_32bit=0x0008;
								}
								else if(data_16bit == 0x1500) //紧急停机
								{
									data_32bit=0x0020;
								}
								else if(data_16bit == 0x1400) //待机
								{
									data_32bit=0x0010;
								}
								else if(data_16bit == 0x1200) //初始待机
								{
									data_32bit=0x0002;
								}
								else if(data_16bit == 0x1600) //启动中
								{
									data_32bit=0x0040;
								}
								else if(data_16bit == 0x9100) //告警运行
								{
									data_32bit=0x20400;
								}
								else if(data_16bit == 0x8100) //降额运行
								{
									data_32bit=0x20800;
								}
								else if(data_16bit == 0x8200) //调度运行
								{
									data_32bit=0x21000;
								}
								else if(data_16bit == 0x5500) //运行故障
								{
									data_32bit=0x0200;
								}
								else if(data_16bit == 0x2500) //通讯故障
								{
									data_32bit=0x2000;
								}
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//告警状态
								data_32bit = 0;
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//故障状态3
								data_32bit = 0;
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//故障状态4
								data_32bit = 0;
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//预留1
								data_32bit = 0;
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//预留2
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;

								uint8_t buf1data[32] = {2, 3, 4, 5, 6, 7, 8, 9,10,11, 
													   12,13,14,15,16,17,19,20,21,22, 
													   23,24,25,26,27,28,29,36,37,38,
													   39,40};
								data_32bit = 0;
								data_16bit = (gdevdata[index*Each_DEV_Byte+90] << 8) | gdevdata[index*Each_DEV_Byte+91]; //状态数据1
								
								for(uint8_t i = 0; i < 32; i++)
								{
									if(data_16bit == buf1data[i])
									{
										data_32bit|=(1<<i);
									}
								}
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
													   
								uint16_t buf2data[32] = {41,42,43,44,47,48,49,50,51,70,
														 71,72,73,74,75,76,77,78,79,74,
														 80,81,87,88,89,532,533,534,535,536};
								data_32bit = 0;
								for(uint8_t i = 0; i < 20; i++)
								{
									if(data_16bit == buf2data[i])
									{
										data_32bit|=(1<<i);
									}
								}
								
								for(uint8_t i = 20; i < 30; i++)
								{
									if(data_16bit == buf2data[i])
									{
										data_32bit|=(1<<(i+2));
									}
								}
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								uint16_t buf3data[32] = {537,538,539,540,541,542,543,544,545,546,
														 547,548,549,550,551,552,553,554,555,556,
														 557,558,559,560,561,562,563,45, 46, 52,
														  53, 54};
								
								data_32bit = 0;
								for(uint8_t i = 0; i < 32; i++)
								{
									if(data_16bit == buf3data[i])
									{
										data_32bit|=(1<<i);
									}
								}
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								uint16_t buf4data[32] = {55,56,57,58,59,60,61,62,63,64,
														 65,66,67,68,69,100,101,102,103,104,
														 105,106,107,108,109,200,201,202,203,204,
														 205,306};
								
								data_32bit = 0;
								for(uint8_t i = 0; i < 32; i++)
								{
									if(data_16bit == buf4data[i])
									{
										data_32bit|=(1<<i);
									}
								}
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								uint16_t buf5data[32] = {307,315,316,82,
														83,84,85,86,300,301,302,303,304,305,
														306,307,308,309,310,311,312,313,314,315,
														316,317};
								
								data_32bit = 0;
								for(uint8_t i = 0; i < 3; i++)
								{
									if(data_16bit == buf5data[i])
									{
										data_32bit|=(1<<i);
									}
								}
								
								for(uint8_t i = 3; i < 26; i++)
								{
									if(data_16bit == buf5data[i])
									{
										data_32bit|=(1<<(i+6));
									}
								}
								
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								uint16_t buf6data[32] = {318,319,320,321,322,401,402,403,404,405,
														 406,407,408,409,500,501,502,503,504,505,
														 506,507,508,509,510,511,512,513,514,515};
								
								data_32bit = 0;
								for(uint8_t i = 0; i < 30; i++)
								{
									if(data_16bit == buf6data[i])
									{
										data_32bit|=(1<<i);
									}
								}
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
							}
							if(gGp.Track_type == TRAKE_ARC) 
							{
								//其他遥信测点
								datatype = (MQTT_BITSTRING_DATA << 10)&0xFC00;
								datalen  = 48;
								addr = 5001; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//工作模式					   
								data_16bit = (gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+18]<<8)|gdevdata[index*Each_DEV_Byte+19];
								
							    data_32bit = 0;
								
								for(uint8_t i = 0; i < 16; i++)
								{
									if(data_16bit == i)
									{
										data_32bit|=(1<<i);
									}
								}
								pbuf[messagelen + 0 + mesdatalength] = data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								mesdatalength = mesdatalength + 2;
								
								//故障状态
								 data_32bit = (gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+44] << 24) | (gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+45] << 16) 
							                | (gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+46] << 8) | gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+47]; 
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
							}
							else if(gGp.Track_type == TRAKE_SUN) 
							{
								datatype = (MQTT_BITSTRING_DATA << 10)&0xFC00;
								datalen  = 70;
								addr = 5049; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//工作模式					   
								data_16bit = (gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+54]<<8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+55];
								
							    data_32bit = 0;
								
								for(uint8_t i = 0; i < 16; i++)
								{
									if(data_16bit == i)
									{
										data_32bit|=(1<<i);
									}
								}
								pbuf[messagelen + 0 + mesdatalength] = data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//X电机状态
								data_16bit = gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+63];
								data_32bit = 0;
								
								for(uint8_t i = 0; i < 3; i++)
								{
									if(data_16bit == i)
									{
										data_32bit|=(1<<i);
									}
								}
								
								//Y电机状态
								data_16bit = gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+67];
								for(uint8_t i = 0; i < 3; i++)
								{
									if(data_16bit == i)
									{
										data_32bit|=(1<<(i+3));
									}
								}
								
								//限位传感器
								data_16bit = (gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+74] << 8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+75];
								for(uint8_t i = 0; i < 16; i++)
								{
									if((data_16bit >> i) & 0x0001)
									{
										data_32bit|=(1<<(i+6));
									}
								}
								
								//错误代码
								data_16bit = (gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+76] << 8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+77];
								for(uint8_t i = 0; i < 10; i++)
								{
									if((data_16bit >> i) & 0x0001)
									{
										data_32bit|=(1<<(i+22));
									}
								}
								pbuf[messagelen + 0 + mesdatalength] = data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//错误代码后6位
								data_16bit = (gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+76] << 8)|gdevdata[index*Each_DEV_Byte+Old_Each_DEV_Byte+77];
								data_32bit = 0;
								
								for(uint8_t i = 0; i < 6; i++)
								{
									if((data_16bit >> (i+10)) & 0x0001)
									{
										data_32bit|=(1<<i);
									}
								}
								pbuf[messagelen + 0 + mesdatalength] = data_32bit&0xFF;  
								mesdatalength = mesdatalength + 1;
							}
						}
						else if((gMpara.devTable[n].DevType == DEV_ACDC)||(gMpara.devTable[n].DevType == DEV_BOX))//直流柜或者汇流箱
						{
							//数据点 50003 50004 传输设备类型 及所传输设备ID
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 50003; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							//设备类型
							pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_BOX;  
							
							//设备ID
							pbuf[messagelen + 5 + mesdatalength] = gMpara.devTable[n].DevAddr;
							
							mesdatalength = mesdatalength + 6;
							
							//汇流箱测点1009 - 1024
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 16;
							addr = 1009; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							//第一路电流 - 第十六路电流
							for(uint8_t i = 0; i<16; i++)
							{
								data = (float)((gdevdata[index*Each_DEV_Byte+26+i*2] << 8) | gdevdata[index*Each_DEV_Byte+27+i*2])/100.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
							}
							
							//总直流电流 总直流功率
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 1005; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							data = (float)((gdevdata[index*Each_DEV_Byte+62] << 8) | gdevdata[index*Each_DEV_Byte+63] | (gdevdata[index*Each_DEV_Byte+64] << 24) | (gdevdata[index*Each_DEV_Byte+65] << 16))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							data = (float)((gdevdata[index*Each_DEV_Byte+66] << 8) | gdevdata[index*Each_DEV_Byte+67] | (gdevdata[index*Each_DEV_Byte+68] << 24) | (gdevdata[index*Each_DEV_Byte+69] << 16));
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//直流母线电压 机内温度
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 1001; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							data = (float)((gdevdata[index*Each_DEV_Byte+12] << 8) | gdevdata[index*Each_DEV_Byte+13])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
								
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+14] << 8) | gdevdata[index*Each_DEV_Byte+15])) / 10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//汇流箱遥信点1301 - 1310
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 1;
							addr = 1301; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							DevStaus = (gdevdata[index*Each_DEV_Byte+82]<<8)|gdevdata[index*Each_DEV_Byte+83];
							//通讯状态
							pbuf[messagelen + 0 + mesdatalength] =  1;	
							mesdatalength = mesdatalength + 1;
							
							//其他遥信测点
							//if(gGp.Transmode == GPRS_SEND_ALSO)
							{
								datatype = (MQTT_BITSTRING_DATA << 10)&0xFC00;
								datalen  = 25;
								addr = 1302; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								data_16bit = (gdevdata[index*Each_DEV_Byte+82]<<8)|gdevdata[index*Each_DEV_Byte+83];
								data_32bit = 0;
								
								if(data_16bit&0x0004) //防雷器故障
								{
									data_32bit|=0x0001;
								}
								
								if(data_16bit&0x0008) //机内温度过高
								{
									data_32bit|=0x0004;
								}
								
								if(data_16bit&0x0020) //短路
								{
									data_32bit|=0x0008;
								}
								
								if(data_16bit&0x0040) //电流过高
								{
									data_32bit|=0x0010;
								}
								
								if(data_16bit&0x0080) //电流过低
								{
									data_32bit|=0x0020;
								}
								
								if(data_16bit&0x0100) //开路
								{
									data_32bit|=0x0040;
								}
								
								if(data_16bit&0x0200) //电流反向
								{
									data_32bit|=0x0080;
								}
								
								if(data_16bit&0x0400) //熔丝熔断
								{
									data_32bit|=0x0100;
								}
								
								//第1路状态 到 第16路状态
								uint16_t newpvsdata[6] = {0};
								data_16bit = 0;
								
								newpvsdata[0] = (gdevdata[index*Each_DEV_Byte+86]<<8)|gdevdata[index*Each_DEV_Byte+87]; //短路数据
								newpvsdata[1] = (gdevdata[index*Each_DEV_Byte+90]<<8)|gdevdata[index*Each_DEV_Byte+91]; //反向电流数据
								newpvsdata[2] = (gdevdata[index*Each_DEV_Byte+94]<<8)|gdevdata[index*Each_DEV_Byte+95]; //电流过高数据
								newpvsdata[3] = (gdevdata[index*Each_DEV_Byte+98]<<8)|gdevdata[index*Each_DEV_Byte+99]; //熔断器熔断数据
								newpvsdata[4] = (gdevdata[index*Each_DEV_Byte+102]<<8)|gdevdata[index*Each_DEV_Byte+103]; //电流过低数据
								newpvsdata[5] = (gdevdata[index*Each_DEV_Byte+110]<<8)|gdevdata[index*Each_DEV_Byte+111]; //开路数据
								
								for(uint8_t i = 0; i < 16; i++)
								{
									//任何一种故障 都算该路故障
									if(((newpvsdata[0] >> i) & 0x01)
									|| ((newpvsdata[1] >> i) & 0x01) 
									|| ((newpvsdata[2] >> i) & 0x01)
									|| ((newpvsdata[3] >> i) & 0x01) 
									|| ((newpvsdata[4] >> i) & 0x01)
									|| ((newpvsdata[5] >> i) & 0x01))
									{
										data_16bit|=(1<<i);
									}
										
										//所有路都无故障 才算该路无故障
									else if((!(newpvsdata[0] >> i))
										  &&(!(newpvsdata[1] >> i))
										  &&(!(newpvsdata[2] >> i))
										  &&(!(newpvsdata[3] >> i))
										  &&(!(newpvsdata[4] >> i))
										  &&(!(newpvsdata[5] >> i)))
									{
										data_16bit&=(~(1<<i));
									}
								}
								
								data_32bit|=(data_16bit<<9);
								pbuf[messagelen + 0 + mesdatalength] =  data_32bit&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (data_32bit>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = (data_32bit>>16)&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (data_32bit>>24)&0xFF;
								mesdatalength = mesdatalength + 4;	
							}
						}
						else if(gMpara.devTable[n].DevType == DEV_EM)//环境监测仪
						{
							//数据点 50003 50004 传输设备类型 及所传输设备ID
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 50003; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							//设备类型
							pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_EM;  
							
							//设备ID
							pbuf[messagelen + 5 + mesdatalength] = gMpara.devTable[n].DevAddr;
							
							mesdatalength = mesdatalength + 6;
							
							if(gMpara.devTable[n].ProtoNo == Proto_AlsoWeather)
							{
								//测点 0 - 36
								datatype = (MQTT_UINT16_DATA << 10)&0xFC00;
								datalen  = 37;
								addr = 0; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								for(uint8_t i = 0; i<37; i++)
								{
									pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+3+i*2];  
									pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+2+i*2];
									mesdatalength = mesdatalength + 2;
									
								}
							}
							else
							{
								//环境监测仪测点 2002 - 2003
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 2;
								addr = 2002; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//总辐射量1
								data = (float)((gdevdata[index*Each_DEV_Byte+54] << 8) | gdevdata[index*Each_DEV_Byte+55] | (gdevdata[index*Each_DEV_Byte+56] << 24) | (gdevdata[index*Each_DEV_Byte+57] << 16))/10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//日照辐射强度1
								if(gMpara.devTable[n].ProtoNo == Proto_SR20D1EM)
								{
									data = (float)((gdevdata[index*Each_DEV_Byte+26] << 24) |(gdevdata[index*Each_DEV_Byte+27] << 16) |(gdevdata[index*Each_DEV_Byte+24] << 8) | gdevdata[index*Each_DEV_Byte+25])/100.0;
								}
								else
								{
									data = (float)((gdevdata[index*Each_DEV_Byte+26] << 8) | gdevdata[index*Each_DEV_Byte+27])/10.0;
								}
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;	
								
								//环境监测仪测点 2006 - 2007
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 2;
								addr = 2006; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//总辐射量2
								data = (float)((gdevdata[index*Each_DEV_Byte+92] << 8) | gdevdata[index*Each_DEV_Byte+93] | (gdevdata[index*Each_DEV_Byte+94] << 24) | (gdevdata[index*Each_DEV_Byte+95] << 16))/10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//日照辐射强度2
								data = (float)((gdevdata[index*Each_DEV_Byte+82] << 8) | gdevdata[index*Each_DEV_Byte+83])/10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//环境监测仪测点 2009 - 2012
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 4;
								addr = 2009; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//环境温度
								if(gMpara.devTable[n].ProtoNo == Proto_SR20D1EM)
								{
									data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+28] << 8) | gdevdata[index*Each_DEV_Byte+29])) / 100.0;
								}
								else
								{
									data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+28] << 8) | gdevdata[index*Each_DEV_Byte+29])) / 10.0;
								}
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//组件温度
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+30] << 8) | gdevdata[index*Each_DEV_Byte+31])) / 10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//风速等级
								data = (float)((gdevdata[index*Each_DEV_Byte+38] << 8) | gdevdata[index*Each_DEV_Byte+39]);
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//风向度数
								data = (float)((gdevdata[index*Each_DEV_Byte+40] << 8) | gdevdata[index*Each_DEV_Byte+41]) / 10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;

								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 2016; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//风速
								data = (float)((gdevdata[index*Each_DEV_Byte+36] << 8) | gdevdata[index*Each_DEV_Byte+37]) / 10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//环境监测仪遥信点2301
								datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 2301; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//通讯状态
								pbuf[messagelen + 0 + mesdatalength] =  1;
								
								mesdatalength = mesdatalength + 1;
							}
						}
						else if(gMpara.devTable[n].DevType == DEV_Meter)//电能表
						{
							//数据点 50003 50004 传输设备类型 及所传输设备ID
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 50003; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							//设备类型
							pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_METER;  
							
							//设备ID
							pbuf[messagelen + 5 + mesdatalength] = gMpara.devTable[n].DevAddr;
							
							mesdatalength = mesdatalength + 6;
							
							if(gMpara.devTable[n].ProtoNo == Proto_WattsOn)
							{
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 4864; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+45];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+44];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+43];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+42];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 4868; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+49];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+48];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+47];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+46];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 4870; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+53];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+52];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+51];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+50];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 512; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+57];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+56];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+55];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+54];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 514; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+61];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+60];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+59];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+58];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 516; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+65];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+64];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+63];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+62];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 518; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+69];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+68];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+67];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+66];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 520; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+73];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+72];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+71];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+70];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 522; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+77];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+76];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+75];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+74];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 524; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+81];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+80];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+79];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+78];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 544; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+85];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+84];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+83];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+82];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 546; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+89];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+88];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+87];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+86];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 548; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+93];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+92];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+91];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+90];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 550; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+97];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+96];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+95];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+94];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 552; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+101];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+100];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+99];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+98];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 554; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+105];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+104];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+103];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+102];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 556; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+109];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+108];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+107];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+106];
								mesdatalength = mesdatalength + 4;
								
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 558; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+113];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+112];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+111];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+110];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 560; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+117];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+116];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+115];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+114];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 562; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+121];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+120];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+119];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+118];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 564; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+125];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+124];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+123];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+122];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 566; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+129];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+128];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+127];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+126];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 580; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+133];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+132];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+131];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+130];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 582; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+137];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+136];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+135];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+134];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 584; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+141];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+140];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+139];
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+138];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_UINT16_DATA << 10)&0xFC00;
								datalen  = 2;
								addr = 1288; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+5];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+4];
								mesdatalength = mesdatalength + 2;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+7];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+6];
								mesdatalength = mesdatalength + 2;

								datatype = (MQTT_UINT16_DATA << 10)&0xFC00;
								datalen  = 2;
								addr = 1280; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+9];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+8];
								mesdatalength = mesdatalength + 2;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+11];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+10];
								mesdatalength = mesdatalength + 2;
							
								datatype = (MQTT_INT16_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 1296; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+25];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+24];
								mesdatalength = mesdatalength + 2;
								
								datatype = (MQTT_INT32_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 1297; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+29];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+28];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+27];  
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+26];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 1299; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+41];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+40];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+39];  
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+38];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_UINT32_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 1301; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+33];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+32];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+31];  
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+30];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_UINT32_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 1309; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+37];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+36];
								pbuf[messagelen + 2 + mesdatalength] = gdevdata[index*Each_DEV_Byte+35];  
								pbuf[messagelen + 3 + mesdatalength] = gdevdata[index*Each_DEV_Byte+34];
								mesdatalength = mesdatalength + 4;
								
								datatype = (MQTT_UINT16_DATA << 10)&0xFC00;
								datalen  = 6;
								addr = 1311; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+13];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+12];
								mesdatalength = mesdatalength + 2;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+15];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+14];
								mesdatalength = mesdatalength + 2;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+17];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+16];
								mesdatalength = mesdatalength + 2;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+19];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+18];
								mesdatalength = mesdatalength + 2;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+21];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+20];
								mesdatalength = mesdatalength + 2;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+23];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+22];
								mesdatalength = mesdatalength + 2;
								
								datatype = (MQTT_UINT16_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 1544; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								pbuf[messagelen + 0 + mesdatalength] = gdevdata[index*Each_DEV_Byte+3];  
								pbuf[messagelen + 1 + mesdatalength] = gdevdata[index*Each_DEV_Byte+2];
								mesdatalength = mesdatalength + 2;
 						    }
							else
							{
								uint16_t u16MetPT = (gdevdata[Each_DEV_Byte*index+6]<<8) | gdevdata[Each_DEV_Byte*index+7];		
								uint16_t u16MetCT = (gdevdata[Each_DEV_Byte*index+8]<<8) | gdevdata[Each_DEV_Byte*index+9];

								//电表测点 8000 - 8003
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 3;
								addr = 8000; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//A相电压
								data = (float)((gdevdata[index*Each_DEV_Byte+12] << 8) | gdevdata[index*Each_DEV_Byte+13])*u16MetPT/10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//B相电压
								data = (float)((gdevdata[index*Each_DEV_Byte+14] << 8) | gdevdata[index*Each_DEV_Byte+15])*u16MetPT/10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//C相电压
								data = (float)((gdevdata[index*Each_DEV_Byte+16] << 8) | gdevdata[index*Each_DEV_Byte+17])*u16MetPT/10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//电表测点 8006 - 8008
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 3;
								addr = 8006; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//A相电流
								data = (float)((gdevdata[index*Each_DEV_Byte+18] << 8) | gdevdata[index*Each_DEV_Byte+19])*u16MetCT/100.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//B相电流
								data = (float)((gdevdata[index*Each_DEV_Byte+20] << 8) | gdevdata[index*Each_DEV_Byte+21])*u16MetCT/100.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//C相电流
								data = (float)((gdevdata[index*Each_DEV_Byte+22] << 8) | gdevdata[index*Each_DEV_Byte+23])*u16MetCT/100.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//电表测点 8014
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 8014; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//系统功率因数
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+30] << 8) | gdevdata[index*Each_DEV_Byte+31])) / 10000.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//电表测点 8018
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 8018; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//系统有功功率
								data = (float)((int32_t)((gdevdata[index*Each_DEV_Byte+44] << 8) | gdevdata[index*Each_DEV_Byte+45] | (gdevdata[index*Each_DEV_Byte+46] << 24) | (gdevdata[index*Each_DEV_Byte+47] << 16)))*u16MetPT*u16MetCT/10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//电表测点 8022
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 8022; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//系统无功功率
								data = (float)((int32_t)((gdevdata[index*Each_DEV_Byte+60] << 8) | gdevdata[index*Each_DEV_Byte+61] | (gdevdata[index*Each_DEV_Byte+62] << 24) | (gdevdata[index*Each_DEV_Byte+63] << 16)))*u16MetPT*u16MetCT/10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//电表测点 8026
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 8026; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//系统视在功率
								data = (float)((gdevdata[index*Each_DEV_Byte+76] << 8) | gdevdata[index*Each_DEV_Byte+77] | (gdevdata[index*Each_DEV_Byte+78] << 24) | (gdevdata[index*Each_DEV_Byte+79] << 16))*u16MetPT*u16MetCT/10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//电表测点 8030 - 8033
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 4;
								addr = 8030; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//正向有功电度
								data = (float)((gdevdata[index*Each_DEV_Byte+80] << 8) | gdevdata[index*Each_DEV_Byte+81] | (gdevdata[index*Each_DEV_Byte+82] << 24) | (gdevdata[index*Each_DEV_Byte+83] << 16))*u16MetPT*u16MetCT*10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//反向有功电度
								data = (float)((gdevdata[index*Each_DEV_Byte+84] << 8) | gdevdata[index*Each_DEV_Byte+85] | (gdevdata[index*Each_DEV_Byte+86] << 24) | (gdevdata[index*Each_DEV_Byte+87] << 16))*u16MetPT*u16MetCT*10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//正向无功电度
								data = (float)((gdevdata[index*Each_DEV_Byte+88] << 8) | gdevdata[index*Each_DEV_Byte+89] | (gdevdata[index*Each_DEV_Byte+90] << 24) | (gdevdata[index*Each_DEV_Byte+91] << 16))*u16MetPT*u16MetCT*10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//反向无功电度
								data = (float)((gdevdata[index*Each_DEV_Byte+92] << 8) | gdevdata[index*Each_DEV_Byte+93] | (gdevdata[index*Each_DEV_Byte+94] << 24) | (gdevdata[index*Each_DEV_Byte+95] << 16))*u16MetPT*u16MetCT*10.0;
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								
								//电表遥信点8330
								datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 8330; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//通讯状态
								pbuf[messagelen + 0 + mesdatalength] =  1;
								
								mesdatalength = mesdatalength + 1;
							}
						}
						else if(gMpara.devTable[n].DevType == DEV_XBCK)//箱变测控
						{
							uint16_t u16XBPT = (gdevdata[Each_DEV_Byte*index+130]<<8) | gdevdata[Each_DEV_Byte*index+131];		
							uint16_t u16XBCT = (gdevdata[Each_DEV_Byte*index+132]<<8) | gdevdata[Each_DEV_Byte*index+133];
							
							//数据点 50003 50004 传输设备类型 及所传输设备ID
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 50003; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							//设备类型
							pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_XB;  
							
							//设备ID
							pbuf[messagelen + 5 + mesdatalength] = gMpara.devTable[n].DevAddr;
							
							mesdatalength = mesdatalength + 6;
							
							//箱变测点 6000 - 6020
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 21;
							addr = 6000; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//Ａ相电压
							data = (float)((gdevdata[index*Each_DEV_Byte+34] << 8) | gdevdata[index*Each_DEV_Byte+35])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//B相电压
							data = (float)((gdevdata[index*Each_DEV_Byte+36] << 8) | gdevdata[index*Each_DEV_Byte+37])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//C相电压
							data = (float)((gdevdata[index*Each_DEV_Byte+38] << 8) | gdevdata[index*Each_DEV_Byte+39])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//AB线电压
							data = (float)((gdevdata[index*Each_DEV_Byte+40] << 8) | gdevdata[index*Each_DEV_Byte+41])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//BC线电压
							data = (float)((gdevdata[index*Each_DEV_Byte+42] << 8) | gdevdata[index*Each_DEV_Byte+43])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//CA线电压
							data = (float)((gdevdata[index*Each_DEV_Byte+44] << 8) | gdevdata[index*Each_DEV_Byte+45])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//A相电流
							data = (float)((gdevdata[index*Each_DEV_Byte+46] << 8) | gdevdata[index*Each_DEV_Byte+47])*u16XBCT/1000.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//B相电流
							data = (float)((gdevdata[index*Each_DEV_Byte+48] << 8) | gdevdata[index*Each_DEV_Byte+49])*u16XBCT/1000.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//C相电流
							data = (float)((gdevdata[index*Each_DEV_Byte+50] << 8) | gdevdata[index*Each_DEV_Byte+51])*u16XBCT/1000.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//有功功率
							data = (float)((int32_t)((gdevdata[index*Each_DEV_Byte+58] << 8) | gdevdata[index*Each_DEV_Byte+59] | (gdevdata[index*Each_DEV_Byte+60] << 24) | (gdevdata[index*Each_DEV_Byte+61] << 16)))*u16XBPT/100.0*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//无功功率
							data = (float)((int32_t)((gdevdata[index*Each_DEV_Byte+62] << 8) | gdevdata[index*Each_DEV_Byte+63] | (gdevdata[index*Each_DEV_Byte+64] << 24) | (gdevdata[index*Each_DEV_Byte+65] << 16)))*u16XBPT/100.0*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//频率
							data = (float)((gdevdata[index*Each_DEV_Byte+56] << 8) | gdevdata[index*Each_DEV_Byte+57])/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//功率因素
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+54] << 8) | gdevdata[index*Each_DEV_Byte+55]))/1000.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//A相温度
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+26] << 8) | gdevdata[index*Each_DEV_Byte+27]))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//B相温度
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+28] << 8) | gdevdata[index*Each_DEV_Byte+29]))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//C相温度
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+30] << 8) | gdevdata[index*Each_DEV_Byte+31]))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//铁芯温度
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+32] << 8) | gdevdata[index*Each_DEV_Byte+33]))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//正向有功电度
							data = (float)((gdevdata[index*Each_DEV_Byte+66] << 8) | gdevdata[index*Each_DEV_Byte+67] | (gdevdata[index*Each_DEV_Byte+68] << 24) | (gdevdata[index*Each_DEV_Byte+69] << 16))*u16XBPT*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//反向有功电度
							data = (float)((gdevdata[index*Each_DEV_Byte+70] << 8) | gdevdata[index*Each_DEV_Byte+71] | (gdevdata[index*Each_DEV_Byte+72] << 24) | (gdevdata[index*Each_DEV_Byte+73] << 16))*u16XBPT*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//正向无功电度
							data = (float)((gdevdata[index*Each_DEV_Byte+74] << 8) | gdevdata[index*Each_DEV_Byte+75] | (gdevdata[index*Each_DEV_Byte+76] << 24) | (gdevdata[index*Each_DEV_Byte+77] << 16))*u16XBPT*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//反向无功电度
							data = (float)((gdevdata[index*Each_DEV_Byte+78] << 8) | gdevdata[index*Each_DEV_Byte+79] | (gdevdata[index*Each_DEV_Byte+80] << 24) | (gdevdata[index*Each_DEV_Byte+81] << 16))*u16XBPT*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//箱变测点 6030 - 6042
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 13;
							addr = 6030; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//Ａ相电压
							data = (float)((gdevdata[index*Each_DEV_Byte+82] << 8) | gdevdata[index*Each_DEV_Byte+83])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//B相电压
							data = (float)((gdevdata[index*Each_DEV_Byte+84] << 8) | gdevdata[index*Each_DEV_Byte+85])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//C相电压
							data = (float)((gdevdata[index*Each_DEV_Byte+86] << 8) | gdevdata[index*Each_DEV_Byte+87])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//AB线电压
							data = (float)((gdevdata[index*Each_DEV_Byte+88] << 8) | gdevdata[index*Each_DEV_Byte+89])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//BC线电压
							data = (float)((gdevdata[index*Each_DEV_Byte+90] << 8) | gdevdata[index*Each_DEV_Byte+91])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//CA线电压
							data = (float)((gdevdata[index*Each_DEV_Byte+92] << 8) | gdevdata[index*Each_DEV_Byte+93])*u16XBPT/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//A相电流
							data = (float)((gdevdata[index*Each_DEV_Byte+94] << 8) | gdevdata[index*Each_DEV_Byte+95])*u16XBCT/1000.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//B相电流
							data = (float)((gdevdata[index*Each_DEV_Byte+96] << 8) | gdevdata[index*Each_DEV_Byte+97])*u16XBCT/1000.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//C相电流
							data = (float)((gdevdata[index*Each_DEV_Byte+98] << 8) | gdevdata[index*Each_DEV_Byte+99])*u16XBCT/1000.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//有功功率
							data = (float)((int32_t)((gdevdata[index*Each_DEV_Byte+106] << 8) | gdevdata[index*Each_DEV_Byte+107] | (gdevdata[index*Each_DEV_Byte+108] << 24) | (gdevdata[index*Each_DEV_Byte+109] << 16)))*u16XBPT/100.0*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//无功功率
							data = (float)((int32_t)((gdevdata[index*Each_DEV_Byte+110] << 8) | gdevdata[index*Each_DEV_Byte+111] | (gdevdata[index*Each_DEV_Byte+112] << 24) | (gdevdata[index*Each_DEV_Byte+113] << 16)))*u16XBPT/100.0*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//频率
							data = (float)((gdevdata[index*Each_DEV_Byte+104] << 8) | gdevdata[index*Each_DEV_Byte+105])/100.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//功率因素
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+102] << 8) | gdevdata[index*Each_DEV_Byte+103]))/1000.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//箱变测点 6047 - 6050
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 4;
							addr = 6047; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//正向有功电度
							data = (float)((gdevdata[index*Each_DEV_Byte+114] << 8) | gdevdata[index*Each_DEV_Byte+115] | (gdevdata[index*Each_DEV_Byte+116] << 24) | (gdevdata[index*Each_DEV_Byte+117] << 16))*u16XBPT*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//反向有功电度
							data = (float)((gdevdata[index*Each_DEV_Byte+118] << 8) | gdevdata[index*Each_DEV_Byte+119] | (gdevdata[index*Each_DEV_Byte+120] << 24) | (gdevdata[index*Each_DEV_Byte+121] << 16))*u16XBPT*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//正向无功电度
							data = (float)((gdevdata[index*Each_DEV_Byte+122] << 8) | gdevdata[index*Each_DEV_Byte+123] | (gdevdata[index*Each_DEV_Byte+124] << 24) | (gdevdata[index*Each_DEV_Byte+125] << 16))*u16XBPT*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//反向无功电度
							data = (float)((gdevdata[index*Each_DEV_Byte+126] << 8) | gdevdata[index*Each_DEV_Byte+127] | (gdevdata[index*Each_DEV_Byte+128] << 24) | (gdevdata[index*Each_DEV_Byte+129] << 16))*u16XBPT*u16XBCT;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//箱变遥信点6500
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 1;
							addr = 6500; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//通讯状态
							pbuf[messagelen + 0 + mesdatalength] =  1;
							
							mesdatalength = mesdatalength + 1;
						}
						else if(gMpara.devTable[n].DevType == DEV_XBWK)//箱变温控
						{
							//数据点 50003 50004 传输设备类型 及所传输设备ID
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 50003; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							//设备类型
							pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_XB;  
							
							//设备ID
							pbuf[messagelen + 5 + mesdatalength] = gMpara.devTable[n].DevAddr;
							
							mesdatalength = mesdatalength + 6;
							
							//箱变测点 6013 - 6015
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 3;
							addr = 6013; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//A相温度
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+26] << 8) | gdevdata[index*Each_DEV_Byte+27]))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//B相温度
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+28] << 8) | gdevdata[index*Each_DEV_Byte+29]))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//C相温度
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+30] << 8) | gdevdata[index*Each_DEV_Byte+31]))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							
							//箱变遥信点6500
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 1;
							addr = 6500; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//通讯状态
							pbuf[messagelen + 0 + mesdatalength] =  1;
							
							mesdatalength = mesdatalength + 1;
						}
						else if(gMpara.devTable[n].DevType == DEV_STH)//温湿度传感器
						{
							//数据点 50003 50004 传输设备类型 及所传输设备ID
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 50003; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							//设备类型
							pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_STH;  
							
							//设备ID
							pbuf[messagelen + 5 + mesdatalength] = gMpara.devTable[n].DevAddr;
							
							mesdatalength = mesdatalength + 6;
							
							//温湿度传感器测点 2009
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 19001; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//环境温度
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+10]<<8)|gdevdata[index*Each_DEV_Byte+11]))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;

							//环境湿度
							data = (float)((gdevdata[index*Each_DEV_Byte+12]<<8)|gdevdata[index*Each_DEV_Byte+13])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//温湿度传感器遥信点2301
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 1;
							addr = 19100; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//通讯状态
							pbuf[messagelen + 0 + mesdatalength] =  1;
							
							mesdatalength = mesdatalength + 1;
						}
						else if(gMpara.devTable[n].DevType == DEV_ACPMD)   //交流配电柜
						{
							//数据点 50003 50004 传输设备类型 及所传输设备ID
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 50003; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							//设备类型
							pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_ACPMD;  
							
							//设备ID
							pbuf[messagelen + 5 + mesdatalength] = gMpara.devTable[n].DevAddr;
							
							mesdatalength = mesdatalength + 6;
							
							//交流配电柜测点
							datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
							datalen  = 13;
							addr = 20001; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//环境温度
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+10]<<8)|gdevdata[index*Each_DEV_Byte+11]))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;

							//环境湿度
							data = (float)((gdevdata[index*Each_DEV_Byte+12]<<8)|gdevdata[index*Each_DEV_Byte+13])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//环境温度档位
							data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+22]<<8)|gdevdata[index*Each_DEV_Byte+23]))/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;

							//环境湿度档位
							data = (float)((gdevdata[index*Each_DEV_Byte+24]<<8)|gdevdata[index*Each_DEV_Byte+25])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//Uab
							data = (float)((gdevdata[index*Each_DEV_Byte+42]<<8)|gdevdata[index*Each_DEV_Byte+43])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//Ubc
							data = (float)((gdevdata[index*Each_DEV_Byte+44]<<8)|gdevdata[index*Each_DEV_Byte+45])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//Uca
							data = (float)((gdevdata[index*Each_DEV_Byte+46]<<8)|gdevdata[index*Each_DEV_Byte+47])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//Uab
							data = (float)((gdevdata[index*Each_DEV_Byte+48]<<8)|gdevdata[index*Each_DEV_Byte+49])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//Ubc
							data = (float)((gdevdata[index*Each_DEV_Byte+50]<<8)|gdevdata[index*Each_DEV_Byte+51])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//Uca
							data = (float)((gdevdata[index*Each_DEV_Byte+52]<<8)|gdevdata[index*Each_DEV_Byte+53])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//Ia
							data = (float)((gdevdata[index*Each_DEV_Byte+54]<<8)|gdevdata[index*Each_DEV_Byte+55])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//Ib
							data = (float)((gdevdata[index*Each_DEV_Byte+56]<<8)|gdevdata[index*Each_DEV_Byte+57])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//Ic
							data = (float)((gdevdata[index*Each_DEV_Byte+58]<<8)|gdevdata[index*Each_DEV_Byte+59])/10.0;
							pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
							pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//遥信点
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 1;
							addr = 21001; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							mesdatalength = mesdatalength + 4;
							
							//通讯状态
							pbuf[messagelen + 0 + mesdatalength] =  1;
							
							mesdatalength = mesdatalength + 1;
						}
						else if(gMpara.devTable[n].DevType == DEV_TRAKER)   //跟踪轴
						{
							//数据点 50003 50004 传输设备类型 及所传输设备ID
							datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
							datalen  = 2;
							addr = 50003; 
							type = (datatype | datalen)&0xFFFF;
							pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
							pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
							pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
							pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
							//设备类型
							pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_TRAKER;  
							
							//设备ID
							pbuf[messagelen + 5 + mesdatalength] = gMpara.devTable[n].DevAddr;
							
							mesdatalength = mesdatalength + 6;
							
							if(gMpara.devTable[n].ProtoNo == Proto_ARC_TRAKER) //中信博
							{
								//中信博测点
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 11;
								addr = 27001; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架配置数
								data = (float)((gdevdata[index*Each_DEV_Byte+12]<<8)|gdevdata[index*Each_DEV_Byte+13]); 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//风速
								data = (float)((gdevdata[index*Each_DEV_Byte+20]<<8)|gdevdata[index*Each_DEV_Byte+21]); 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//风向
								data = (float)((gdevdata[index*Each_DEV_Byte+22]<<8)|gdevdata[index*Each_DEV_Byte+23]); 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//目标倾角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+38]<<8)|gdevdata[index*Each_DEV_Byte+39]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//目标转角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+40]<<8)|gdevdata[index*Each_DEV_Byte+41]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架1倾角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+48]<<8)|gdevdata[index*Each_DEV_Byte+49]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架1转角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+50]<<8)|gdevdata[index*Each_DEV_Byte+51]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架2倾角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+52]<<8)|gdevdata[index*Each_DEV_Byte+53]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架2转角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+54]<<8)|gdevdata[index*Each_DEV_Byte+55]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架3倾角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+56]<<8)|gdevdata[index*Each_DEV_Byte+57]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架3转角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+58]<<8)|gdevdata[index*Each_DEV_Byte+59]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//遥信点
								datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 27201; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//通讯状态
								pbuf[messagelen + 0 + mesdatalength] =  1;
								
								mesdatalength = mesdatalength + 1;
							}
							else if(gMpara.devTable[n].ProtoNo == Proto_SUN_TRAKER) //黄山睿基
							{
								//黄山睿基测点
								datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
								datalen  = 9;
								addr = 27012; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//设置跟踪方位角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+56]<<8)|gdevdata[index*Each_DEV_Byte+57]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								 //设置跟踪高度角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+58]<<8)|gdevdata[index*Each_DEV_Byte+59]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//风速
								data = (float)(((gdevdata[index*Each_DEV_Byte+60]<<8)|gdevdata[index*Each_DEV_Byte+61]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								 //X电机实时值
								data = (float)(gdevdata[index*Each_DEV_Byte+62])/10.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//Y电机实时值
								data = (float)(gdevdata[index*Each_DEV_Byte+64])/10.0;  
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//支架实时方位角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+66]<<8)|gdevdata[index*Each_DEV_Byte+67]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								 //支架实时高度角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+68]<<8)|gdevdata[index*Each_DEV_Byte+69]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//太阳方位角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+70]<<8)|gdevdata[index*Each_DEV_Byte+71]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								 //太阳高度角
								data = (float)((int16_t)((gdevdata[index*Each_DEV_Byte+72]<<8)|gdevdata[index*Each_DEV_Byte+73]))/100.0; 
								pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
								pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
								mesdatalength = mesdatalength + 4;

								//遥信点
								datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
								datalen  = 1;
								addr = 27201; 
								type = (datatype | datalen)&0xFFFF;
								pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
								pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
								pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
								pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
								mesdatalength = mesdatalength + 4;
								
								//通讯状态
								pbuf[messagelen + 0 + mesdatalength] =  1;
								
								mesdatalength = mesdatalength + 1;
							}
						}
						break;
					}
				}
			}
		}
	}
	else if(MQTTLink.linkStatus == MQTT_LINK_PUBLISH_FAULTDATA) //发布故障数据
	{
		uint8_t l_faultnum = 0;
		
		pbuf[0] = 0x32;    //发布消息 QOS = 1 retean = 1; 
		pbuf[1] = 0;       //消息长度 待定
		pbuf[2] = 0;
		
		messagelen = strlen(MQTT_PubishTopic3);
		
		pbuf[3] = 0;
		pbuf[4] = messagelen; 
		memcpy(&pbuf[5], MQTT_PubishTopic3, messagelen);
		
		pbuf[5+messagelen] = 0;
		pbuf[6+messagelen] = 1; 
						
		//AES-128加密
		memcpy(&pbuf[7+messagelen], gGp.devSN, 15);
		memset(&pbuf[22+messagelen], 0, 9);
		
		pbuf[31+messagelen] = 1;   //加密类型AES-128
		pbuf[32+messagelen] = 0;   //测点长度 后补
		pbuf[33+messagelen] = 0;   
		pbuf[34+messagelen] = 0;   //数据包CRC校验 后补
		pbuf[35+messagelen]= 0;
		messagelen = messagelen + 36;
		mesdatalength = 0;
			
		//数据点  50000  "SolarInfo LoggerV3"
		datatype = (MQTT_STRING_DATA << 10)&0xFC00;
		datalen  = (strlen("SolarInfo LoggerV3"))&0x03FF;		
		addr = 50000; 
		type = (datatype | datalen)&0xFFFF;	
		pbuf[messagelen + 0 + mesdatalength] =  addr&0xFF;  
		pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;	
		pbuf[messagelen + 2 + mesdatalength] =  type&0xFF;  
		pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
		memcpy(&pbuf[messagelen + 4 + mesdatalength], "SolarInfo LoggerV3", strlen("SolarInfo LoggerV3"));
		mesdatalength = mesdatalength + strlen("SolarInfo LoggerV3") + 4;
				
		//数据点 50001 Logger序列号
		datatype = (MQTT_STRING_DATA << 10)&0xFC00;
		datalen  = (strlen((const char*)gGp.devSN))&0x03FF;
		addr = 50001; 
		type = (datatype | datalen)&0xFFFF;
		pbuf[messagelen + 0 + mesdatalength] =  addr&0xFF;  
		pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
		pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
		pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
		memcpy(&pbuf[messagelen + 4 + mesdatalength], (const char*)gGp.devSN, strlen((const char*)gGp.devSN));
		mesdatalength = mesdatalength + strlen((const char*)gGp.devSN) + 4;
		
		//下面上传遥信数据
		for(uint16_t n = gGp.Log_MQTTReadSOE_Num;  n < gGp.Log_TotSOE_Num && l_faultnum <= 50;  n++)
		{
			ret = getLogeData(LOG_SOE_ID, gGp.Logbuf, 256, Cmd_MQTT, 0);	
	    
			if(ret > 0)
			{
				l_faultnum++;
				
				//设备类型
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 1;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] =  addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				pbuf[messagelen + 4 + mesdatalength] = gGp.Logbuf[14];
				mesdatalength = mesdatalength + 5;
				
				datatype = (MQTT_STATECHANGE_DATA << 10)&0xFC00;
				datalen  = 1;
				addr = (gGp.Logbuf[16]<<8)|gGp.Logbuf[17]; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] =  addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				
				pbuf[messagelen + 4 + mesdatalength] = ((2000+gGp.Logbuf[8])>>8);  
				pbuf[messagelen + 5 + mesdatalength] =  (2000+gGp.Logbuf[8])&0xFF;
				pbuf[messagelen + 6 + mesdatalength] = gGp.Logbuf[9];
				pbuf[messagelen + 7 + mesdatalength] = gGp.Logbuf[10];
				pbuf[messagelen + 8 + mesdatalength] = gGp.Logbuf[11];
				pbuf[messagelen + 9 + mesdatalength] = gGp.Logbuf[12];
				pbuf[messagelen + 10+ mesdatalength] = gGp.Logbuf[13];
				pbuf[messagelen + 11+ mesdatalength] = gGp.Logbuf[15];
				pbuf[messagelen + 12+ mesdatalength] = gGp.Logbuf[18];
				
				mesdatalength = mesdatalength + 13;
				break;
			}
		}
	}
	
	else if(MQTTLink.linkStatus == MQTT_LINK_PUBLISH_HISTORDATA) //发布历史数据
	{
		pbuf[0] = 0x32;    //发布消息 QOS = 1 retean = 1; 
		pbuf[1] = 0;       //消息长度 待定
		pbuf[2] = 0;
		
		messagelen = strlen(MQTT_PubishTopic4);
		
		pbuf[3] = 0;
		pbuf[4] = messagelen; 
		memcpy(&pbuf[5], MQTT_PubishTopic4, messagelen);
		
		pbuf[5+messagelen] = 0;
		pbuf[6+messagelen] = 1; 
						
		//AES-128加密
		memcpy(&pbuf[7+messagelen], gGp.devSN, 15);
		memset(&pbuf[22+messagelen], 0, 9);
		
		pbuf[31+messagelen] = 1;   //加密类型AES-128
		pbuf[32+messagelen] = 0;   //测点长度 后补
		pbuf[33+messagelen] = 0;   
		pbuf[34+messagelen] = 0;   //数据包CRC校验 后补
		pbuf[35+messagelen]= 0;
		messagelen = messagelen + 36;
		mesdatalength = 0;
			
		//数据点  50000  "SolarInfo LoggerV3"
		datatype = (MQTT_STRING_DATA << 10)&0xFC00;
		datalen  = (strlen("SolarInfo LoggerV3"))&0x03FF;		
		addr = 50000; 
		type = (datatype | datalen)&0xFFFF;	
		pbuf[messagelen + 0 + mesdatalength] =  addr&0xFF;  
		pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;	
		pbuf[messagelen + 2 + mesdatalength] =  type&0xFF;  
		pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
		memcpy(&pbuf[messagelen + 4 + mesdatalength], "SolarInfo LoggerV3", strlen("SolarInfo LoggerV3"));
		mesdatalength = mesdatalength + strlen("SolarInfo LoggerV3") + 4;
				
		//数据点 50001 Logger序列号
		datatype = (MQTT_STRING_DATA << 10)&0xFC00;
		datalen  = (strlen((const char*)gGp.devSN))&0x03FF;
		addr = 50001; 
		type = (datatype | datalen)&0xFFFF;
		pbuf[messagelen + 0 + mesdatalength] =  addr&0xFF;  
		pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
		pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
		pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
		memcpy(&pbuf[messagelen + 4 + mesdatalength], (const char*)gGp.devSN, strlen((const char*)gGp.devSN));
		mesdatalength = mesdatalength + strlen((const char*)gGp.devSN) + 4;
	
		//下面上传历史记录
		ret = getLogeData(LOG_Dev_ID, gGp.Logbuf, 256, Cmd_MQTT, 0);	
	
		if(ret > 0)
		{
			//数据点50002 Logger当前上传时间
			datatype = (MQTT_SIMPLETIME_DATA << 10)&0xFC00;
			datalen  = 1;
			addr = 50002; 
			type = (datatype | datalen)&0xFFFF;
			pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
			pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
			pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
			pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
			pbuf[messagelen + 4 + mesdatalength] = ((2000+gGp.Logbuf[21])>>8);  
			pbuf[messagelen + 5 + mesdatalength] =  (2000+gGp.Logbuf[21])&0xFF;
			pbuf[messagelen + 6 + mesdatalength] = gGp.Logbuf[22];
			pbuf[messagelen + 7 + mesdatalength] = gGp.Logbuf[23];
			pbuf[messagelen + 8 + mesdatalength] = gGp.Logbuf[24];
			pbuf[messagelen + 9 + mesdatalength] = gGp.Logbuf[25];
			pbuf[messagelen + 10+ mesdatalength] = 0;
			mesdatalength = mesdatalength + 11;
			
			//数据点50005
			if(gGp.Logbuf[8] == DEV_Logger)
			{
				datatype = (MQTT_STRING_DATA << 10)&0xFC00;
				datalen  = strlen("SolarInfo LoggerV3");
				addr = 50005; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				memcpy(&pbuf[messagelen + 4 + mesdatalength], "SolarInfo LoggerV3", strlen("SolarInfo LoggerV3"));
				mesdatalength = mesdatalength + strlen("SolarInfo LoggerV3") + 4;
			}
			else
			{
				memset(devnamebuf,0,sizeof(devnamebuf));
				devnamebuf[0] = gGp.Logbuf[45];
				devnamebuf[1] = gGp.Logbuf[46];
				SearchDevType(devnamebuf, INV_NAME_UNKNOW);
							
				datatype = (MQTT_STRING_DATA << 10)&0xFC00;
				datalen  = strlen(devnamebuf);
				addr = 50005; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				
				memcpy(&pbuf[messagelen + 4 + mesdatalength], devnamebuf, strlen(devnamebuf));
				mesdatalength = mesdatalength + strlen(devnamebuf) + 4;
			}
			if(gGp.Logbuf[8] == DEV_Logger) //Logger数据 
			{
				//数据点 50003 50004 传输设备类型 及所传输设备ID
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				//设备类型
				pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_LOGGER;  
				//设备ID
				pbuf[messagelen + 5 + mesdatalength] = gGp.Logbuf[28];
				mesdatalength = mesdatalength + 6;
				
				//Logger测点 10007 10008
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 10007; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//温度
				data = (float)((int16_t)((gGp.Logbuf[50] << 8) | gGp.Logbuf[51])) /10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//湿度
				data = (float)((gGp.Logbuf[52] << 8) | gGp.Logbuf[53]) /10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//Logger测点 接入设备总数 故障设备总数
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 10037; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;

				pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[59];  
				pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[61];
				mesdatalength = mesdatalength + 2;
			}	
			else if(gGp.Logbuf[8] == DEV_INT) //逆变器数据 
			{
				//数据点 50003 50004 传输设备类型 及所传输设备ID
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				//设备类型
				pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_INT;  
				//设备ID
				pbuf[messagelen + 5 + mesdatalength] = gGp.Logbuf[28];
				mesdatalength = mesdatalength + 6;
				
				//逆变器测点2 3 4 5 6 7 8 9 10
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 10;
				addr = 1; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//日电量
				data = (float)((gGp.Logbuf[45+6] << 8) | gGp.Logbuf[45+7])*100;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//总发电量
				data = (float)((gGp.Logbuf[45+8] << 8) | gGp.Logbuf[45+9] | (gGp.Logbuf[45+10] << 24) | (gGp.Logbuf[45+11] << 16))*1000;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//总运行时间
				data = (float)((gGp.Logbuf[45+12] << 8) | gGp.Logbuf[45+13] | (gGp.Logbuf[45+14] << 24) | (gGp.Logbuf[45+15] << 16));
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//机内空气温度
				data = (float)((int16_t)((gGp.Logbuf[45+16] << 8) | gGp.Logbuf[45+17]))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//直流电压1
				data = (float)((gGp.Logbuf[45+22] << 8) | gGp.Logbuf[45+23])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//直流电流1
				data = (float)((gGp.Logbuf[45+24] << 8) | gGp.Logbuf[45+25])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//直流电压2
				data = (float)((gGp.Logbuf[45+26] << 8) | gGp.Logbuf[45+27])/10.0;
				ret = check_data_vaild(&gGp.Logbuf[45+26]);
				if(ret < 0)
				{
					data = 0.0;
				}
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//直流电流2
				data = (float)((gGp.Logbuf[45+28] << 8) | gGp.Logbuf[45+29])/10.0;
				ret = check_data_vaild(&gGp.Logbuf[45+28]);
				if(ret < 0)
				{
					data = 0.0;
				}
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//直流电压3
				data = (float)((gGp.Logbuf[45+30] << 8) | gGp.Logbuf[45+31])/10.0;
				ret = check_data_vaild(&gGp.Logbuf[45+30]);
				if(ret < 0)
				{
					data = 0.0;
				}
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//直流电流3
				data = (float)((gGp.Logbuf[45+32] << 8) | gGp.Logbuf[45+33])/10.0;
				ret = check_data_vaild(&gGp.Logbuf[45+32]);
				if(ret < 0)
				{
					data = 0.0;
				}
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//逆变器测点14
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 1;
				addr = 14; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				//总直流功率
				data = (float)((gGp.Logbuf[45+34] << 8) | gGp.Logbuf[45+35] | (gGp.Logbuf[45+36] << 24) | (gGp.Logbuf[45+37] << 16));
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				if((gGp.Logbuf[45+4] == 0) && (gGp.Logbuf[45+5] == 2)) //线电压
				{
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 3;
					addr = 15; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
				}
				else
				{
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 3;
					addr = 18; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
				}
				//AB线电压
				data = (float)((gGp.Logbuf[45+38] << 8) | gGp.Logbuf[45+39])/10.0;
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//BC线电压
				data = (float)((gGp.Logbuf[45+40] << 8) | gGp.Logbuf[45+41])/10.0;
				ret = check_data_vaild(&gGp.Logbuf[45+40]);
				if(ret < 0)
				{
					data = 0.0;
				}
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//CA线电压
				data = (float)((gGp.Logbuf[45+42] << 8) | gGp.Logbuf[45+43])/10.0;
				ret = check_data_vaild(&gGp.Logbuf[45+42]);
				if(ret < 0)
				{
					data = 0.0;
				}
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//逆变器测点21 22 23 24 25 26 27 28
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 8;
				addr = 21; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				//A相电流
				data = (float)((gGp.Logbuf[45+44] << 8) | gGp.Logbuf[45+45])/10.0;
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//B相电流
				data = (float)((gGp.Logbuf[45+46] << 8) | gGp.Logbuf[45+47])/10.0;
				ret = check_data_vaild(&gGp.Logbuf[45+46]);
				if(ret < 0)
				{
					data = 0.0;
				}
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//C相电流
				data = (float)((gGp.Logbuf[45+48] << 8) | gGp.Logbuf[45+49])/10.0;
				ret = check_data_vaild(&gGp.Logbuf[45+48]);
				if(ret < 0)
				{
					data = 0.0;
				}
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//总有功功率
				data = (float)((gGp.Logbuf[45+62] << 8) | gGp.Logbuf[45+63] | (gGp.Logbuf[45+64] << 24) | (gGp.Logbuf[45+65] << 16));
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//总无功功率
				data = (float)((int32_t)((gGp.Logbuf[45+66] << 8) | gGp.Logbuf[45+67] | (gGp.Logbuf[45+68] << 24) | (gGp.Logbuf[45+69] << 16)));
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//总功率因素
				data = (float)((int16_t)((gGp.Logbuf[45+70] << 8) | gGp.Logbuf[45+71]))/1000;
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//电网频率
				data = (float)((gGp.Logbuf[45+72] << 8) | gGp.Logbuf[45+73])/10.0;
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//逆变器效率
				data = (float)((gGp.Logbuf[45+74] << 8) | gGp.Logbuf[45+75])/1000.0;
				ret = check_data_vaild(&gGp.Logbuf[45+74]);
				if(ret < 0)
				{
					data = 0.0;
				}
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//逆变器测点39 - 43
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 5;
				addr = 39; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
// 				//模块温度1
// 				data = (float)((int16_t)((gGp.Logbuf[45+126] << 8) | gGp.Logbuf[45+127]))/10.0;
// 				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 				mesdatalength = mesdatalength + 4;
// 				
// 				//模块温度2
// 				data = (float)((int16_t)((gGp.Logbuf[45+128] << 8) | gGp.Logbuf[45+129]))/10.0;
// 				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 				mesdatalength = mesdatalength + 4;
// 				
// 				//模块温度3
// 				data = (float)((int16_t)((gGp.Logbuf[45+130] << 8) | gGp.Logbuf[45+131]))/10.0;
// 				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 				mesdatalength = mesdatalength + 4;
// 				
// 				//模块温度4
// 				data = (float)((int16_t)((gGp.Logbuf[45+132] << 8) | gGp.Logbuf[45+133]))/10.0;
// 				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 				mesdatalength = mesdatalength + 4;
// 				
// 				//模块温度5
// 				data = (float)((int16_t)((gGp.Logbuf[45+134] << 8) | gGp.Logbuf[45+135]))/10.0;
// 				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 				mesdatalength = mesdatalength + 4;
// 				
// 				//模块温度6
// 				data = (float)((int16_t)((gGp.Logbuf[45+136] << 8) | gGp.Logbuf[45+137]))/10.0;
// 				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
// 				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
// 				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
// 				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
// 				mesdatalength = mesdatalength + 4;
				
				//正极对地阻抗
				data = (float)((gGp.Logbuf[45+146] << 8) | gGp.Logbuf[45+147] | (gGp.Logbuf[45+148] << 24) | (gGp.Logbuf[45+149] << 16))*10.0;
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//负极对地阻抗
				data = (float)((gGp.Logbuf[45+150] << 8) | gGp.Logbuf[45+151] | (gGp.Logbuf[45+152] << 24) | (gGp.Logbuf[45+153] << 16))*10.0;
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//限功率实际值
				data = (float)((gGp.Logbuf[45+154] << 8) | gGp.Logbuf[45+155] | (gGp.Logbuf[45+156] << 24) | (gGp.Logbuf[45+157] << 16));
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//无功调节实际值
				data = (float)((int32_t)((gGp.Logbuf[45+158] << 8) | gGp.Logbuf[45+159] | (gGp.Logbuf[45+160] << 24) | (gGp.Logbuf[45+161] << 16)));
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//视在功率
				data = (float)((gGp.Logbuf[45+174] << 8) | gGp.Logbuf[45+175] | (gGp.Logbuf[45+176] << 24) | (gGp.Logbuf[45+177] << 16));
				pbuf[messagelen + 0 + mesdatalength] =  (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				if(gGp.Logbuf[245] == TRAKE_ARC)
				{
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 11;
					addr = 3000; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架配置数
					data = (float)((gGp.Logbuf[30]<<8)|gGp.Logbuf[31]); 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//风速
					data = (float)(((gGp.Logbuf[32]<<8)|gGp.Logbuf[33])); 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//风向
					data = (float)(((gGp.Logbuf[34]<<8)|gGp.Logbuf[35])); 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//目标倾角
					data = (float)((int16_t)((gGp.Logbuf[36]<<8)|gGp.Logbuf[37]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//目标转角
					data = (float)((int16_t)((gGp.Logbuf[38]<<8)|gGp.Logbuf[39]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架1倾角
					data = (float)((int16_t)((gGp.Logbuf[40]<<8)|gGp.Logbuf[41]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架1转角
					data = (float)((int16_t)((gGp.Logbuf[42]<<8)|gGp.Logbuf[43]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架2倾角
					data = (float)((int16_t)((gGp.Logbuf[237]<<8)|gGp.Logbuf[238]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架2转角
					data = (float)((int16_t)((gGp.Logbuf[239]<<8)|gGp.Logbuf[240]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架3倾角
					data = (float)((int16_t)((gGp.Logbuf[241]<<8)|gGp.Logbuf[242]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架3转角
					data = (float)((int16_t)((gGp.Logbuf[243]<<8)|gGp.Logbuf[244]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
				}
				else if(gGp.Logbuf[245] == TRAKE_SUN)
				{
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 9;
					addr = 3011; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//设置跟踪方位角
					data = (float)((int16_t)((gGp.Logbuf[30]<<8)|gGp.Logbuf[31]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//设置跟踪高度角
					data = (float)((int16_t)((gGp.Logbuf[32]<<8)|gGp.Logbuf[33]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//保留
					data = (float)(((gGp.Logbuf[34]<<8)|gGp.Logbuf[35]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//X电机实时值
					data = (float)(gGp.Logbuf[37])/10.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//Y电机实时值
					data = (float)(gGp.Logbuf[39])/10.0;  
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架实时方位角
					data = (float)((int16_t)((gGp.Logbuf[40]<<8)|gGp.Logbuf[41]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架实时高度角
					data = (float)((int16_t)((gGp.Logbuf[42]<<8)|gGp.Logbuf[43]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//太阳方位角
					data = (float)((int16_t)((gGp.Logbuf[237]<<8)|gGp.Logbuf[238]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//太阳高度角
					data = (float)((int16_t)((gGp.Logbuf[239]<<8)|gGp.Logbuf[240]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
				}
			}
			else if((gGp.Logbuf[8] == DEV_ACDC)||(gGp.Logbuf[8] == DEV_BOX))//直流柜或者汇流箱
			{
				//数据点 50003 50004 传输设备类型 及所传输设备ID
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				//设备类型
				pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_BOX;  
				
				//设备ID
				pbuf[messagelen + 5 + mesdatalength] = gGp.Logbuf[28];
				
				mesdatalength = mesdatalength + 6;
				
				//汇流箱测点1009 - 1024
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 16;
				addr = 1009; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				//第一路电流 - 第十六路电流
				for(uint8_t i = 0; i<16; i++)
				{
					data = (float)((gGp.Logbuf[45+26+i*2] << 8) | gGp.Logbuf[45+27+i*2])/100.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
				}
				
				//总直流电流 总直流功率
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 1005; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				data = (float)((gGp.Logbuf[45+62] << 8) | gGp.Logbuf[45+63] | (gGp.Logbuf[45+64] << 24) | (gGp.Logbuf[45+65] << 16))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				data = (float)((gGp.Logbuf[45+66] << 8) | gGp.Logbuf[45+67] | (gGp.Logbuf[45+68] << 24) | (gGp.Logbuf[45+69] << 16));
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//直流母线电压 机内温度
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 1001; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				data = (float)((gGp.Logbuf[45+12] << 8) | gGp.Logbuf[45+13])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
					
				data = (float)((int16_t)((gGp.Logbuf[45+14] << 8) | gGp.Logbuf[45+15])) / 10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
			}
			else if(gGp.Logbuf[8] == DEV_EM)//环境监测仪
			{
				//数据点 50003 50004 传输设备类型 及所传输设备ID
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				//设备类型
				pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_EM;  
				
				//设备ID
				pbuf[messagelen + 5 + mesdatalength] = gGp.Logbuf[28]; 
				
				mesdatalength = mesdatalength + 6;
				
				if(gGp.Logbuf[29] == Proto_AlsoWeather)
				{
					//测点 0 - 36
					datatype = (MQTT_UINT16_DATA << 10)&0xFC00;
					datalen  = 37;
					addr = 0; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					for(uint8_t i = 0; i<37; i++)
					{
						pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+3+i*2];  
						pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+2+i*2];
						mesdatalength = mesdatalength + 2;
						
					}
				}
				else
				{
					//环境监测仪测点 2002 - 2003
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 2;
					addr = 2002; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//总辐射量1
					data = (float)((gGp.Logbuf[45+54] << 8) | gGp.Logbuf[45+55] | (gGp.Logbuf[45+56] << 24) | (gGp.Logbuf[45+57] << 16))/10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//日照辐射强度1
					data = (float)((gGp.Logbuf[45+26] << 8) | gGp.Logbuf[45+27])/10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//环境监测仪测点 2006 - 2007
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 2;
					addr = 2006; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//总辐射量2
					data = (float)((gGp.Logbuf[45+92] << 8) | gGp.Logbuf[45+93] | (gGp.Logbuf[45+94] << 24) | (gGp.Logbuf[45+95] << 16))/10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//日照辐射强度2
					data = (float)((gGp.Logbuf[45+82] << 8) | gGp.Logbuf[45+83])/10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//环境监测仪测点 2009 - 2012
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 4;
					addr = 2009; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//环境温度
					data = (float)((int16_t)((gGp.Logbuf[45+28] << 8) | gGp.Logbuf[45+29])) / 10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
									
					//组件温度
					data = (float)((int16_t)((gGp.Logbuf[45+30] << 8) | gGp.Logbuf[45+31])) / 10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
									
					//风速等级
					data = (float)((gGp.Logbuf[45+38] << 8) | gGp.Logbuf[45+39]);
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
									
					//风向度数
					data = (float)((gGp.Logbuf[45+40] << 8) | gGp.Logbuf[45+41]) / 10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
									
	// 				//风向位置
	// 				data = (float)((gGp.Logbuf[45+44] << 8) | gGp.Logbuf[45+45]);
	// 				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
	// 				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
	// 				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
	// 				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
	// 				mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 2016; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//风速
					data = (float)((gGp.Logbuf[45+36] << 8) | gGp.Logbuf[45+37]) / 10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
				}
			}
			else if(gGp.Logbuf[8] == DEV_Meter)//电能表
			{
				uint16_t u16MetPT = (gGp.Logbuf[45+6]<<8) | gGp.Logbuf[45+7];		
				uint16_t u16MetCT = (gGp.Logbuf[45+8]<<8) | gGp.Logbuf[45+9];

				//数据点 50003 50004 传输设备类型 及所传输设备ID
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				//设备类型
				pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_METER;  
				
				//设备ID
				pbuf[messagelen + 5 + mesdatalength] = gGp.Logbuf[28];
				
				mesdatalength = mesdatalength + 6;
				
				if(gGp.Logbuf[29] == Proto_WattsOn)
				{
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 4864; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+45];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+44];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+43];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+42];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 4868; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+49];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+48];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+47];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+46];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 4870; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+53];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+52];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+51];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+50];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 512; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+57];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+56];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+55];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+54];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 514; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+61];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+60];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+59];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+58];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 516; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+65];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+64];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+63];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+62];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 518; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+69];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+68];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+67];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+66];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 520; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+73];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+72];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+71];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+70];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 522; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+77];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+76];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+75];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+74];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 524; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+81];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+80];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+79];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+78];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 544; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+85];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+84];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+83];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+82];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 546; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+89];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+88];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+87];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+86];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 548; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+93];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+92];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+91];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+90];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 550; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+97];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+96];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+95];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+94];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 552; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+101];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+100];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+99];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+98];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 554; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+105];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+104];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+103];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+102];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 556; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+109];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+108];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+107];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+106];
					mesdatalength = mesdatalength + 4;
					
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 558; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+113];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+112];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+111];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+110];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 560; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+117];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+116];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+115];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+114];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 562; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+121];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+120];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+119];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+118];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 564; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+125];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+124];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+123];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+122];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 566; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+129];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+128];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+127];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+126];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 580; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+133];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+132];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+131];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+130];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 582; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+137];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+136];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+135];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+134];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 584; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+141];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+140];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+139];
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+138];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_UINT16_DATA << 10)&0xFC00;
					datalen  = 2;
					addr = 1288; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+5];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+4];
					mesdatalength = mesdatalength + 2;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+7];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+6];
					mesdatalength = mesdatalength + 2;

					datatype = (MQTT_UINT16_DATA << 10)&0xFC00;
					datalen  = 2;
					addr = 1280; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+9];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+8];
					mesdatalength = mesdatalength + 2;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+11];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+10];
					mesdatalength = mesdatalength + 2;
				
					datatype = (MQTT_INT16_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 1296; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+25];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+24];
					mesdatalength = mesdatalength + 2;
					
					datatype = (MQTT_INT32_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 1297; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+29];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+28];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+27];  
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+26];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 1299; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+41];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+40];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+39];  
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+38];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_UINT32_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 1301; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+33];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+32];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+31];  
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+30];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_UINT32_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 1309; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+37];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+36];
					pbuf[messagelen + 2 + mesdatalength] = gGp.Logbuf[45+35];  
					pbuf[messagelen + 3 + mesdatalength] = gGp.Logbuf[45+34];
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_UINT16_DATA << 10)&0xFC00;
					datalen  = 6;
					addr = 1311; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+13];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+12];
					mesdatalength = mesdatalength + 2;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+15];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+14];
					mesdatalength = mesdatalength + 2;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+17];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+16];
					mesdatalength = mesdatalength + 2;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+19];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+18];
					mesdatalength = mesdatalength + 2;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+21];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+20];
					mesdatalength = mesdatalength + 2;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+23];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+22];
					mesdatalength = mesdatalength + 2;
					
					datatype = (MQTT_UINT16_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 1544; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					pbuf[messagelen + 0 + mesdatalength] = gGp.Logbuf[45+3];  
					pbuf[messagelen + 1 + mesdatalength] = gGp.Logbuf[45+2];
					mesdatalength = mesdatalength + 2;
				}
				else
				{
					//电表测点 8000 - 8002
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 3;
					addr = 8000; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//A相电压
					data = (float)((gGp.Logbuf[45+12] << 8) | gGp.Logbuf[45+13])*u16MetPT/10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//B相电压
					data = (float)((gGp.Logbuf[45+14] << 8) | gGp.Logbuf[45+15])*u16MetPT/10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//C相电压
					data = (float)((gGp.Logbuf[45+16] << 8) | gGp.Logbuf[45+17])*u16MetPT/10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 3;
					addr = 8006; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//A相电流
					data = (float)((gGp.Logbuf[45+18] << 8) | gGp.Logbuf[45+19])*u16MetCT/100.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//B相电流
					data = (float)((gGp.Logbuf[45+20] << 8) | gGp.Logbuf[45+21])*u16MetCT/100.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//C相电流
					data = (float)((gGp.Logbuf[45+22] << 8) | gGp.Logbuf[45+23])*u16MetCT/100.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//电表测点 8014
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 8014; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//系统功率因数
					data = (float)((int16_t)((gGp.Logbuf[45+30] << 8) | gGp.Logbuf[45+31])) / 10000.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//电表测点 8018
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 8018; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//系统有功功率
					data = (float)((int32_t)((gGp.Logbuf[45+44] << 8) | gGp.Logbuf[45+45] | (gGp.Logbuf[45+46] << 24) | (gGp.Logbuf[45+47] << 16)))*u16MetPT*u16MetCT/10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//电表测点 8022
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 8022; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//系统无功功率
					data = (float)((int32_t)((gGp.Logbuf[45+60] << 8) | gGp.Logbuf[45+61] | (gGp.Logbuf[45+62] << 24) | (gGp.Logbuf[45+63] << 16)))*u16MetPT*u16MetCT/10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//电表测点 8026
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 1;
					addr = 8026; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//系统视在功率
					data = (float)((gGp.Logbuf[45+76] << 8) | gGp.Logbuf[45+77] | (gGp.Logbuf[45+78] << 24) | (gGp.Logbuf[45+79] << 16))*u16MetPT*u16MetCT/10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//电表测点 8030 - 8033
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 4;
					addr = 8030; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//正向有功电度
					data = (float)((gGp.Logbuf[45+80] << 8) | gGp.Logbuf[45+81] | (gGp.Logbuf[45+82] << 24) | (gGp.Logbuf[45+83] << 16))*u16MetPT*u16MetCT*10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//反向有功电度
					data = (float)((gGp.Logbuf[45+84] << 8) | gGp.Logbuf[45+85] | (gGp.Logbuf[45+86] << 24) | (gGp.Logbuf[45+87] << 16))*u16MetPT*u16MetCT*10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//正向无功电度
					data = (float)((gGp.Logbuf[45+88] << 8) | gGp.Logbuf[45+89] | (gGp.Logbuf[45+90] << 24) | (gGp.Logbuf[45+91] << 16))*u16MetPT*u16MetCT*10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					
					//反向无功电度
					data = (float)((gGp.Logbuf[45+92] << 8) | gGp.Logbuf[45+93] | (gGp.Logbuf[45+94] << 24) | (gGp.Logbuf[45+95] << 16))*u16MetPT*u16MetCT*10.0;
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
				}
			}
			else if(gGp.Logbuf[8] == DEV_XBCK)//箱变测控
			{			
				uint16_t u16XBPT = (gGp.Logbuf[45+130]<<8) | gGp.Logbuf[45+131];		
				uint16_t u16XBCT = (gGp.Logbuf[45+132]<<8) | gGp.Logbuf[45+133];

				//数据点 50003 50004 传输设备类型 及所传输设备ID
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				//设备类型
				pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_XB;  
				
				//设备ID
				pbuf[messagelen + 5 + mesdatalength] = gGp.Logbuf[28];
				
				mesdatalength = mesdatalength + 6;
				
				//箱变测点 6000 - 6020
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 21;
				addr = 6000; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//Ａ相电压
				data = (float)((gGp.Logbuf[45+34] << 8) | gGp.Logbuf[45+35])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//B相电压
				data = (float)((gGp.Logbuf[45+36] << 8) | gGp.Logbuf[45+37])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//C相电压
				data = (float)((gGp.Logbuf[45+38] << 8) | gGp.Logbuf[45+39])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//AB线电压
				data = (float)((gGp.Logbuf[45+40] << 8) | gGp.Logbuf[45+41])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//BC线电压
				data = (float)((gGp.Logbuf[45+42] << 8) | gGp.Logbuf[45+43])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//CA线电压
				data = (float)((gGp.Logbuf[45+44] << 8) | gGp.Logbuf[45+45])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//A相电流
				data = (float)((gGp.Logbuf[45+46] << 8) | gGp.Logbuf[45+47])*u16XBCT/1000.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//B相电流
				data = (float)((gGp.Logbuf[45+48] << 8) | gGp.Logbuf[45+49])*u16XBCT/1000.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//C相电流
				data = (float)((gGp.Logbuf[45+50] << 8) | gGp.Logbuf[45+51])*u16XBCT/1000.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//有功功率
				data = (float)((int32_t)((gGp.Logbuf[45+58] << 8) | gGp.Logbuf[45+59] | (gGp.Logbuf[45+60] << 24) | (gGp.Logbuf[45+61] << 16)))*u16XBPT/100.0*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//无功功率
				data = (float)((int32_t)((gGp.Logbuf[45+62] << 8) | gGp.Logbuf[45+63] | (gGp.Logbuf[45+64] << 24) | (gGp.Logbuf[45+65] << 16)))*u16XBPT/100.0*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//频率
				data = (float)((gGp.Logbuf[45+56] << 8) | gGp.Logbuf[45+57])/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//功率因素
				data = (float)((int16_t)((gGp.Logbuf[45+54] << 8) | gGp.Logbuf[45+55]))/1000.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//A相温度
				data = (float)((int16_t)((gGp.Logbuf[45+26] << 8) | gGp.Logbuf[45+27]))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//B相温度
				data = (float)((int16_t)((gGp.Logbuf[45+28] << 8) | gGp.Logbuf[45+29]))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//C相温度
				data = (float)((int16_t)((gGp.Logbuf[45+30] << 8) | gGp.Logbuf[45+31]))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//铁芯温度
				data = (float)((int16_t)((gGp.Logbuf[45+32] << 8) | gGp.Logbuf[45+33]))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//正向有功电度
				data = (float)((gGp.Logbuf[45+66] << 8) | gGp.Logbuf[45+67] | (gGp.Logbuf[45+68] << 24) | (gGp.Logbuf[45+69] << 16))*u16XBPT*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//反向有功电度
				data = (float)((gGp.Logbuf[45+70] << 8) | gGp.Logbuf[45+71] | (gGp.Logbuf[45+72] << 24) | (gGp.Logbuf[45+73] << 16))*u16XBPT*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//正向无功电度
				data = (float)((gGp.Logbuf[45+74] << 8) | gGp.Logbuf[45+75] | (gGp.Logbuf[45+76] << 24) | (gGp.Logbuf[45+77] << 16))*u16XBPT*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//反向无功电度
				data = (float)((gGp.Logbuf[45+78] << 8) | gGp.Logbuf[45+79] | (gGp.Logbuf[45+80] << 24) | (gGp.Logbuf[45+81] << 16))*u16XBPT*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//箱变测点 6030 - 6042
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 13;
				addr = 6030; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//Ａ相电压
				data = (float)((gGp.Logbuf[45+82] << 8) | gGp.Logbuf[45+83])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//B相电压
				data = (float)((gGp.Logbuf[45+84] << 8) | gGp.Logbuf[45+85])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//C相电压
				data = (float)((gGp.Logbuf[45+86] << 8) | gGp.Logbuf[45+87])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//AB线电压
				data = (float)((gGp.Logbuf[45+88] << 8) | gGp.Logbuf[45+89])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//BC线电压
				data = (float)((gGp.Logbuf[45+90] << 8) | gGp.Logbuf[45+91])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//CA线电压
				data = (float)((gGp.Logbuf[45+92] << 8) | gGp.Logbuf[45+93])*u16XBPT/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//A相电流
				data = (float)((gGp.Logbuf[45+94] << 8) | gGp.Logbuf[45+95])*u16XBCT/1000.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//B相电流
				data = (float)((gGp.Logbuf[45+96] << 8) | gGp.Logbuf[45+97])*u16XBCT/1000.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//C相电流
				data = (float)((gGp.Logbuf[45+98] << 8) | gGp.Logbuf[45+99])*u16XBCT/1000.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//有功功率
				data = (float)((int32_t)((gGp.Logbuf[45+106] << 8) | gGp.Logbuf[45+107] | (gGp.Logbuf[45+108] << 24) | (gGp.Logbuf[45+109] << 16)))*u16XBPT/100.0*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//无功功率
				data = (float)((int32_t)((gGp.Logbuf[45+110] << 8) | gGp.Logbuf[45+111] | (gGp.Logbuf[45+112] << 24) | (gGp.Logbuf[45+113] << 16)))*u16XBPT/100.0*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//频率
				data = (float)((gGp.Logbuf[45+104] << 8) | gGp.Logbuf[45+105])/100.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//功率因素
				data = (float)((int16_t)((gGp.Logbuf[45+102] << 8) | gGp.Logbuf[45+103]))/1000.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//箱变测点 6047 - 6050
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 4;
				addr = 6047; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//正向有功电度
				data = (float)((gGp.Logbuf[45+114] << 8) | gGp.Logbuf[45+115] | (gGp.Logbuf[45+116] << 24) | (gGp.Logbuf[45+117] << 16))*u16XBPT*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//反向有功电度
				data = (float)((gGp.Logbuf[45+118] << 8) | gGp.Logbuf[45+119] | (gGp.Logbuf[45+120] << 24) | (gGp.Logbuf[45+121] << 16))*u16XBPT*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//正向无功电度
				data = (float)((gGp.Logbuf[45+122] << 8) | gGp.Logbuf[45+123] | (gGp.Logbuf[45+124] << 24) | (gGp.Logbuf[45+125] << 16))*u16XBPT*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//反向无功电度
				data = (float)((gGp.Logbuf[45+126] << 8) | gGp.Logbuf[45+127] | (gGp.Logbuf[45+128] << 24) | (gGp.Logbuf[45+129] << 16))*u16XBPT*u16XBCT;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
			}
			else if(gGp.Logbuf[8] == DEV_XBWK)//箱变温控
			{			
				//数据点 50003 50004 传输设备类型 及所传输设备ID
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				//设备类型
				pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_XB;  
				
				//设备ID
				pbuf[messagelen + 5 + mesdatalength] = gGp.Logbuf[28];
				
				mesdatalength = mesdatalength + 6;
				
				//箱变测点 6013 - 6015
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 3;
				addr = 6013; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//A相温度
				data = (float)((int16_t)((gGp.Logbuf[45+26] << 8) | gGp.Logbuf[45+27]))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//B相温度
				data = (float)((int16_t)((gGp.Logbuf[45+28] << 8) | gGp.Logbuf[45+29]))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//C相温度
				data = (float)((int16_t)((gGp.Logbuf[45+30] << 8) | gGp.Logbuf[45+31]))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
			}
			else if(gGp.Logbuf[8] == DEV_STH)//温湿度传感器
			{			
				//数据点 50003 50004 传输设备类型 及所传输设备ID
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				//设备类型
				pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_STH;  
				
				//设备ID
				pbuf[messagelen + 5 + mesdatalength] = gGp.Logbuf[28];
				
				mesdatalength = mesdatalength + 6;
				
				//温湿度传感器测点 2009
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 19001; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//环境温度
				data = (float)((int16_t)((gGp.Logbuf[45+10]<<8)|gGp.Logbuf[45+11]))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;

				//环境湿度
				data = (float)((gGp.Logbuf[45+12]<<8)|gGp.Logbuf[45+13])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
			}
			else if(gGp.Logbuf[8] == DEV_ACPMD)//交流配电柜
			{			
				//数据点 50003 50004 传输设备类型 及所传输设备ID
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				//设备类型
				pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_ACPMD;  
				
				//设备ID
				pbuf[messagelen + 5 + mesdatalength] = gGp.Logbuf[28];
				
				mesdatalength = mesdatalength + 6;
				
				//交流配电柜测点
				datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
				datalen  = 13;
				addr = 20001; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//环境温度
				data = (float)((int16_t)((gGp.Logbuf[45+10]<<8)|gGp.Logbuf[45+11]))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;

				//环境湿度
				data = (float)((gGp.Logbuf[45+12]<<8)|gGp.Logbuf[45+13])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//环境温度档位
				data = (float)((int16_t)((gGp.Logbuf[45+22]<<8)|gGp.Logbuf[45+23]))/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;

				//环境湿度档位
				data = (float)((gGp.Logbuf[45+24]<<8)|gGp.Logbuf[45+25])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//AB线电压
				data = (float)((gGp.Logbuf[45+42]<<8)|gGp.Logbuf[45+43])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//BC线电压
				data = (float)((gGp.Logbuf[45+44]<<8)|gGp.Logbuf[45+45])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//CA线电压
				data = (float)((gGp.Logbuf[45+46]<<8)|gGp.Logbuf[45+47])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				
				//AB线电压
				data = (float)((gGp.Logbuf[45+48]<<8)|gGp.Logbuf[45+49])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//BC线电压
				data = (float)((gGp.Logbuf[45+50]<<8)|gGp.Logbuf[45+51])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//CA线电压
				data = (float)((gGp.Logbuf[45+52]<<8)|gGp.Logbuf[45+53])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//Ia线电压
				data = (float)((gGp.Logbuf[45+54]<<8)|gGp.Logbuf[45+55])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//Ib线电压
				data = (float)((gGp.Logbuf[45+56]<<8)|gGp.Logbuf[45+57])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
				
				//Ic线电压
				data = (float)((gGp.Logbuf[45+58]<<8)|gGp.Logbuf[45+59])/10.0;
				pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
				pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
				mesdatalength = mesdatalength + 4;
			}
			else if(gGp.Logbuf[8] == DEV_TRAKER)//交流配电柜
			{			
				//数据点 50003 50004 传输设备类型 及所传输设备ID
				datatype = (MQTT_UINT8_DATA << 10)&0xFC00;
				datalen  = 2;
				addr = 50003; 
				type = (datatype | datalen)&0xFFFF;
				pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
				pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
				pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
				pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
				//设备类型
				pbuf[messagelen + 4 + mesdatalength] = MQTT_DEV_TRAKER;  
				
				//设备ID
				pbuf[messagelen + 5 + mesdatalength] = gGp.Logbuf[28];
				
				mesdatalength = mesdatalength + 6;
				
				if(gGp.Logbuf[29] == Proto_ARC_TRAKER)
				{
					//中信博测点
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 11;
					addr = 27001; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架配置数
					data = (float)((gGp.Logbuf[45+12]<<8)|gGp.Logbuf[45+13]); 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//风速
					data = (float)((gGp.Logbuf[45+20]<<8)|gGp.Logbuf[45+21]); 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//风向
					data = (float)((gGp.Logbuf[45+22]<<8)|gGp.Logbuf[45+23]); 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//目标倾角
					data = (float)((int16_t)((gGp.Logbuf[45+38]<<8)|gGp.Logbuf[45+39]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//目标转角
					data = (float)((int16_t)((gGp.Logbuf[45+40]<<8)|gGp.Logbuf[45+41]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架1倾角
					data = (float)((int16_t)((gGp.Logbuf[45+48]<<8)|gGp.Logbuf[45+49]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架1转角
					data = (float)((int16_t)((gGp.Logbuf[45+50]<<8)|gGp.Logbuf[45+51]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架2倾角
					data = (float)((int16_t)((gGp.Logbuf[45+52]<<8)|gGp.Logbuf[45+53]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架2转角
					data = (float)((int16_t)((gGp.Logbuf[45+54]<<8)|gGp.Logbuf[45+55]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架3倾角
					data = (float)((int16_t)((gGp.Logbuf[45+56]<<8)|gGp.Logbuf[45+57]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架3转角
					data = (float)((int16_t)((gGp.Logbuf[45+58]<<8)|gGp.Logbuf[45+59]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
				}
				else if(gGp.Logbuf[29] == Proto_SUN_TRAKER)
				{
					//黄山睿基测点
					datatype = (MQTT_FLOAT_DATA << 10)&0xFC00;
					datalen  = 9;
					addr = 27012; 
					type = (datatype | datalen)&0xFFFF;
					pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
					pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//设置跟踪方位角
					data = (float)((int16_t)((gGp.Logbuf[45+56]<<8)|gGp.Logbuf[45+57]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					 //设置跟踪高度角
					data = (float)((int16_t)((gGp.Logbuf[45+58]<<8)|gGp.Logbuf[45+59]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//风速
					data = (float)(((gGp.Logbuf[45+60]<<8)|gGp.Logbuf[45+61]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					 //X电机实时值
					data = (float)(gGp.Logbuf[45+62])/10.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//Y电机实时值
					data = (float)(gGp.Logbuf[45+64])/10.0;  
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//支架实时方位角
					data = (float)((int16_t)((gGp.Logbuf[45+66]<<8)|gGp.Logbuf[45+67]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					 //支架实时高度角
					data = (float)((int16_t)((gGp.Logbuf[45+68]<<8)|gGp.Logbuf[45+69]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					//太阳方位角
					data = (float)((int16_t)((gGp.Logbuf[45+70]<<8)|gGp.Logbuf[45+71]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
					
					 //太阳高度角
					data = (float)((int16_t)((gGp.Logbuf[45+72]<<8)|gGp.Logbuf[45+73]))/100.0; 
					pbuf[messagelen + 0 + mesdatalength] = (*rdata)&0xFF;  
					pbuf[messagelen + 1 + mesdatalength] = ((*rdata)>>8)&0xFF;
					pbuf[messagelen + 2 + mesdatalength] = ((*rdata)>>16)&0xFF;
					pbuf[messagelen + 3 + mesdatalength] = ((*rdata)>>24)&0xFF;
					mesdatalength = mesdatalength + 4;
				}
			}
		}
	}
	else if((MQTTLink.linkStatus == MQTT_LINK_PUBLISH_SETDATA) || (MQTTLink.linkStatus == MQTT_LINK_PUBLISH_SETPASSWORD))
	{
		pbuf[0] = 0x32;    //发布消息 QOS = 1 retean = 0; 
		pbuf[1] = 0;       //消息长度 待定
		pbuf[2] = 0;
		
		messagelen = strlen(MQTT_PubishTopic9);
		
		pbuf[3] = 0;
		pbuf[4] = messagelen; 
		memcpy(&pbuf[5], MQTT_PubishTopic9, messagelen);
		
		pbuf[5+messagelen] = 0;
		pbuf[6+messagelen] = 1; 
						
		//AES-128加密
		memcpy(&pbuf[7+messagelen], gGp.devSN, 15);
		memset(&pbuf[22+messagelen], 0, 9);
		
		pbuf[31+messagelen] = 1;   //加密类型AES-128
		pbuf[32+messagelen] = 0;   //测点长度 后补
		pbuf[33+messagelen] = 0;   
		pbuf[34+messagelen] = 0;   //数据包CRC校验 后补
		pbuf[35+messagelen]= 0;
		messagelen = messagelen + 36;
		mesdatalength = 0;
			
		//数据点  50000  "SolarInfo LoggerV3"
		datatype = (MQTT_STRING_DATA << 10)&0xFC00;
		datalen  = (strlen("SolarInfo LoggerV3"))&0x03FF;		
		addr = 50000; 
		type = (datatype | datalen)&0xFFFF;	
		pbuf[messagelen + 0 + mesdatalength] =  addr&0xFF;  
		pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;	
		pbuf[messagelen + 2 + mesdatalength] =  type&0xFF;  
		pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
		memcpy(&pbuf[messagelen + 4 + mesdatalength], "SolarInfo LoggerV3", strlen("SolarInfo LoggerV3"));
		mesdatalength = mesdatalength + strlen("SolarInfo LoggerV3") + 4;
		
		//数据点 50001 Logger序列号
		datatype = (MQTT_STRING_DATA << 10)&0xFC00;
		datalen  = (strlen((const char*)gGp.devSN))&0x03FF;
		addr = 50001; 
		type = (datatype | datalen)&0xFFFF;
		pbuf[messagelen + 0 + mesdatalength] =  addr&0xFF;  
		pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
		pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
		pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
		memcpy(&pbuf[messagelen + 4 + mesdatalength], (const char*)gGp.devSN, strlen((const char*)gGp.devSN));
		mesdatalength = mesdatalength + strlen((const char*)gGp.devSN) + 4;
		
		//数据点 50006 设定参数的结果
		datatype = (MQTT_STRING_DATA << 10)&0xFC00;
		addr = 50006; 
		datalen  = (strlen(MQTT_PCSetParaID)+1)&0x03FF;
		type = (datatype | datalen)&0xFFFF;
		
		pbuf[messagelen + 0 + mesdatalength] = addr&0xFF;  
		pbuf[messagelen + 1 + mesdatalength] = (addr>>8)&0xFF;
		pbuf[messagelen + 2 + mesdatalength] = type&0xFF;  
		pbuf[messagelen + 3 + mesdatalength] = (type>>8)&0xFF;
		
		if(MQTTLink.parasetresult == 1)
		{
			pbuf[messagelen + 4 + mesdatalength] = 0x31;
		}
		else
		{
			pbuf[messagelen + 4 + mesdatalength] = 0x30;
		}
		memcpy(&pbuf[messagelen + 5 + mesdatalength], MQTT_PCSetParaID, strlen(MQTT_PCSetParaID));
		mesdatalength = mesdatalength + strlen(MQTT_PCSetParaID) + 5;
	}
	//汇总报文 
	if(messagelen >= 12)
	{
		pbuf[messagelen - 4] =   mesdatalength&0xFF; 							
		pbuf[messagelen - 3] = (mesdatalength>>8)&0xFF; 

		//AES-128加密
		if(mesdatalength%16)
		{
			memset(&pbuf[messagelen + 0 + mesdatalength], 0xFF, 16 - (mesdatalength%16));
			mesdatalength = mesdatalength + 16 - (mesdatalength%16);
		}
		
		crc = getCRC16(&pbuf[messagelen + 0], mesdatalength);

		pbuf[messagelen - 2] =  crc&0xFF;   
		pbuf[messagelen - 1] = (crc>>8)&0xFF;
		
		if(gGp.Transmode == GPRS_SEND_ALSO)
		{
			MQTT_pkg_encrypt(&pbuf[messagelen + 0], mesdatalength, MQTTLink.publickey); //AES-128加密 test
		}
		else
		{
			MQTT_pkg_encrypt(&pbuf[messagelen + 0], mesdatalength, MQTTLink.aes128key); //AES-128加密 test
		}
		messagelen =  messagelen + mesdatalength;

		//获取消息长度
		buflen = messagelen - 3; 

		messagelen = MQTT_Encode_Length(buflen);

		if(messagelen >= 256) 
		{ 
			pbuf[1] =  messagelen & 0xFF;    
			pbuf[2] = (messagelen >> 8)& 0xFF;
			buflen = buflen + 3;
		}
		else
		{
			pbuf[1] =  messagelen & 0xFF;     
			memcpy(&pbuf[2], &pbuf[3], messagelen);
			buflen = buflen + 2;
		}
	}
	return buflen;
}

uint32_t u32MQTTSetvalue = 0;

//MQTT协议数据处理部分:接收数据
static void MQTT_Receive_Data(uint8_t *pbuf, uint16_t buflen)
{
	uint8_t  beginindex = 0;     //获取数据区起始地址 =2或者=3
	uint8_t  databeginindex = 0; //实际数据起始偏移
	uint8_t  cmd = (pbuf[0]>>4)&0x0F;
	uint16_t length = MQTT_Decode_Length(pbuf, buflen); //获取数据区长度
	if(length >= buflen)
	{
		return;
	}
	
	uint16_t datareg   = 0;   //设定的数据寄存器  
	uint16_t datatype  = 0;   //设定的数据类型
	uint16_t devsetreg = 0;   //设定的寄存器地址
	uint16_t lentemp  = 0;    //设定的数据实际偏移
	uint16_t crcresult=0;     //CRC校验值
	uint8_t  devaddr  = 0;    //设定设备地址
	uint8_t  rdbIndex = 0;    //设备的偏移
	uint8_t  tmp[3] = {0};    //CRC计算值
				
	float *fp = (float *)&u32MQTTSetvalue;

	beginindex = buflen - length; //获取数据区起始地址 =2或者=3
	databeginindex = 0;
	
	MQTTLink.pingtime = gGp.SysTickPoint;
	MQTTLink.pingnum = 0;
	
	switch(cmd)
	{
		case MQTT_CONNACK:  //链接应答
			if((pbuf[1] == 0x02)&&(pbuf[3] == 0x00)) //链接成功
			{
				MQTT_Sent_Data(MQTT_SUBSCRIBE); //启动订阅
			}
			else //链接失败
			{
				MQTT_Link_Close();
			}
			break;
		case MQTT_SUBACK:  //收到订阅应答 无动作
			if(gGp.Log_TotDev_Num  > gGp.Log_MQTTReadDev_Num)
			{
				MQTTLink.linkStatus = MQTT_LINK_PUBLISH_HISTORDATA;
			}
			else if(gGp.Log_TotSOE_Num  > gGp.Log_MQTTReadSOE_Num)
			{
				MQTTLink.linkStatus = MQTT_LINK_PUBLISH_FAULTDATA;
			}
			else
			{
				MQTTLink.linkStatus = MQTT_LINK_PUBLISH_RUNDATA;
			}
			MQTTLink.PCState = MQTT_PC_ONLINE;
			MQTT_Sent_Data(MQTT_PUBLISH);
			break;
		case MQTT_PUBACK:  //收到发布应答 
			MQTTLink.publishtime = gGp.SysTickPoint;
			if(MQTTLink.linkStatus == MQTT_LINK_PUBLISH_RUNDATA) //实时数据发送
			{
				MQTTLink.publishDevno++;
				
				if((MQTTLink.publishDevno > gMpara.gcalval.NormalDevNum) || (MQTTLink.publishIndex > gMpara.sTable.devNum)) //传输完毕
				{
					MQTTLink.publishDevno = 0;
					MQTTLink.publishIndex = 0;
					MQTTLink.linkStatus = MQTT_LINK_PING;
				}
				MQTTLink.publishResult = MQTT_SEND_SUCESS;
			}
			else if(MQTTLink.linkStatus == MQTT_LINK_PUBLISH_FAULTDATA) //历史故障数据发送
			{
				BSP_MQTT_DATA_LOCK();
				gGp.Log_MQTTReadSOE_Num++;
				write_eeprom(Log_MQTTREADSOE_Addr, &gGp.Log_MQTTReadSOE_Num, 2);
				BSP_MQTT_DATA_UNLOCK();
				if(gGp.Log_TotSOE_Num  <= gGp.Log_MQTTReadSOE_Num) //历史故障已经发送完毕
				{
					MQTTLink.linkStatus = MQTT_LINK_PING;
				}
				MQTTLink.publishResult = MQTT_SEND_SUCESS;
			}
			else if(MQTTLink.linkStatus == MQTT_LINK_PUBLISH_HISTORDATA) //发送历史数据
			{
				BSP_MQTT_DATA_LOCK();
				gGp.Log_MQTTReadDev_Num++;
				write_eeprom(Log_MQTTREADDEV_Addr, &gGp.Log_MQTTReadDev_Num, 2);
				BSP_MQTT_DATA_UNLOCK();
				if(gGp.Log_TotDev_Num  <= gGp.Log_MQTTReadDev_Num) //历史数据已经发送完毕
				{
					if(MQTTLink.MQTTStart == 0xAA)
					{
						MQTTLink.linkStatus = MQTT_LINK_PUBLISH_RUNDATA;
						MQTTLink.MQTTStart = 0;
					}
					else
					{
						MQTTLink.linkStatus = MQTT_LINK_PING;
					}
				}
				MQTTLink.publishResult = MQTT_SEND_SUCESS;
			}
			else if(MQTTLink.linkStatus == MQTT_LINK_PUBLISH_SETDATA) //设置参数
			{
				MQTTLink.linkStatus = MQTT_LINK_PING;
			}
			else if(MQTTLink.linkStatus == MQTT_LINK_PUBLISH_SETPASSWORD) //设置密码
            {
				if(MQTTLink.PCState == MQTT_PC_ONLINE)
				{
					if(gGp.Log_TotSOE_Num > gGp.Log_MQTTReadSOE_Num)
					{
						MQTTLink.linkStatus = MQTT_LINK_PUBLISH_FAULTDATA;
					}
					else if(gGp.Log_TotDev_Num > gGp.Log_MQTTReadDev_Num)
					{
						MQTTLink.linkStatus = MQTT_LINK_PUBLISH_HISTORDATA;
					}
					else
					{
						MQTTLink.linkStatus = MQTT_LINK_PUBLISH_RUNDATA; 
					}
				}
				else
				{
					MQTTLink.linkStatus = MQTT_LINK_PING;
				}
			}				
			
			MQTTLink.DUP = 0;
			if(MQTTLink.linkStatus == MQTT_LINK_PING)
			{
				MQTT_Sent_Data(MQTT_PINGREQ);
			}
			else
			{
				MQTT_Sent_Data(MQTT_PUBLISH);
			}
			break;
		case MQTT_PUBLISH: //接收到云平台发布的消息 主题 PC/Time PC/Pass PC/SetPara 
			MQTTLink.SUBmsgID = (pbuf[beginindex+2+pbuf[beginindex+1]]<<8)|pbuf[beginindex+3+pbuf[beginindex+1]]; //获取message ID
			if((pbuf[0]&0x06) > 0)
			{
				MQTT_Sent_Data(MQTT_PUBACK);  //应答云平台的发布消息
				databeginindex = beginindex+4+pbuf[beginindex+1];
			}
			else
			{
				databeginindex = beginindex+2+pbuf[beginindex+1];
			}
			
			if(strncmp((char *)&pbuf[beginindex+2], MQTT_SubscribeTopic1, strlen(MQTT_SubscribeTopic1)) == 0) //收到对时
			{
				//AES-128解密
				if(gGp.Transmode == GPRS_SEND_ALSO)
				{
					MQTT_pkg_decrypt(&pbuf[databeginindex+5], (buflen - databeginindex-5), MQTTLink.publickey);
				}
				else
				{
					MQTT_pkg_decrypt(&pbuf[databeginindex+5], (buflen - databeginindex-5), MQTTLink.aes128key);
				}
				
				//CRC校验
				lentemp = (buflen - databeginindex-5);
				
				crcresult = getCRC16(&pbuf[databeginindex+5], lentemp);
				tmp[0] = crcresult & 0xff;
				tmp[1] = (crcresult >> 8) & 0xff;
			
				if ((pbuf[databeginindex+3] == tmp[0])&&(pbuf[databeginindex+4] == tmp[1])) //CRC校验成功
				{
					datareg  = pbuf[databeginindex+5] | (pbuf[databeginindex+6] << 8);
					datatype = pbuf[databeginindex+7] | (pbuf[databeginindex+8] << 8);
					
					if((datareg == 0xCB20) && (datatype == 0x3C01))
					{
						uint16_t u16year = (pbuf[databeginindex+9] <<8)|pbuf[databeginindex+10];
						uint8_t newtime[6] = { 0 };

						if(u16year > 2000)
						{
							pbuf[databeginindex+10] = u16year-2000;
							
							if(gGp.Transmode == GPRS_SEND_ALSO) //Also下发的对时信息 需要考虑时区
							{
								Synchronization_Time_Zone(&pbuf[databeginindex+10]);
							}
							
							newtime[0] = ((pbuf[databeginindex+10]/10) << 4) + (pbuf[databeginindex+10]%10);
							newtime[1] = ((pbuf[databeginindex+11]/10) << 4) + (pbuf[databeginindex+11]%10);
							newtime[2] = ((pbuf[databeginindex+12]/10) << 4) + (pbuf[databeginindex+12]%10);
							newtime[3] = ((pbuf[databeginindex+13]/10) << 4) + (pbuf[databeginindex+13]%10);
							newtime[4] = ((pbuf[databeginindex+14]/10) << 4) + (pbuf[databeginindex+14]%10);
							newtime[5] = ((pbuf[databeginindex+15]/10) << 4) + (pbuf[databeginindex+15]%10);
							
							//同步到主进程 更新时间	
							NET_MSG_LOCK();
							g_net_msg.cmd=TCP_TIME_Data;
							g_net_msg.len=6;
							memcpy(g_net_msg.data, newtime, 6);
							NET_MSG_UNLOCK();
							bsp_os_queue_send(&g_msg_Net_handle, (void*)&g_net_msg, sizeof(TProcessMsg));
						}
					}
				}
				MQTTLink.PCState = MQTT_PC_ONLINE;
			}
			else if(strncmp((char *)&pbuf[beginindex+2], MQTT_SubscribeTopic2, strlen(MQTT_SubscribeTopic2)) == 0) //收到设定SetPara
			{
				MQTTLink.parasetresult = 0;
				memset(MQTT_PCSetParaID, 0x00, sizeof(MQTT_PCSetParaID));
				//AES-128解密
				if(gGp.Transmode == GPRS_SEND_ALSO)
				{
					MQTT_pkg_decrypt(&pbuf[databeginindex+5], (buflen - databeginindex-5), MQTTLink.publickey);
				}
				else
				{
					MQTT_pkg_decrypt(&pbuf[databeginindex+5], (buflen - databeginindex-5), MQTTLink.aes128key);
				}
				
				
				//CRC校验
				lentemp = (buflen - databeginindex-5);
				
				crcresult = getCRC16(&pbuf[databeginindex+5], lentemp);
				tmp[0] = crcresult & 0xff;
				tmp[1] = (crcresult >> 8) & 0xff;
			
				if ((pbuf[databeginindex+3] == tmp[0])&&(pbuf[databeginindex+4] == tmp[1])) //CRC校验成功
				{
					//参数设置主题使用
					datareg  = pbuf[databeginindex+5] | (pbuf[databeginindex+6] << 8);
					datatype = pbuf[databeginindex+7] | (pbuf[databeginindex+8] << 8);
					lentemp = datatype&0x03FF;
					
					if(datareg == 52005) //设定参数信息的ID信息
					{
						memcpy(MQTT_PCSetParaID, &pbuf[databeginindex+9], lentemp); //保存PC参数设置的ID信息
					}
					
					datareg  = pbuf[databeginindex+9+lentemp] | (pbuf[databeginindex+10+lentemp] << 8);
					datatype = pbuf[databeginindex+11+lentemp]| (pbuf[databeginindex+12+lentemp] << 8);
					
					if(datareg == 52006)//设定Logger所接入设备的地址
					{
						devaddr = pbuf[databeginindex+13+lentemp];
						
						devsetreg = pbuf[databeginindex+14+lentemp] | (pbuf[databeginindex+15+lentemp] << 8);
						datatype  = pbuf[databeginindex+16+lentemp] | (pbuf[databeginindex+17+lentemp] << 8);
						
						if(devsetreg == 52004)
						{
							if(((datatype >>10)&0x3F) == MQTT_STRING_DATA)
							{
								MQTTLink.parasetresult = 1;
							}
							else
							{
								MQTTLink.parasetresult = 0;
							}
						}
						else
						{
							//目前仅支持2种数据类型：浮点型及uint8_t
							if(((datatype >>10)&0x3F) == MQTT_FLOAT_DATA)
							{
								u32MQTTSetvalue = (pbuf[databeginindex+18+lentemp] | (pbuf[databeginindex+19+lentemp] << 8)
										 | (pbuf[databeginindex+20+lentemp] << 16) | (pbuf[databeginindex+21+lentemp] << 24))&0xFFFFFFFF;
							}
							else
							{
								u32MQTTSetvalue = pbuf[databeginindex+18+lentemp];
							}
							
							if(devaddr == g_LogRead.LogAddr) //设定Logger参数
							{
							}
							else
							{
								rdbIndex = GetRdbIndex(devaddr); //根据设备地址获取到偏移
								if(rdbIndex < MAX_DEV_NUM)
								{
									if(gMpara.devTable[rdbIndex].IsInvild == 1)
									{
										if(gMpara.devTable[rdbIndex].DevType == DEV_INT) //对逆变器进行设置
										{
											if(devsetreg == 401) //开关机
											{
												if(u32MQTTSetvalue == 0) //关机
												{
													gMpara.devTable[rdbIndex].DevOnOff = 0xCE;
													gGp.MQTTSetIndex = rdbIndex;
													gGp.MQTTSetFlag = 0xAA;
												}
												else if(u32MQTTSetvalue == 1) //开机
												{
													gMpara.devTable[rdbIndex].DevOnOff = 0xCF;
													gGp.MQTTSetIndex = rdbIndex;
													gGp.MQTTSetFlag = 0xAA;
												}
												else
												{
													MQTTLink.parasetresult = 0;
												}
											}
											else if(devsetreg == 702) //有功功率实际值设置
											{
												gMpara.devTable[rdbIndex].setP = (int)((*fp)/100);
												gMpara.devTable[rdbIndex].PowSwitch = 0xAA;
												gGp.MQTTSetIndex = rdbIndex;
												gGp.MQTTSetFlag = 0xAA;
											}
											else if(devsetreg == 703) //无功功率实际值设置
											{
												gMpara.devTable[rdbIndex].setQ = (int)((*fp)/100);
												gMpara.devTable[rdbIndex].FacSwitch = 0xA2;
												gGp.MQTTSetIndex = rdbIndex;
												gGp.MQTTSetFlag = 0xAA;
											}
											
											else if(devsetreg == 706) //有功功率比例值设置
											{
												gMpara.devTable[rdbIndex].Limit = (int)((*fp)*10);
												gMpara.devTable[rdbIndex].PowSwitch = 0xAA;
												gGp.MQTTSetIndex = rdbIndex;
												gGp.MQTTSetFlag = 0xAA;
											}
											
											else if(devsetreg == 707) //功率因素
											{
												gMpara.devTable[rdbIndex].Factor = (int)((*fp)*1000);
												gMpara.devTable[rdbIndex].FacSwitch = 0xA1;
												gGp.MQTTSetIndex = rdbIndex;
												gGp.MQTTSetFlag = 0xAA;
											}
											else if(devsetreg == 708) //无功比例设置
											{
												gMpara.devTable[rdbIndex].LimitRec = (int)((*fp)*10);
												gMpara.devTable[rdbIndex].FacSwitch = 0xA2;
												gGp.MQTTSetIndex = rdbIndex;
												gGp.MQTTSetFlag = 0xAA;
											}
											else
											{
												MQTTLink.parasetresult = 0;
											}
										}
										else
										{
											MQTTLink.parasetresult = 0;
										}
									}
									else
									{
										MQTTLink.parasetresult = 0;
									}
								}
								else
								{
									MQTTLink.parasetresult = 0;
								}
							}
						}
					}
					else
					{
						MQTTLink.parasetresult = 0;
					}
		
				}
				else
				{
					MQTTLink.parasetresult = 0;
				}
				MQTTLink.PCState = MQTT_PC_ONLINE;
				MQTTLink.linkStatus = MQTT_LINK_PUBLISH_SETDATA; 
				MQTTLink.DUP = 0;
				MQTT_Sent_Data(MQTT_PUBLISH);
			}
			else if(strncmp((char *)&pbuf[beginindex+2], MQTT_SubscribeTopic3, strlen(MQTT_SubscribeTopic3)) == 0)  //收到密钥信息
			{
				MQTTLink.parasetresult = 0;
				memset(MQTT_PCSetParaID, 0x00, sizeof(MQTT_PCSetParaID));
				//AES-128解密
				MQTT_pkg_decrypt(&pbuf[databeginindex+5], (buflen - databeginindex-5), MQTTLink.publickey);
				
				//CRC校验
				lentemp = (buflen - databeginindex-5);
				
				crcresult = getCRC16(&pbuf[databeginindex+5], lentemp);
				tmp[0] = crcresult & 0xff;
				tmp[1] = (crcresult >> 8) & 0xff;
			
				if ((pbuf[databeginindex+3] == tmp[0])&&(pbuf[databeginindex+4] == tmp[1])) //CRC校验成功
				{
					//参数设置主题使用
					datareg  = pbuf[databeginindex+5] | (pbuf[databeginindex+6] << 8);
					datatype = pbuf[databeginindex+7] | (pbuf[databeginindex+8] << 8);
					lentemp = datatype&0x03FF;
					
					if(datareg == 52005) //设定参数信息的ID信息
					{
						memcpy(MQTT_PCSetParaID, &pbuf[databeginindex+9], lentemp); //保存PC参数设置的ID信息
						datareg  = pbuf[databeginindex+9+lentemp] | (pbuf[databeginindex+10+lentemp] << 8);
						datatype = pbuf[databeginindex+11+lentemp]| (pbuf[databeginindex+12+lentemp] << 8);
						
						if(datareg == 52003)//设定Logger所接入设备的地址
						{
							memcpy(MQTTLink.aes128key, &pbuf[databeginindex+13+lentemp], 16);
							write_eeprom(MQTT_PRIVATE_KEY_Addr, MQTTLink.aes128key, 16);
							MQTTLink.parasetresult = 1;
						}
					}
					else if(datareg == 52003)//设定Logger所接入设备的地址
					{
						memcpy(MQTTLink.aes128key, &pbuf[databeginindex+9], 16);
						write_eeprom(MQTT_PRIVATE_KEY_Addr, MQTTLink.aes128key, 16);
						MQTTLink.parasetresult = 1;
					}
				}
				MQTTLink.PCState = MQTT_PC_ONLINE;
				MQTTLink.linkStatus = MQTT_LINK_PUBLISH_SETPASSWORD; 
				MQTTLink.DUP = 0;
				MQTT_Sent_Data(MQTT_PUBLISH);
			}
			else if(strncmp((char *)&pbuf[beginindex+2], MQTT_SubscribeTopic4, strlen(MQTT_SubscribeTopic4)) == 0)  //PC掉线
			{
				//保存历史数据
				MQTTLink.PCState = MQTT_PC_OFFLINE;
				MQTTLink.publishResult = MQTT_SEND_FAULT;
				
				MQTTLink.DUP = 0;
				MQTTLink.linkStatus = MQTT_LINK_PING;
				MQTT_Sent_Data(MQTT_PINGREQ);
			}
			else if(strncmp((char *)&pbuf[beginindex+2], MQTT_SubscribeTopic5, strlen(MQTT_SubscribeTopic5)) == 0)  //电站信息 电子大屏使用
			{
				//AES-128解密
				if(gGp.Transmode == GPRS_SEND_ALSO)
				{
					MQTT_pkg_decrypt(&pbuf[databeginindex+5], (buflen - databeginindex-5), MQTTLink.publickey);
				}
				else
				{
					MQTT_pkg_decrypt(&pbuf[databeginindex+5], (buflen - databeginindex-5), MQTTLink.aes128key);
				}
			
				//CRC校验
				lentemp = (buflen - databeginindex-5);
				
				crcresult = getCRC16(&pbuf[databeginindex+5], lentemp);
				tmp[0] = crcresult & 0xff;
				tmp[1] = (crcresult >> 8) & 0xff;
			
				if ((pbuf[databeginindex+3] == tmp[0])&&(pbuf[databeginindex+4] == tmp[1])) //CRC校验成功
				{
					for(uint8_t i = 0; i < 4; i++)
					{
						datareg  = pbuf[databeginindex+5+i*8] | (pbuf[databeginindex+6+i*8] << 8);
						datatype = pbuf[databeginindex+7+i*8] | (pbuf[databeginindex+8+i*8] << 8);
			            
						u32MQTTSetvalue = (pbuf[databeginindex+9+i*8]|(pbuf[databeginindex+10+i*8]<<8)
								   |(pbuf[databeginindex+11+i*8]<<16)|(pbuf[databeginindex+12+i*8]<<24))&0xFFFFFFFF;
						
						if(datareg == 52007) 
						{
							gMpara.gcalval.TolcurP = (*fp);
						}
						else if(datareg == 52008) 
						{
							gMpara.gcalval.daykwh = (*fp)/100;
						}
						else if(datareg == 52009) 
						{
							gMpara.gcalval.TolKwh = (*fp)/1000;
						}
						else if(datareg == 52010) 
						{
							gMpara.gcalval.TolCO2 = (*fp)/1000;
						}
					}	
				}
			}
			break;
		case MQTT_PINGRESP:
			break;
		default:
			break;
	}
	bsp_os_watch_data_feed_by_net(); //网络进程收到数据
}


//MQTT协议数据处理部分:发送数据
static void MQTT_Sent_Data(uint8_t lMQTT_CMD)
{
	uint16_t messagelen = 0;
	switch (lMQTT_CMD)
	{
		case MQTT_CONNECT:
			MQTTLink.sendBuf[0] = 0x10;   //消息类型：CONNECT 
		
			MQTTLink.sendBuf[1] = 0;      //消息长度  预留
			MQTTLink.sendBuf[2] = 0; 
		
			messagelen = 3;
		
			MQTTLink.sendBuf[messagelen+0] = 0;      //可变头部：长度
			MQTTLink.sendBuf[messagelen+1] = 0x06;
			MQTTLink.sendBuf[messagelen+2] = 'M';    //可变头部：内容
			MQTTLink.sendBuf[messagelen+3] = 'Q';
			MQTTLink.sendBuf[messagelen+4] = 'I';
			MQTTLink.sendBuf[messagelen+5] = 's';
			MQTTLink.sendBuf[messagelen+6] = 'd';
			MQTTLink.sendBuf[messagelen+7] = 'p';
			MQTTLink.sendBuf[messagelen+8] = 0x03;  //协议版本
		
		    MQTTLink.sendBuf[messagelen+9] = 0xEE;  //链接标志：user=1,pass=1,Qos=1,will falg=1, Clean Session = 1 CE
		    
			MQTTLink.sendBuf[messagelen+10]= 0x00;  //链接保持时间：2分钟
		    MQTTLink.sendBuf[messagelen+11]= 0x78;  
			
			messagelen = messagelen+12;
	
		    MQTTLink.sendBuf[messagelen+0] = 0;     //负载 消息体 Client ID
		    MQTTLink.sendBuf[messagelen+1] = strlen(MQTT_ClientID); 
			memcpy(&MQTTLink.sendBuf[messagelen+2], MQTT_ClientID, strlen(MQTT_ClientID));
			
			messagelen = messagelen+2+strlen(MQTT_ClientID);
			
			MQTTLink.sendBuf[messagelen+0] = 0;     //负载 消息体 will topic
		    MQTTLink.sendBuf[messagelen+1] = strlen(MQTT_PubishTopic1); 
			memcpy(&MQTTLink.sendBuf[messagelen+2], MQTT_PubishTopic1, strlen(MQTT_PubishTopic1));
			
			messagelen = messagelen+2+strlen(MQTT_PubishTopic1);
			
			MQTTLink.sendBuf[messagelen+0] = 0;     //负载 消息体 Will Message
		    MQTTLink.sendBuf[messagelen+1] = strlen(MQTT_WillMessge); 
			memcpy(&MQTTLink.sendBuf[messagelen+2], MQTT_WillMessge, strlen(MQTT_WillMessge));
			
			messagelen = messagelen+2+strlen(MQTT_WillMessge);

			MQTTLink.sendBuf[messagelen+0] = 0;     //负载 消息体 User Name
		    MQTTLink.sendBuf[messagelen+1] = strlen(MQTT_user); 
			memcpy(&MQTTLink.sendBuf[messagelen+2], MQTT_user, strlen(MQTT_user));
			
			messagelen = messagelen+2+strlen(MQTT_user);
			
			MQTTLink.sendBuf[messagelen+0] = 0;     //负载 消息体 Password
		    MQTTLink.sendBuf[messagelen+1] = strlen(MQTT_pass); 
			memcpy(&MQTTLink.sendBuf[messagelen+2], MQTT_pass, strlen(MQTT_pass));
			
			messagelen = messagelen+2+strlen(MQTT_pass);
			
			MQTTLink.sendlen = messagelen - 3; 

			messagelen = MQTT_Encode_Length(MQTTLink.sendlen);
			
			if(messagelen >= 256) //两位数据长度
			{
				MQTTLink.sendBuf[1] =  messagelen & 0xFF;      //消息长度 
				MQTTLink.sendBuf[2] = (messagelen >> 8)& 0xFF;
				MQTTLink.sendlen = MQTTLink.sendlen + 3;
			}
			else
			{
				MQTTLink.sendBuf[1] =  messagelen & 0xFF;      //消息长度 
				memcpy(&MQTTLink.sendBuf[2], &MQTTLink.sendBuf[3], MQTTLink.sendlen);
				MQTTLink.sendlen = MQTTLink.sendlen + 2;
			}
			
			//数据封装完毕 启动发送
			tcp_write(MQTTLink.linkHandle, MQTTLink.sendBuf, MQTTLink.sendlen, TCP_WRITE_FLAG_COPY);
			tcp_output(MQTTLink.linkHandle);
			break;
			
		case MQTT_SUBSCRIBE: //订阅 QOS都为1
			MQTTLink.sendBuf[0] = 0x82;   //消息类型：SUBSCRIBE 
		
			MQTTLink.sendBuf[1] = 0;      //消息长度  预留
			MQTTLink.sendBuf[2] = 0; 
		
			messagelen = 3;
		
			MQTTLink.sendBuf[messagelen+0] = 0;      //可变头部：Message ID 暂定为255
			MQTTLink.sendBuf[messagelen+1] = 0xFF; 
			
			messagelen = messagelen+2;
		
		    MQTTLink.sendBuf[messagelen+0] = 0;     //负载 消息体 SN/Time
		    MQTTLink.sendBuf[messagelen+1] = strlen(MQTT_SubscribeTopic1);
			memcpy(&MQTTLink.sendBuf[messagelen+2], MQTT_SubscribeTopic1, strlen(MQTT_SubscribeTopic1));
			MQTTLink.sendBuf[messagelen+2+strlen(MQTT_SubscribeTopic1)] = 1; //QOS
			messagelen = messagelen+3+strlen(MQTT_SubscribeTopic1);
			
		
			MQTTLink.sendBuf[messagelen+0] = 0;     //负载 消息体 SN/SetPara
		    MQTTLink.sendBuf[messagelen+1] = strlen(MQTT_SubscribeTopic2);
			memcpy(&MQTTLink.sendBuf[messagelen+2], MQTT_SubscribeTopic2, strlen(MQTT_SubscribeTopic2));
			MQTTLink.sendBuf[messagelen+2+strlen(MQTT_SubscribeTopic2)] = 1; //QOS
			messagelen = messagelen+3+strlen(MQTT_SubscribeTopic2);
			
			MQTTLink.sendBuf[messagelen+0] = 0;     //负载 消息体 SN/Password
		    MQTTLink.sendBuf[messagelen+1] = strlen(MQTT_SubscribeTopic3);
			memcpy(&MQTTLink.sendBuf[messagelen+2], MQTT_SubscribeTopic3, strlen(MQTT_SubscribeTopic3));
			MQTTLink.sendBuf[messagelen+2+strlen(MQTT_SubscribeTopic3)] = 1; //QOS
			messagelen = messagelen+3+strlen(MQTT_SubscribeTopic3);
			
			MQTTLink.sendBuf[messagelen+0] = 0;     //负载 消息体 PC/Offline
		    MQTTLink.sendBuf[messagelen+1] = strlen(MQTT_SubscribeTopic4);
			memcpy(&MQTTLink.sendBuf[messagelen+2], MQTT_SubscribeTopic4, strlen(MQTT_SubscribeTopic4));
			MQTTLink.sendBuf[messagelen+2+strlen(MQTT_SubscribeTopic4)] = 1; //QOS
			messagelen = messagelen+3+strlen(MQTT_SubscribeTopic4);

			MQTTLink.sendlen = messagelen - 3; 
			messagelen = MQTT_Encode_Length(MQTTLink.sendlen);
			
			if(messagelen >= 256) //两位数据长度
			{
				MQTTLink.sendBuf[1] =  messagelen & 0xFF;      //消息长度 
				MQTTLink.sendBuf[2] = (messagelen >> 8)& 0xFF;
				MQTTLink.sendlen = MQTTLink.sendlen + 3;
			}
			else
			{
				MQTTLink.sendBuf[1] =  messagelen & 0xFF;      //消息长度 
				memcpy(&MQTTLink.sendBuf[2], &MQTTLink.sendBuf[3], MQTTLink.sendlen);
				MQTTLink.sendlen = MQTTLink.sendlen + 2;
			}
			
			//数据封装完毕 启动发送
			tcp_write(MQTTLink.linkHandle, MQTTLink.sendBuf, MQTTLink.sendlen, TCP_WRITE_FLAG_COPY);
			tcp_output(MQTTLink.linkHandle);
			break;
			
		case MQTT_PUBLISH:  //发布消息 首先发布设备实时数据
			MQTTLink.sendlen = MQTT_PUBISH_DEVICEDATA(MQTTLink.sendBuf);
			if(MQTTLink.sendlen > 0)
			{
				MQTTLink.sendBuf[0] = (MQTTLink.sendBuf[0] | (MQTTLink.DUP<<3))&0xFF;
				
				//数据封装完毕 启动发送
				tcp_write(MQTTLink.linkHandle, MQTTLink.sendBuf, MQTTLink.sendlen, TCP_WRITE_FLAG_COPY);
				tcp_output(MQTTLink.linkHandle);
			}
			break;
		case MQTT_PUBACK:	
			MQTTLink.sendBuf[0] = 0x40;   //消息类型：MQTT_PUBACK 
		
			MQTTLink.sendBuf[1] = 0x02;      //消息长度 
			MQTTLink.sendBuf[2] = (MQTTLink.SUBmsgID >> 8)&0xFF; 
			MQTTLink.sendBuf[3] =  MQTTLink.SUBmsgID&0xFF; 
		    MQTTLink.sendlen = 4;
			//数据封装完毕 启动发送
			tcp_write(MQTTLink.linkHandle, MQTTLink.sendBuf, MQTTLink.sendlen, TCP_WRITE_FLAG_COPY);
			tcp_output(MQTTLink.linkHandle);
			break;
		case MQTT_PINGREQ:
			MQTTLink.sendBuf[0] = 0xC0;   //消息类型：MQTT_PINGREQ 
			MQTTLink.sendBuf[1] = 0x00; 
		    MQTTLink.sendlen = 2;
			//数据封装完毕 启动发送
			tcp_write(MQTTLink.linkHandle, MQTTLink.sendBuf, MQTTLink.sendlen, TCP_WRITE_FLAG_COPY);
			tcp_output(MQTTLink.linkHandle);
			MQTTLink.pingtime = gGp.SysTickPoint;
			break;
		case MQTT_DISCONNECT:
			MQTTLink.sendBuf[0] = 0xE0;   //消息类型：MQTT_DISCONNECT 
			MQTTLink.sendBuf[1] = 0x00; 
		    MQTTLink.sendlen = 2;
			//数据封装完毕 启动发送
			tcp_write(MQTTLink.linkHandle, MQTTLink.sendBuf, MQTTLink.sendlen, TCP_WRITE_FLAG_COPY);
			tcp_output(MQTTLink.linkHandle);
		    //断开连接
			MQTT_Link_Close();
			break;
		default:
			break;
	}

	if(gGp.UDP_MQTT_PAK == 0xAA)
	{
		udp_send_PPC(MQTTLink.sendBuf, MQTTLink.sendlen);
	}
}


/*******************************************以下为网络链接部分******************************************/

#define MQTT_CLIENT_SET_CALLBACK_NULL(pcb)   \
do{                                         \
    tcp_arg((pcb),  NULL);                  \
    tcp_recv((pcb), NULL);                  \
    tcp_err((pcb),  NULL);                  \
    tcp_poll((pcb), NULL, 0);               \
    tcp_sent((pcb), NULL);                  \
}while(0)

#define MQTT_CLIENT_CLOSE_TCP(pcb)       \
do{                                     \
    MQTT_CLIENT_SET_CALLBACK_NULL(pcb);  \
    tcp_close((pcb));                   \
}while(0)

//链接初始化
static void MQTTLink_Init(void)
{
	MQTTLink.linkHandle = NULL;
	MQTTLink.linkStatus = MQTT_LINK_NULL;
	MQTTLink.publishIndex = 0;
	MQTTLink.publishResult= MQTT_SEND_NULL;
	MQTTLink.pingnum = 0;
	MQTTLink.PCState = MQTT_PC_OFFLINE;
	MQTTLink.MQTTStart = 0xAA;
}

//链接关闭处理
void MQTT_Link_Close(void)
{
	if(MQTTLink.linkHandle != NULL)
	{
		MQTT_CLIENT_CLOSE_TCP(MQTTLink.linkHandle);
		MQTTLink_Init();
	}
}


//链接失败处理
static void MQTT_Link_Error(void *arg, err_t err)
{
	net_start_DNS(4);
	MQTTLink_Init();
}


//此部分用于循环发送设备数据使用
static err_t  MQTT_Link_sent(void *arg, struct tcp_pcb *pcb, uint16_t len)
{
	return ERR_OK;	
}


//链接周期函数
static err_t  MQTT_Link_poll(void *arg, struct tcp_pcb *pcb)
{
	//处理发布数据未成功
	if((MQTTLink.linkStatus == MQTT_LINK_PUBLISH_RUNDATA) 
	|| (MQTTLink.linkStatus == MQTT_LINK_PUBLISH_FAULTDATA) 
	|| (MQTTLink.linkStatus == MQTT_LINK_PUBLISH_HISTORDATA))
	{
		if((gGp.SysTickPoint - MQTTLink.publishtime) >= 60*1000) //60s未收到回复
		{
			//处理
			MQTTLink.publishResult = MQTT_SEND_FAULT;
			
			//断开
			MQTT_Sent_Data(MQTT_DISCONNECT);
		}
	}
	else
	{
		//历史故障实时上传
		if((gGp.Log_TotSOE_Num > gGp.Log_MQTTReadSOE_Num) && (MQTTLink.aes128key[0] != 0) && (MQTTLink.PCState == MQTT_PC_ONLINE))//存储了历史SOE记录
		{
			MQTTLink.linkStatus = MQTT_LINK_PUBLISH_FAULTDATA;
			MQTTLink.DUP = 0;
			MQTT_Sent_Data(MQTT_PUBLISH);
		}
		else if(((gGp.SysTickPoint - MQTTLink.publishtime) >= MQTTLink.sentcycle*1000) && (MQTTLink.aes128key[0] != 0) && (MQTTLink.PCState == MQTT_PC_ONLINE)) //其余数据定时上传
		{
			if(gGp.Log_TotDev_Num > gGp.Log_MQTTReadDev_Num)
			{
				MQTTLink.linkStatus = MQTT_LINK_PUBLISH_HISTORDATA;
				MQTTLink.DUP = 0;
				MQTT_Sent_Data(MQTT_PUBLISH);
			}
			else
			{
				MQTTLink.linkStatus = MQTT_LINK_PUBLISH_RUNDATA;
				MQTTLink.DUP = 0;
				MQTT_Sent_Data(MQTT_PUBLISH);
			}
		}	
		else 
		{
			if((gGp.SysTickPoint - MQTTLink.pingtime) >= 60000)  //每分钟发送1个ping包
			{
				MQTTLink.pingnum++;
				if(MQTTLink.pingnum <= 5) //5分钟内保持链接
				{
					MQTT_Sent_Data(MQTT_PINGREQ);
				}
				else
				{
					//断开链接
					MQTT_Sent_Data(MQTT_DISCONNECT);
				}
			}
		}

// 		if(gGp.Transmode == GPRS_SEND_ALSO) //Also要求数据5分钟发送一次
// 		{
// 			if(((gGp.SysTickPoint - MQTTLink.publishtime) >= MQTTLink.sentcycle*1000) && (MQTTLink.aes128key[0] != 0) && (MQTTLink.PCState == MQTT_PC_ONLINE))
// 			{
// 				if(gGp.Log_TotDev_Num > gGp.Log_MQTTReadDev_Num)
// 				{
// 					MQTTLink.linkStatus = MQTT_LINK_PUBLISH_HISTORDATA;
// 					MQTTLink.DUP = 0;
// 					MQTT_Sent_Data(MQTT_PUBLISH);
// 				}
// 				else if(gGp.Log_TotSOE_Num > gGp.Log_MQTTReadSOE_Num)
// 				{
// 					MQTTLink.linkStatus = MQTT_LINK_PUBLISH_FAULTDATA;
// 					MQTTLink.DUP = 0;
// 					MQTT_Sent_Data(MQTT_PUBLISH);
// 				}
// 				else
// 				{
// 					MQTTLink.linkStatus = MQTT_LINK_PUBLISH_RUNDATA;
// 					MQTTLink.DUP = 0;
// 					MQTT_Sent_Data(MQTT_PUBLISH);
// 				}
// 			}	
// 			else 
// 			{
// 				if((gGp.SysTickPoint - MQTTLink.pingtime) >= 60000)  //每分钟发送1个ping包
// 				{
// 					MQTTLink.pingnum++;
// 					if(MQTTLink.pingnum <= 5) //5分钟内保持链接
// 					{
// 						MQTT_Sent_Data(MQTT_PINGREQ);
// 					}
// 					else
// 					{
// 						//断开链接
// 						MQTT_Sent_Data(MQTT_DISCONNECT);
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
// 			if((gGp.Log_TotDev_Num > gGp.Log_MQTTReadDev_Num) && (MQTTLink.aes128key[0] != 0)&& (MQTTLink.PCState == MQTT_PC_ONLINE))
// 			{
// 				MQTTLink.linkStatus = MQTT_LINK_PUBLISH_HISTORDATA;
// 				MQTTLink.DUP = 0;
// 				MQTT_Sent_Data(MQTT_PUBLISH);
// 			}
// 			else if((gGp.Log_TotSOE_Num > gGp.Log_MQTTReadSOE_Num) && (MQTTLink.aes128key[0] != 0) && (MQTTLink.PCState == MQTT_PC_ONLINE))//存储了历史SOE记录
// 			{
// 				MQTTLink.linkStatus = MQTT_LINK_PUBLISH_FAULTDATA;
// 				MQTTLink.DUP = 0;
// 				MQTT_Sent_Data(MQTT_PUBLISH);
// 			}
// 			else if(((gGp.SysTickPoint - MQTTLink.publishtime) >= MQTTLink.sentcycle*1000) && (MQTTLink.aes128key[0] != 0)&& (MQTTLink.PCState == MQTT_PC_ONLINE))  //定时发送
// 			{
// 				MQTTLink.linkStatus = MQTT_LINK_PUBLISH_RUNDATA;
// 				MQTTLink.DUP = 0;
// 				MQTT_Sent_Data(MQTT_PUBLISH);
// 			}
// 			else
// 			{
// 				if((gGp.SysTickPoint - MQTTLink.pingtime) >= 60000)
// 				{
// 					MQTTLink.pingnum++;
// 					if(MQTTLink.pingnum <= 5) //5分钟内保持链接
// 					{
// 						MQTT_Sent_Data(MQTT_PINGREQ);
// 					}
// 					else
// 					{
// 						//断开链接
// 						MQTT_Sent_Data(MQTT_DISCONNECT);
// 					}
// 				}
// 			}
// 		}
	}
	return ERR_OK;
}



//此部分用于接收MQTT数据，并启动数据发送
static err_t MQTT_Link_receive(void *arg, struct tcp_pcb *pcb, struct pbuf *p_rcvdPkt, err_t err)
{
	if ((err != ERR_OK) || (p_rcvdPkt == NULL))
    {
        if (p_rcvdPkt != NULL)
        {            
            tcp_recved(pcb, p_rcvdPkt->tot_len);    /* Inform TCP that we have taken the data. */
            pbuf_free(p_rcvdPkt);
        }
        MQTT_Link_Close();
        return ERR_OK;
    }

    struct pbuf *p_rcvdBuf = p_rcvdPkt;
	struct pbuf *P_prercvBuf = NULL;
	
	uint16_t len = 0;
	while(p_rcvdBuf != NULL)
	{
		if(len < MQTT_RCV_BUF_LEN)
		{
			memcpy(&MQTTLink.rcvBuf[len], (uint8_t *)p_rcvdBuf->payload, p_rcvdBuf->len);
		}
		
		P_prercvBuf = p_rcvdBuf;
		len += p_rcvdBuf->len;
		p_rcvdBuf = p_rcvdBuf->next;
		pbuf_free(P_prercvBuf);
		
		MQTTLink.rcvlen = len;
	}
	tcp_recved(pcb, p_rcvdPkt->tot_len);
	
	if(gGp.UDP_MQTT_PAK == 0xAA)
	{
		udp_send_PPC(MQTTLink.rcvBuf, MQTTLink.rcvlen);
	}

	if(MQTTLink.rcvlen < MQTT_RCV_BUF_LEN)
	{
		MQTT_Receive_Data(MQTTLink.rcvBuf, MQTTLink.rcvlen); //接收数据处理
	}
	return ERR_OK;
}



//链接成功的回调函数
static err_t MQTT_Link_Callback(void *arg, struct tcp_pcb *pcb, err_t err)
{
	LWIP_UNUSED_ARG(arg);

    if (ERR_OK==err)
    {
		MQTTLink.linkStatus = MQTT_LINK_OK;
		tcp_sent(pcb, MQTT_Link_sent);
		tcp_poll(pcb, MQTT_Link_poll, 2); //平均1s 调用一次
	    tcp_recv(pcb, MQTT_Link_receive);
		
		MQTT_Sent_Data(MQTT_CONNECT);
    }

    else
    {
        MQTT_Link_Close();
    }
    
    return err;
}


//MQTT启动链接
void MQTT_Link_Start(void)
{
	if((MQTTLink.linkStatus == MQTT_LINK_NULL) || (MQTTLink.linkHandle == NULL))
	{
		struct tcp_pcb *MQTTPCB = tcp_new();
    
		if(MQTTPCB !=NULL)
		{
			struct ip_addr ipa;

			err_t eret;

			IP4_ADDR(&ipa, MQTTLink.linkIP[0],MQTTLink.linkIP[1],MQTTLink.linkIP[2],MQTTLink.linkIP[3]);
			
			eret=tcp_bind(MQTTPCB, IP_ADDR_ANY, 0);
			
			if (ERR_USE==eret)
			{
				tcp_close(MQTTPCB);
			}
			else
			{
				MQTTLink.linkHandle = MQTTPCB;
				tcp_arg(MQTTPCB, MQTTLink.linkHandle);
				eret=tcp_connect(MQTTPCB, &ipa, MQTTLink.LinkPort, MQTT_Link_Callback);
			}
		  
			if (ERR_OK==eret)
			{
				tcp_err(MQTTPCB, MQTT_Link_Error);
			}
			else
			{
				tcp_close(MQTTPCB);
				MQTTLink_Init();
			}
		}
	}
}


//MQTT初始化
void MQTT_Init(void)
{
	MQTTLink_Init();
	
	sprintf(MQTT_SubscribeTopic1,"SN%s/Time",gGp.devSN);
	sprintf(MQTT_SubscribeTopic2,"SN%s/SetPara",gGp.devSN);
	sprintf(MQTT_SubscribeTopic3,"%s","PC/Password");
	sprintf(MQTT_SubscribeTopic4,"%s","PC/Offline");
	sprintf(MQTT_SubscribeTopic5,"SN%s/PowerInfo",gGp.devSN);
	sprintf(MQTT_ClientID,"%s",gGp.devSN);
	sprintf(MQTT_WillMessge,"SN:%s",gGp.devSN);
}
