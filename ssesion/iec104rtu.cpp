#include "includes.h"

void IEC104_linkClose(S_IEC104Body_T *ss)
{
    if (ss != NULL)
    {
		for(uint8_t i=0;i<TCP_IEC104_LINK_NUM;i++)
		{
			if(g_iec104_ss[i]==ss)
			{
				close(ss->sock);
				g_iec104_ss[i]=NULL;
				free(ss);
				YX_Data_Send_Left=REMOTE_SIGNAL_END_ADDR-REMOTE_SIGNAL_BEGIN_ADDR+1;
				YC_Data_Send_Left=REMOTE_MESURE_END_ADDR-REMOTE_MESURE_BEGIN_ADDR+1;
			}
		}
    }
}
void IEC104_linkSent(void)
{
	for(uint8_t i=0;i<TCP_IEC104_LINK_NUM;i++){
		if(g_iec104_ss[i] != NULL){
			if(g_iec104_ss[i]->callbackflag == true){
				g_iec104_ss[i]->callbackflag = false;
				IEC104_ASDU_Response(g_iec104_ss[i]);
			}
		}
	}
}
void IEC104_linkSendData(uint8_t *buf, uint8_t len, S_IEC104Body_T *ss)
{
    send(ss->sock, buf, len, 0);
}
void IO_Process_Data_From_PC_By_NET_IEC104(uint8_t *buf, uint8_t len, S_IEC104Body_T *ss)//处理IEC104协议I帧、S帧、U帧
{
    if(buf[APDU_HEAD_IDX] != IEC_104_HEAD ||
       buf[APDU_LEN_IDX]  != len - IEC_104_HEAD_LENGTH ||
       buf[APDU_LEN_IDX]  <  IEC_104_MIN_LENGTH || // 不是有效的APDU
       buf[APDU_LEN_IDX]  >  IEC_104_RCV_BUF_LEN) // 接收的APDU包长度大于接收缓冲区长度，丢弃
    {
        return;
    }

    if(ss == NULL) return;
    ss->rcvLen = buf[1];  // APDU包长度
    memcpy(ss->rcvBuf, buf + 2, ss->rcvLen);
    ss->t3 = System_Info.IEC104_T3;  // t3复位
    
    switch(ss->rcvBuf[0] & IEC_104_APDU_TYPE_MASK) // APDU类型判断
    {
        case 0:
        case 2://I帧
            if(ss->active != 0)
            {
                IEC104_APDU_CountRefresh(IEC_104_RECV, IEC_104_FORMAT_I,ss);
                IEC104_APDU_I_Proc(ss);
                // 收到I帧，停止t1超时计数以及k计数清零
                ss->sentUI = 0;
                ss->k = 0;
            }
        break;
        case 1://S帧
            if(ss->active != 0)
            {
                IEC104_APDU_CountRefresh(IEC_104_RECV, IEC_104_FORMAT_S,ss);
				IEC104_APDU_S_Proc();
                // 收到S帧，停止t1超时计数以及k计数清零
                ss->sentUI = 0;
                ss->k = 0;
            }
        break;
        case 3:
            IEC104_APDU_U_Proc(ss);
        break;
        default:
        break;
    }
    IEC104_ASDU_Response(ss);
}
void IEC104Body_Reset(S_IEC104Body_T *ss)
{
    ss->active = 0;
    ss->rcvLen = 0;
    ss->sendNum = 0;
    ss->rcvNum = 0;
    ss->ackNum = 0;
    ss->cmdQHead = 0;
    ss->cmdQTail = 0;
    ss->sentUI = 0;
    ss->k = 0;
    ss->t1 = System_Info.IEC104_T1;
    ss->t3 = System_Info.IEC104_T3;
    ss->Start_Call_All = false;
	YX_Data_Send_Left=REMOTE_SIGNAL_END_ADDR-REMOTE_SIGNAL_BEGIN_ADDR+1;
	YC_Data_Send_Left=REMOTE_MESURE_END_ADDR-REMOTE_MESURE_BEGIN_ADDR+1;
}
uint8_t IEC104_ASDU_CallAll(S_IEC104Body_T *ss)// 总召唤
{
    if(ss->rcvBuf[ASDU_QOI_IDX] != 20 || ss->rcvBuf[ASDU_VARLEN_IDX] != 1)
    {
        return UNKNOWN_ASDU_TYPE;
    }

    // 传送原因有效性判断
    if(getCombinedData(&ss->rcvBuf[ASDU_REASON_IDX], ASDU_REASON_LEN) != 6)
    {
        return UNKNOWN_ASDU_REASON;
    }

    // 信息地址有效性判断
    if(getCombinedData(&ss->rcvBuf[ASDU_FIRST_MSGADDR_IDX], ASDU_MSGADDR_LEN) != 0)
    {
        return UNKNOWN_ASDU_MSGADDR;
    }

    // 确定为总召唤命令，填充响应命令队列
    ss->cmdQueue[0].cmdType = C_IC_NA_1;
    ss->cmdQueue[0].reason = 7;
    ss->cmdQueue[1].cmdType = M_SP_NA_1;
    ss->cmdQueue[1].reason = 20;
    ss->cmdQueue[2].cmdType = M_ME_NC_1;
    ss->cmdQueue[2].reason = 20;
// 	ss->cmdQueue[3].cmdType = M_IT_NA_1;
// 	ss->cmdQueue[3].reason = 20;
    ss->cmdQueue[3].cmdType = C_IC_NA_1;
    ss->cmdQueue[3].reason = 10;
    ss->cmdQHead = 0;
    ss->cmdQTail = IEC_104_CMD_QUEUE_LEN-1;

	ss->Start_Call_All = true;

	YX_Data_Send_Left=REMOTE_SIGNAL_END_ADDR-REMOTE_SIGNAL_BEGIN_ADDR+1;
	YC_Data_Send_Left=REMOTE_MESURE_END_ADDR-REMOTE_MESURE_BEGIN_ADDR+1;
    return ASDU_FORMAT_OK;
}
uint8_t IEC104_ASDU_RemotePulse(S_IEC104Body_T *ss)// 遥脉 电度量 目前仅支持读 不支持冻结和复位
{
    if((ss->rcvBuf[ASDU_VARLEN_IDX] != 1) || 
        ((ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX] & 0x3F) != 5) || // RQT = 5 电度量请求
        (((ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX] >> 6) & 0x03) != 0))  // FRZ = 0 读电度量
    {
        return UNKNOWN_ASDU_TYPE;
    }

    // 传送原因有效性判断
    if(getCombinedData(&ss->rcvBuf[ASDU_REASON_IDX], ASDU_REASON_LEN) != 6)
    {
        return UNKNOWN_ASDU_REASON;
    }

    // 信息地址有效性判断
    if(getCombinedData(&ss->rcvBuf[ASDU_FIRST_MSGADDR_IDX], ASDU_MSGADDR_LEN) != 0)
    {
        return UNKNOWN_ASDU_MSGADDR;
    }

    // 确定为总召唤命令，填充响应命令队列
    ss->cmdQueue[0].cmdType = C_CI_NA_1;
    ss->cmdQueue[0].reason = 7;
    ss->cmdQueue[1].cmdType = M_IT_NA_1;
    ss->cmdQueue[1].reason = 37;
    ss->cmdQueue[2].cmdType = C_CI_NA_1;
    ss->cmdQueue[2].reason = 10;
    ss->cmdQHead = 0;
    ss->cmdQTail = 3;

    return ASDU_FORMAT_OK;
}
uint8_t IEC104_ASDU_RemoteControl(S_IEC104Body_T *ss)// 遥控
{
    // 可变结构限定词有效性判断
    if(ss->rcvBuf[ASDU_VARLEN_IDX] != 1)
    {
        return UNKNOWN_ASDU_TYPE;
    }

    // 传送原因有效性判断
    uint16_t reason = getCombinedData(&ss->rcvBuf[ASDU_REASON_IDX], ASDU_REASON_LEN);
    if(reason != 6 && reason != 8)
    {
        return UNKNOWN_ASDU_REASON;
    }

    // 信息地址有效性判断
    uint32_t msgaddr = getCombinedData(&ss->rcvBuf[ASDU_FIRST_MSGADDR_IDX], ASDU_MSGADDR_LEN);
    if(msgaddr < REMOTE_CONTROL_BEGIN_ADDR || msgaddr > REMOTE_CONTROL_END_ADDR)
    {
        return UNKNOWN_ASDU_MSGADDR;
    }

    // 遥控命令合法
    if(reason == 6)
    {
        if((ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX] & 0x80) != 0)//分两步，先选择再设置
        {
            // 选择命令
            ss->cmdQueue[0].cmdType = C_SC_NA_1;
            ss->cmdQueue[0].reason = 7;
            ss->cmdQHead = 0;
            ss->cmdQTail = 1;
        }
        else
        {
            // 执行命令
            uint16_t rlt = doRemoteControl(System_Info.IEC104_Address, msgaddr, (ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX] & 0x1));
            // 返回执行结果
            ss->cmdQueue[0].cmdType = C_SC_NA_1;
            ss->cmdQueue[0].reason = rlt;
            ss->cmdQueue[1].cmdType = C_SC_NA_1;
            ss->cmdQueue[1].reason = 10;
            ss->cmdQHead = 0;
            ss->cmdQTail = 2;
        }
    }
    else// if(reason == 8)  // 撤销命令
    {
        // 选择命令
        ss->cmdQueue[0].cmdType = C_SC_NA_1;
        ss->cmdQueue[0].reason = 9;
        ss->cmdQueue[1].cmdType = C_SC_NA_1;
        ss->cmdQueue[1].reason = 10;
        ss->cmdQHead = 0;
        ss->cmdQTail = 2;
    }
    return ASDU_FORMAT_OK;
}
uint8_t IEC104_ASDU_RemoteAdjust(uint8_t cmdType, S_IEC104Body_T *ss)// 遥调
{
	uint32_t msgAddr[50];//多点信息体地址
	uint32_t data[50];//多点信息
	uint32_t msgNum=0;//多点信息个数

    if((cmdType != C_SE_NC_1) && (cmdType != C_SE_NB_1) && (cmdType != C_SE_ND_1)) // 可变结构限定词有效性判断
    {
        return UNKNOWN_ASDU_TYPE;
    }	

	if((ss->rcvBuf[ASDU_VARLEN_IDX] < 1))
	{
		return UNKNOWN_ASDU_TYPE;
	}
	else
	{
		msgNum=ss->rcvBuf[ASDU_VARLEN_IDX];
	}
	 
    // 传送原因有效性判断
    if(getCombinedData(&ss->rcvBuf[ASDU_REASON_IDX], ASDU_REASON_LEN) != 6)
    {
        return UNKNOWN_ASDU_REASON;
    }

    uint8_t dataLen = 2;
    if(cmdType == C_SE_NC_1)
    {
        dataLen = 4;
    }

    //uint32_t data = getCombinedData(&ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX], dataLen);

	for(uint8_t i=0;i<msgNum;i++)
	{
		msgAddr[i] = getCombinedData(&ss->rcvBuf[ASDU_FIRST_MSGADDR_IDX+i*5], ASDU_MSGADDR_LEN);
		if(!((msgAddr[i] >= REMOTE_ADJUST_BEGIN_ADDR) && (msgAddr[i] <= REMOTE_ADJUST_END_ADDR)) &&
		   !((msgAddr[i] >= REMOTE_ADJUST1_BEGIN_ADDR) && (msgAddr[i] <= REMOTE_ADJUST1_END_ADDR)))
		{
			return UNKNOWN_ASDU_MSGADDR;
		}
		data[i] = getCombinedData(&ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX+i*5], dataLen);
	}

	if((ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX+(msgNum-1)*5+dataLen] & 0x80) != 0)
	{
		//选择命令
		ss->cmdQueue[0].cmdType = cmdType;
		ss->cmdQueue[0].reason = 7;
		ss->cmdQueue[0].msgNum = msgNum;
		ss->cmdQHead = 0;
		ss->cmdQTail = 1;
	}
	else
	{
		// 遥调命令合法，执行
		uint16_t rlt=0;
		for(uint8_t i=0;i<msgNum;i++)
		{
			rlt = doRemoteAdjust(System_Info.IEC104_Address, msgAddr[i], data[i]);			
		}
		// 返回执行结果
		ss->cmdQueue[0].cmdType = cmdType;
		ss->cmdQueue[0].reason = rlt;
		ss->cmdQueue[0].msgNum = msgNum;
		//ss->cmdQueue[1].cmdType = cmdType;
		//ss->cmdQueue[1].reason = 10;
		//ss->cmdQueue[1].msgNum = msgNum;
		ss->cmdQHead = 0;
		ss->cmdQTail = 1;//2;
	}
    return ASDU_FORMAT_OK;
}
uint8_t IEC104_ASDU_TimeSet(S_IEC104Body_T *ss)//对时
{
    // 可变结构限定词有效性判断
    if(ss->rcvBuf[ASDU_VARLEN_IDX] != 1)
    {
        return UNKNOWN_ASDU_TYPE;
    }

    // 传送原因有效性判断
    if(getCombinedData(&ss->rcvBuf[ASDU_REASON_IDX], ASDU_REASON_LEN) != 6)
    {
        return UNKNOWN_ASDU_REASON;
    }

    // 信息地址有效性判断
    if(getCombinedData(&ss->rcvBuf[ASDU_FIRST_MSGADDR_IDX], ASDU_MSGADDR_LEN) != 0)
    {
        return UNKNOWN_ASDU_MSGADDR;
    }

    // 命令合法，执行
    uint16_t rlt = doSetTime(System_Info.IEC104_Address, &ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX]);

    ss->cmdQueue[0].cmdType = C_CS_NA_1;
    ss->cmdQueue[0].reason = rlt;
    ss->cmdQHead = 0;
    ss->cmdQTail = 1;
    return ASDU_FORMAT_OK;
}
void IEC104_ASDU_UnknownASDU(uint8_t unknownType,S_IEC104Body_T *ss)// 未知命令
{
    ss->cmdQueue[0].cmdType = ss->rcvBuf[ASDU_TYPE_IDX];
    ss->cmdQueue[0].reason = unknownType;
    ss->cmdQHead = 0;
    ss->cmdQTail = 1;
}
uint32_t getCombinedData(uint8_t *buf, uint8_t len)// 将数组buf中的各字节拼接成一个32位无符号数，低字节低位 高字节高位
{
    uint8_t i;
    uint32_t cdata = 0;
    uint8_t *p = (uint8_t *)&cdata;
    if(buf == NULL || len == 0 || len > 4)
    {
        return 0;
    }

    for(i = 0; i < len; i++)
    {
        p[i] = buf[i];
    }
    return cdata;
}
void getSplitData(uint8_t *buf, uint8_t len, uint32_t data)// 将一个32位无符号数分解成数组buf中的各字节，低字节低位 高字节高位// len为指定的分解后的个数
{
    uint8_t i;
    uint32_t cdata = data;
    uint8_t *p = (uint8_t *)&cdata;
    if(buf == NULL || len == 0 || len > 4)
    {
        return;
    }

    for(i = 0; i < len; i++)
    {
        buf[i] = p[i];
    }
}
void IEC104_APDU_I_Proc(S_IEC104Body_T *ss)
{
    uint16_t rlt = ASDU_FORMAT_OK;
    if(getCombinedData(&ss->rcvBuf[ASDU_COMADDR_IDX], ASDU_COMADDR_LEN)==System_Info.IEC104_Address)
    {
        switch(ss->rcvBuf[ASDU_TYPE_IDX])
        {
            case C_SC_NA_1:// 单点遥控
            	mutex_for_timer.lock();
                rlt = IEC104_ASDU_RemoteControl(ss);
                mutex_for_timer.unlock();
            break;
            case C_SE_NC_1:// 浮点遥调
                rlt = IEC104_ASDU_RemoteAdjust(C_SE_NC_1,ss);
            break;
            case C_SE_NB_1:// 标度化值遥调
                rlt = IEC104_ASDU_RemoteAdjust(C_SE_NB_1,ss);
            break;
			case C_SE_ND_1://多点遥调  add by sundl 2015.5.13
				 rlt = IEC104_ASDU_RemoteAdjust(C_SE_ND_1,ss);
				break;
            case C_IC_NA_1:// 总召唤
                rlt = IEC104_ASDU_CallAll(ss);
            break;
            case C_CS_NA_1:// 时间同步
            	mutex_for_timer.lock();
                rlt = IEC104_ASDU_TimeSet(ss);
                mutex_for_timer.unlock();
            break;
            case C_CI_NA_1:// 电能累计量
                rlt = IEC104_ASDU_RemotePulse(ss);
            break;
            default:// 不支持的ASDU类型
                rlt = UNKNOWN_ASDU_TYPE;
            break;
        }
    }
    else
    {
        rlt = UNKNOWN_ASDU_COMADDR;
    }

    if(rlt != ASDU_FORMAT_OK)
    {
        IEC104_ASDU_UnknownASDU(rlt,ss);
    }
}
uint32_t getRemoteSignal(uint32_t beginAddr, uint8_t *buf, uint8_t num, S_IEC104Body_T *ss)
 {
// 获取单点遥信数据，数据按协议地址顺序从小到大排列，每个数据占一个字节，每个地址占3个字节
// buf中数据格式：addr(3字节) + data(1字节)
// 地址范围：REMOTE_SIGNAL_BEGIN_ADDR ~ REMOTE_SIGNAL_END_ADDR
// 返回读取的数据个数
    if(num == 0 || buf == NULL)
    {
        return 0;
    }
    uint8_t i;
    for(i = 0; i < num; i++)
    {
        buf[(i<<2)]     = (beginAddr + i)         & UINT8_MAX;
        buf[(i<<2) + 1] = ((beginAddr + i) >> 8)  & UINT8_MAX;
        buf[(i<<2) + 2] = ((beginAddr + i) >> 16) & UINT8_MAX;
        buf[(i<<2) + 3] = doGetRemoteSignal(System_Info.IEC104_Address, beginAddr + i);
    }
    return i;
}
uint32_t getRemoteMesure(uint32_t beginAddr, uint8_t *buf, uint8_t num, S_IEC104Body_T *ss)
{
// 获取浮点遥测数据，数据按协议地址顺序从小到大排列，每个数据占4个字节，每个地址占3个字节
// 地址范围：REMOTE_MESURE_BEGIN_ADDR ~ REMOTE_MESURE_END_ADDR
// 返回读取的数据个数
    if(beginAddr < REMOTE_MESURE_BEGIN_ADDR || num == 0 || buf == NULL)
    {
        return 0;
    }
    uint8_t i;
    uint32_t data = 0;
    getSplitData(buf, ASDU_MSGADDR_LEN, beginAddr);
    for(i = 0; i < num; i++)
    {
        if(beginAddr + i > REMOTE_MESURE_END_ADDR)
        {
            break;
        }
        uint8_t idx = ASDU_MSGADDR_LEN + i*5;
        data = doGetRemoteMesure(System_Info.IEC104_Address, beginAddr + i);
		
		if(data != YC_data[beginAddr - REMOTE_MESURE_BEGIN_ADDR + i])
		{
			YC_data[beginAddr - REMOTE_MESURE_BEGIN_ADDR + i]=data;
		}
		
        buf[idx]     =  data        & UINT8_MAX;
        buf[idx + 1] = (data >> 8)  & UINT8_MAX;
        buf[idx + 2] = (data >> 16) & UINT8_MAX;
        buf[idx + 3] = (data >> 24) & UINT8_MAX;
		buf[idx + 4] = 0;
	}
    return i;
}
uint32_t getRemoteMesure1(uint32_t beginAddr, uint8_t *buf, uint8_t num, S_IEC104Body_T *ss)
{
// 获取规一化遥测数据，数据按协议地址顺序从小到大排列，每个数据占4个字节，每个地址占3个字节
// 地址范围：REMOTE_MESURE_BEGIN_ADDR ~ REMOTE_MESURE_END_ADDR
// 返回读取的数据个数
    if(beginAddr < REMOTE_MESURE1_BEGIN_ADDR || num == 0 || buf == NULL)
    {
        return 0;
    }
    uint8_t i;
    uint32_t data = 0;
    for(i = 0; i < num; i++)
    {
        if(beginAddr + i > REMOTE_MESURE1_END_ADDR)
        {
            break;
        }
        uint8_t idx = i*6;
        buf[idx]     = (beginAddr + i)         & UINT8_MAX;
        buf[idx + 1] = ((beginAddr + i) >> 8)  & UINT8_MAX;
        buf[idx + 2] = ((beginAddr + i) >> 16) & UINT8_MAX;
        data = doGetRemoteMesure(System_Info.IEC104_Address, beginAddr + i);
		
		if(data != YC_data[beginAddr - REMOTE_MESURE_BEGIN_ADDR + i])
		{
			YC_data[beginAddr - REMOTE_MESURE_BEGIN_ADDR + i]=data;
		}
		
        buf[idx + 3] = data        & UINT8_MAX;
        buf[idx + 4] = (data >> 8) & UINT8_MAX;
        buf[idx + 5] = 0;
    }
    return i;
}
uint32_t getRemoteMesure2(uint32_t beginAddr, uint8_t *buf, uint8_t num, S_IEC104Body_T *ss)
{
// 获取规一化遥测数据，数据按协议地址顺序从小到大排列，每个数据占4个字节，每个地址占3个字节
// 地址范围：REMOTE_MESURE_BEGIN_ADDR ~ REMOTE_MESURE_END_ADDR
// 返回读取的数据个数
	//static uint32_t YC_data[REMOTE_MESURE_END_ADDR-REMOTE_MESURE_BEGIN_ADDR+1]={0};
	
    if(beginAddr < REMOTE_MESURE_BEGIN_ADDR || num == 0 || buf == NULL)
    {
        return 0;
    }
    uint8_t i,j=0;
    uint32_t data = 0;
    for(i = 0; i < num; i++)
    {
        if(beginAddr + i > REMOTE_MESURE_END_ADDR)
        {
            break;
        }
        uint8_t idx = j*8;
        buf[idx]     = (beginAddr + i)         & UINT8_MAX;
        buf[idx + 1] = ((beginAddr + i) >> 8)  & UINT8_MAX;
        buf[idx + 2] = ((beginAddr + i) >> 16) & UINT8_MAX;
        data = doGetRemoteMesure(System_Info.IEC104_Address, beginAddr + i);

		if(data == YC_data[beginAddr - REMOTE_MESURE_BEGIN_ADDR + i])
		{
			continue;
		}
		else
		{
			YC_data[beginAddr - REMOTE_MESURE_BEGIN_ADDR + i]=data;
		}
		
        buf[idx + 3] =  data        & UINT8_MAX;
        buf[idx + 4] = (data >> 8)  & UINT8_MAX;
        buf[idx + 5] = (data >> 16) & UINT8_MAX;
        buf[idx + 6] = (data >> 24) & UINT8_MAX;
		buf[idx + 7] = 0;
		
		j++;
    }
    return j;
}
uint32_t getRemotePulse(uint32_t beginAddr, uint8_t *buf, uint8_t num, S_IEC104Body_T *ss)
{
    if(beginAddr < REMOTE_PLUSE_BEGIN_ADDR || num == 0 || buf == NULL)
    {
        return 0;
    }
    uint8_t i;
    uint32_t data = 0;
    for(i = 0; i < num; i++)
    {
        if(beginAddr + i > REMOTE_PLUSE_END_ADDR)
        {
            break;
        }
        buf[(i<<3)]     = (beginAddr + i)         & UINT8_MAX;
        buf[(i<<3) + 1] = ((beginAddr + i) >> 8)  & UINT8_MAX;
        buf[(i<<3) + 2] = ((beginAddr + i) >> 16) & UINT8_MAX;
        data = doGetRemotePulse(System_Info.IEC104_Address, beginAddr + i);
        buf[(i<<3) + 3] =  data        & UINT8_MAX;
        buf[(i<<3) + 4] = (data >> 8)  & UINT8_MAX;
        buf[(i<<3) + 5] = (data >> 16) & UINT8_MAX;
        buf[(i<<3) + 6] = (data >> 24) & UINT8_MAX;
        buf[(i<<3) + 7] = i + 1;
    }
    return i;
}
void IEC104_UploadRemoteSignal(uint16_t devAddr, ChangedSignal *cs, uint8_t num, S_IEC104Body_T *ss)
{
// 遥信量变位上传// devAddr：发生信号变位的设备地址// num：变位信号个数
    uint8_t i;
    
    if((ss == NULL) || (ss->active == 0) || (cs == NULL) || (num == 0))
    {
        return;
    }

    ss->sendBuf[APDU_HEAD_IDX] = IEC_104_HEAD;

    uint8_t leftNum = num;
    while(leftNum > 0)   // 未分包，最大一次传输22个数据s
    {
        uint8_t sendNum = 0;
		
		// 最多未回复I帧发送计数达到最大则停止发送数据
		if(ss->k >= System_Info.IEC104_K)
		{
			return;
		}
		
        if(leftNum > MAX_SIGUPLOAD_PER_PKG)
        {
            // 分包
            sendNum = MAX_SIGUPLOAD_PER_PKG;
        }
        else
        {
            sendNum = leftNum;
        }
        uint8_t datalen = sendNum * 11;
        // 填充数据包长度
        ss->sendBuf[APDU_LEN_IDX] = APDU_FIRST_MSGADDR_IDX + datalen - IEC_104_HEAD_LENGTH;
        // 填充发送接收计数字段
        ss->sendBuf[APDU_TXID_IDX] = ss->sendNum & UINT8_MAX;
        ss->sendBuf[APDU_TXID_IDX + 1] = (ss->sendNum & 0xFF00) >> 8;
        ss->sendBuf[APDU_RXID_IDX] = ss->rcvNum & UINT8_MAX;
        ss->sendBuf[APDU_RXID_IDX + 1] = (ss->rcvNum  & 0xFF00) >> 8;
        // 填充类别
        ss->sendBuf[APDU_UTYPE_IDX] = M_SP_TB_1;
        // 填充可变结构限定词
        ss->sendBuf[APDU_VARLEN_IDX] = sendNum;
        // 填充发送原因
        getSplitData(&ss->sendBuf[APDU_REASON_IDX], ASDU_REASON_LEN, 3);
        // 填充公共地址
        getSplitData(&ss->sendBuf[APDU_COMADDR_IDX], ASDU_COMADDR_LEN, devAddr);
        // 填充变位数据
        for(i = 0; i < sendNum; i++)
        {
            getSplitData(&ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i*11], ASDU_MSGADDR_LEN, cs[num - leftNum + i].addr);
            ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i*11 + ASDU_MSGADDR_LEN] = cs[num - leftNum + i].data;
            ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i*11 + ASDU_MSGADDR_LEN + 1] = cs[num - leftNum + i].time[0];
            ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i*11 + ASDU_MSGADDR_LEN + 2] = cs[num - leftNum + i].time[1];
            ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i*11 + ASDU_MSGADDR_LEN + 3] = cs[num - leftNum + i].time[2];
            ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i*11 + ASDU_MSGADDR_LEN + 4] = cs[num - leftNum + i].time[3];
            ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i*11 + ASDU_MSGADDR_LEN + 5] = cs[num - leftNum + i].time[4];
            ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i*11 + ASDU_MSGADDR_LEN + 6] = cs[num - leftNum + i].time[5];
            ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i*11 + ASDU_MSGADDR_LEN + 7] = cs[num - leftNum + i].time[6];
        }
        // 执行发送
        IEC104_ASDU_SendData(ss->sendBuf, APDU_FIRST_MSGADDR_IDX + datalen,ss);
        //OSTimeDly(100);
    
        // 刷新发送接收计数
        IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);

        leftNum -= sendNum;
    }
}
uint32_t dataUpload(uint32_t startAddr, uint32_t endAddr, uint8_t groupLen, uint8_t cmdType, S_IEC104Body_T *ss)
{
// 返回发送的数据包数// startAddr：待发送数据的首地址
    uint8_t  varLen = 0;  // 可变结构限定词
    uint8_t  sendLen = 0; // 发送APDU数据包长度
    uint32_t totalNum = endAddr - startAddr + 1;// 待发送数据的个数

    while(totalNum > 0)
    {
        uint8_t sendNum = 0;
		
        if(ss->k >= System_Info.IEC104_K)
        {
            return totalNum;
        }
		
        if(totalNum > groupLen)
        {
            // 分包
            sendNum = groupLen;
            totalNum -= groupLen;
        }
        else
        {
            sendNum = totalNum;
            totalNum = 0;
        }
        // 填充发送接收计数字段
        ss->sendBuf[APDU_TXID_IDX] = ss->sendNum & UINT8_MAX;
        ss->sendBuf[APDU_TXID_IDX + 1] = (ss->sendNum & 0xFF00) >> 8;
        ss->sendBuf[APDU_RXID_IDX] = ss->rcvNum  & UINT8_MAX;
        ss->sendBuf[APDU_RXID_IDX + 1] = (ss->rcvNum  & 0xFF00) >> 8;
    
        if(cmdType == M_SP_NA_1)        // 遥信
        {
            varLen = getRemoteSignal(startAddr,
                                     &ss->sendBuf[APDU_FIRST_MSGADDR_IDX],
                                     sendNum,ss);
            sendLen = APDU_FIRST_MSGADDR_IDX + varLen * (ASDU_MSGADDR_LEN + 1);
        }
        else if(cmdType == M_ME_NC_1)   // 浮点遥测
        {
            varLen = getRemoteMesure(startAddr,
                                     &ss->sendBuf[APDU_FIRST_MSGADDR_IDX],
                                     sendNum,ss);
            sendLen = APDU_FIRST_MSGADDR_IDX + ASDU_MSGADDR_LEN + varLen * 5;
            varLen |= 0x80;   // linf 20120616 组包发送

        }
        else if(cmdType == M_ME_NB_1)   // 标度化遥测
        {
            varLen = getRemoteMesure1(startAddr,
                                     &ss->sendBuf[APDU_FIRST_MSGADDR_IDX],
                                     sendNum,ss);
            sendLen = APDU_FIRST_MSGADDR_IDX + varLen * (ASDU_MSGADDR_LEN + 3);
        }
        else
        {
            return 0;
        }
        // 填充数据包长度
        ss->sendBuf[APDU_LEN_IDX] = sendLen - IEC_104_HEAD_LENGTH;
        // 填充可变结构限定词
        ss->sendBuf[APDU_VARLEN_IDX] = varLen;
    
        // 执行发送
        IEC104_ASDU_SendData(ss->sendBuf, sendLen,ss);
        IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);

        startAddr += sendNum;
    }
	
	return 0;
}

uint32_t dataUploadAuto(uint32_t startAddr, uint32_t endAddr, uint8_t groupLen, uint8_t cmdType, S_IEC104Body_T *ss)
{
// 返回发送的数据包数// startAddr：待发送数据的首地址
    uint8_t  varLen = 0;  // 可变结构限定词
    uint8_t  sendLen = 0; // 发送APDU数据包长度
    uint32_t  totalNum = endAddr - startAddr + 1;// 待发送数据的个数

    while(totalNum > 0)
    {
        uint8_t sendNum = 0;
        if(totalNum > groupLen)
        {
            // 分包
            sendNum = groupLen;
            totalNum -= groupLen;
        }
        else
        {
            sendNum = totalNum;
            totalNum = 0;
        }
        // 填充发送接收计数字段
        ss->sendBuf[APDU_TXID_IDX] = ss->sendNum & UINT8_MAX;
        ss->sendBuf[APDU_TXID_IDX + 1] = (ss->sendNum & 0xFF00) >> 8;
        ss->sendBuf[APDU_RXID_IDX] = ss->rcvNum  & UINT8_MAX;
        ss->sendBuf[APDU_RXID_IDX + 1] = (ss->rcvNum  & 0xFF00) >> 8;
    
		if(cmdType == M_ME_NC_1)	//标准化变化遥测
		{
            varLen = getRemoteMesure2(startAddr,
                                     &ss->sendBuf[APDU_FIRST_MSGADDR_IDX],
                                     sendNum,ss);
            sendLen = APDU_FIRST_MSGADDR_IDX + varLen * (ASDU_MSGADDR_LEN + 5);
		}
        else
        {
            return 0;
        }
		
		if(sendLen>APDU_FIRST_MSGADDR_IDX)
		{
        	// 填充数据包长度
        	ss->sendBuf[APDU_LEN_IDX] = sendLen - IEC_104_HEAD_LENGTH;
        	// 填充可变结构限定词
        	ss->sendBuf[APDU_VARLEN_IDX] = varLen;
    
        	// 执行发送
        	IEC104_ASDU_SendData(ss->sendBuf, sendLen,ss);
        	IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);
			return totalNum;
		}

        startAddr += sendNum;
    }
	return 0;
}

void IEC104_ASDU_Response(S_IEC104Body_T *ss)// 响应处理，所有RTU的ASDU响应帧均通过该函数发出，该函数处理cmdQueue队列中的待发送数据帧
{
    if(ss->cmdQHead == ss->cmdQTail)
    {
        return;
    }
    uint8_t  idx = ss->cmdQHead;
    uint8_t  varLen = 0;  // 可变结构限定词
    uint8_t  sendLen = 0; // 发送APDU数据包长度
    uint8_t  i,j;
    uint32_t startAddr = 0;     // 待发送数据的首地址
    uint8_t  totalNum = 0;       // 待发送数据的个数

    ss->sendBuf[APDU_HEAD_IDX] = IEC_104_HEAD;

    while(idx < ss->cmdQTail)
    {
        // 最多未回复I帧发送计数，达到最大则停止发送数据
        if(ss->k >= System_Info.IEC104_K)
        {
            return;
        }

        // 填充发送接收计数字段
        ss->sendBuf[APDU_TXID_IDX] = ss->sendNum & UINT8_MAX;
        ss->sendBuf[APDU_TXID_IDX + 1] = (ss->sendNum & 0xFF00) >> 8;
        ss->sendBuf[APDU_RXID_IDX] = ss->rcvNum  & UINT8_MAX;
        ss->sendBuf[APDU_RXID_IDX + 1] = (ss->rcvNum  & 0xFF00) >> 8;

        // 填充类别
        ss->sendBuf[APDU_UTYPE_IDX] = ss->cmdQueue[idx].cmdType;
        // 填充发送原因
        getSplitData(&ss->sendBuf[APDU_REASON_IDX], ASDU_REASON_LEN, ss->cmdQueue[idx].reason);
        // 填充公共地址
        getSplitData(&ss->sendBuf[APDU_COMADDR_IDX], ASDU_COMADDR_LEN, System_Info.IEC104_Address);
        ss->sendBuf[APDU_COMADDR_IDX + 1] = HIBYTE(System_Info.IEC104_Address);

        if(ss->cmdQueue[idx].cmdType == M_SP_NA_1) 			// 单点遥信
        {
            YX_Data_Send_Left=dataUpload(REMOTE_SIGNAL_END_ADDR+1-YX_Data_Send_Left, REMOTE_SIGNAL_END_ADDR, MAX_SIGNAL_PER_PKG, M_SP_NA_1,ss);
			if(YX_Data_Send_Left!=0)
			{
				return;
			}
			else
			{
				ss->callbackflag = true;
				ss->cmdQHead++;
				YX_Data_Send_Left=REMOTE_SIGNAL_END_ADDR-REMOTE_SIGNAL_BEGIN_ADDR+1;
			}
			return;
        }
        else if(ss->cmdQueue[idx].cmdType == M_ME_NC_1)   	// 浮点遥测
        {
        	YC_Data_Send_Left=dataUpload(REMOTE_MESURE_END_ADDR+1-YC_Data_Send_Left, REMOTE_MESURE_END_ADDR, MAX_MESURE_PER_PKG, M_ME_NC_1,ss);
            if(YC_Data_Send_Left!=0)
            {
            	return;
            }
			else
			{
				YC_Data_Send_Left=REMOTE_MESURE_END_ADDR-REMOTE_MESURE_BEGIN_ADDR+1;
			}
        }
        else if(ss->cmdQueue[idx].cmdType == M_ME_NB_1)   	// 标度化值遥测
        {
            dataUpload(REMOTE_MESURE1_BEGIN_ADDR, REMOTE_MESURE1_END_ADDR, MAX_MESURE1_PRE_PKG, M_ME_NB_1,ss);
        }
        else if(ss->cmdQueue[idx].cmdType == C_SC_NA_1) 	// 单点遥控
        {
            // 填充信息地址
			for(i = 0; i < ASDU_MSGADDR_LEN; i++)
			{
				ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i] = ss->rcvBuf[ASDU_FIRST_MSGADDR_IDX + i];
			}
			// 填充信息元素
            ss->sendBuf[APDU_FIRST_MSGADDR_IDX + ASDU_MSGADDR_LEN] = ss->rcvBuf[ASDU_FIRST_MSGADDR_IDX + ASDU_MSGADDR_LEN];
            varLen = 1;
            sendLen = APDU_FIRST_MSGADDR_IDX + ASDU_MSGADDR_LEN + 1;
            // 填充数据包长度
            ss->sendBuf[APDU_LEN_IDX] = sendLen - IEC_104_HEAD_LENGTH;
            // 填充可变结构限定词
            ss->sendBuf[APDU_VARLEN_IDX] = varLen;

            // 执行发送
            IEC104_ASDU_SendData(ss->sendBuf, sendLen,ss);
            IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);
        }
        else if(ss->cmdQueue[idx].cmdType == C_SE_NC_1) // 浮点遥调
        {
            // 填充信息地址
                for(i = 0; i < ASDU_MSGADDR_LEN; i++)
                {
                    ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i] = ss->rcvBuf[ASDU_FIRST_MSGADDR_IDX + i];
                }
            // 填充信息元素
            for(i = 0; i < 5; i++)
            {
                ss->sendBuf[APDU_FIRST_MSGDATA_IDX + i] = ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX + i];
            }
            varLen = 1;
            sendLen = APDU_FIRST_MSGDATA_IDX + 5;
            // 填充数据包长度
            ss->sendBuf[APDU_LEN_IDX] = sendLen - IEC_104_HEAD_LENGTH;
            // 填充可变结构限定词
            ss->sendBuf[APDU_VARLEN_IDX] = varLen;

            // 执行发送
            IEC104_ASDU_SendData(ss->sendBuf, sendLen,ss);
            IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);
        }
        else if(ss->cmdQueue[idx].cmdType == C_SE_NB_1) // 标度化值遥调
        {
            // 填充信息地址
            for(i = 0; i < ASDU_MSGADDR_LEN; i++)
            {
                ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i] = ss->rcvBuf[ASDU_FIRST_MSGADDR_IDX + i];
            }
            // 填充信息元素
            for(i = 0; i < 3; i++)
            {
                ss->sendBuf[APDU_FIRST_MSGDATA_IDX + i] = ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX + i];
            }
            varLen = 1;
            sendLen = APDU_FIRST_MSGDATA_IDX + 3;
            // 填充数据包长度
            ss->sendBuf[APDU_LEN_IDX] = sendLen - IEC_104_HEAD_LENGTH;
            // 填充可变结构限定词
            ss->sendBuf[APDU_VARLEN_IDX] = varLen;

            // 执行发送
            IEC104_ASDU_SendData(ss->sendBuf, sendLen,ss);
            IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);
        }
        else if(ss->cmdQueue[idx].cmdType == C_IC_NA_1)   // 总召唤
        {
            // 填充信息地址
            getSplitData(&ss->sendBuf[APDU_FIRST_MSGADDR_IDX], ASDU_MSGADDR_LEN, 0);
            // 填充信息元素
            ss->sendBuf[APDU_FIRST_MSGDATA_IDX] = 20;
            varLen = 1;
            sendLen = APDU_FIRST_MSGDATA_IDX + 1;
            // 填充数据包长度
            ss->sendBuf[APDU_LEN_IDX] = sendLen - IEC_104_HEAD_LENGTH;
            // 填充可变结构限定词
            ss->sendBuf[APDU_VARLEN_IDX] = varLen;

            // 执行发送
            IEC104_ASDU_SendData(ss->sendBuf, sendLen,ss);
            IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);
        }
        else if(ss->cmdQueue[idx].cmdType == C_CS_NA_1) // 时间同步
        {
            // 填充信息地址
            getSplitData(&ss->sendBuf[APDU_FIRST_MSGADDR_IDX], ASDU_MSGADDR_LEN, 0);
            // 填充信息元素
            for(i = 0; i < 7; i++)
            {
                ss->sendBuf[APDU_FIRST_MSGDATA_IDX + i] = ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX + i];
            }
            varLen = 1;
            sendLen = APDU_FIRST_MSGDATA_IDX + 7;
            // 填充数据包长度
            ss->sendBuf[APDU_LEN_IDX] = sendLen - IEC_104_HEAD_LENGTH;
            // 填充可变结构限定词
            ss->sendBuf[APDU_VARLEN_IDX] = varLen;

            // 执行发送
            IEC104_ASDU_SendData(ss->sendBuf, sendLen,ss);
            IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);
        }
        else if(ss->cmdQueue[idx].cmdType == C_CI_NA_1) // 电能累计召唤
        {
            // 填充信息地址
            getSplitData(&ss->sendBuf[APDU_FIRST_MSGADDR_IDX], ASDU_MSGADDR_LEN, 0);
            // 填充信息元素
            ss->sendBuf[APDU_FIRST_MSGDATA_IDX] = ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX];
            sendLen = APDU_FIRST_MSGDATA_IDX + 1;
            // 填充数据包长度
            ss->sendBuf[APDU_LEN_IDX] = sendLen - IEC_104_HEAD_LENGTH;
            // 填充可变结构限定词
            ss->sendBuf[APDU_VARLEN_IDX] = 1;

            // 执行发送
            IEC104_ASDU_SendData(ss->sendBuf, sendLen,ss);
            IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);
        }
        else if(ss->cmdQueue[idx].cmdType == M_IT_NA_1) // 电能累计量上传
        {
            startAddr = REMOTE_PLUSE_BEGIN_ADDR;
            totalNum = REMOTE_PLUSE_END_ADDR - REMOTE_PLUSE_BEGIN_ADDR + 1;
            
            varLen = getRemotePulse(startAddr, &ss->sendBuf[APDU_FIRST_MSGADDR_IDX], totalNum,ss);
            sendLen = APDU_FIRST_MSGADDR_IDX + varLen * (ASDU_MSGADDR_LEN + 5);
            // 填充数据包长度
            ss->sendBuf[APDU_LEN_IDX] = sendLen - IEC_104_HEAD_LENGTH;
            // 填充可变结构限定词
            ss->sendBuf[APDU_VARLEN_IDX] = varLen;

            // 执行发送
            IEC104_ASDU_SendData(ss->sendBuf, sendLen,ss);
            IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);
        }
        else if(ss->cmdQueue[idx].cmdType == C_SE_ND_1)
        {
            // 填充信息地址
            for(j=0; j<ss->cmdQueue[idx].msgNum; j++)
            {
	            for(i = 0; i < ASDU_MSGADDR_LEN; i++)
	            {
	                ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i + 5*j] = ss->rcvBuf[ASDU_FIRST_MSGADDR_IDX + i + 5*j];
	            }
	            // 填充信息元素
	            for(i = 0; i < 2; i++)
	            {
	                ss->sendBuf[APDU_FIRST_MSGDATA_IDX + i + 5*j] = ss->rcvBuf[ASDU_FIRST_MSGDATA_IDX + i + 5*j];
	            }
            }
			
            sendLen = APDU_FIRST_MSGADDR_IDX + 5*ss->cmdQueue[idx].msgNum + 1;
			
			ss->sendBuf[sendLen] = 0;//QL   add by Lihd.
			
            // 填充数据包长度
            ss->sendBuf[APDU_LEN_IDX] = sendLen - IEC_104_HEAD_LENGTH;
            // 填充可变结构限定词
            ss->sendBuf[APDU_VARLEN_IDX] = ss->cmdQueue[idx].msgNum;

            // 执行发送
            IEC104_ASDU_SendData(ss->sendBuf, sendLen,ss);
            IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);
        }
        else if(IS_UNKNOW_ASDU(ss->cmdQueue[idx].reason))
        {
            sendLen = ss->rcvLen + IEC_104_HEAD_LENGTH;
            for(i = 0; i < sendLen - APDU_FIRST_MSGADDR_IDX; i++)
            {
                ss->sendBuf[APDU_FIRST_MSGADDR_IDX + i] = ss->rcvBuf[ASDU_FIRST_MSGADDR_IDX + i];
            }
            // 填充可变结构限定词
            ss->sendBuf[APDU_VARLEN_IDX] = ss->rcvBuf[ASDU_VARLEN_IDX];
            // 填充数据包长度
            ss->sendBuf[APDU_LEN_IDX] = ss->rcvLen;

            // 执行发送
            IEC104_ASDU_SendData(ss->sendBuf, sendLen,ss);
            IEC104_APDU_CountRefresh(IEC_104_SEND, IEC_104_FORMAT_I,ss);
        }
        idx++;
        ss->cmdQHead++;
    }
    ss->cmdQHead = 0;
    ss->cmdQTail = 0;
}
void IEC104_UploadRemoteMesure(S_IEC104Body_T *ss)// 遥测值上传
{
    static uint32_t startAddr = REMOTE_MESURE_BEGIN_ADDR;
	static uint32_t totalNum = REMOTE_MESURE_END_ADDR - REMOTE_MESURE_BEGIN_ADDR + 1;
	if((ss == NULL) || (ss->active == 0))
    {
        return;
    }

	// 防止有ASDU在发送
	if(ss->cmdQHead != ss->cmdQTail)
    {
        return;
    }

	// 最多未回复I帧发送计数达到最大则停止发送数据
    if(ss->k >= System_Info.IEC104_K)
    {
        return;
    }

	uint8_t sendNum = 0;
	if(totalNum > MAX_MESURE_PER_PKG)
    {
        // 分包
        totalNum -= MAX_MESURE_PER_PKG;
		sendNum = MAX_MESURE_PER_PKG;
    }
    else
    {
        sendNum = totalNum;
        totalNum = REMOTE_MESURE_END_ADDR - REMOTE_MESURE_BEGIN_ADDR + 1;
    }
    
	// 填充发送接收计数字段
    ss->sendBuf[APDU_TXID_IDX] = ss->sendNum & UINT8_MAX;
    ss->sendBuf[APDU_TXID_IDX + 1] = (ss->sendNum & 0xFF00) >> 8;
    ss->sendBuf[APDU_RXID_IDX] = ss->rcvNum  & UINT8_MAX;
    ss->sendBuf[APDU_RXID_IDX + 1] = (ss->rcvNum  & 0xFF00) >> 8;

    // 填充类别
    ss->sendBuf[APDU_UTYPE_IDX] = M_ME_NC_1;
    // 填充发送原因
    getSplitData(&ss->sendBuf[APDU_REASON_IDX], ASDU_REASON_LEN, 3);
    // 填充公共地址
    getSplitData(&ss->sendBuf[APDU_COMADDR_IDX], ASDU_COMADDR_LEN, System_Info.IEC104_Address);

	// 上传一包数据
	dataUpload(startAddr, startAddr + sendNum - 1, sendNum, M_ME_NC_1,ss);
	
	// 地址刷新
	if(startAddr + sendNum <= REMOTE_MESURE_END_ADDR)
	{
		startAddr += sendNum;
	}
	else
	{
		startAddr = REMOTE_MESURE_BEGIN_ADDR;
	}

}

void IEC104_UploadRemoteMesureChanged(S_IEC104Body_T *ss)//变化遥测值上传
{
	static uint32_t Change_YC_Left = REMOTE_MESURE_END_ADDR-REMOTE_MESURE_BEGIN_ADDR+1;
	
	if((ss == NULL) || (ss->active == 0))
    {
        return;
    }

	// 防止有ASDU在发送
	if(ss->cmdQHead != ss->cmdQTail)
    {
        return;
    }

	// 最多未回复I帧发送计数达到最大则停止发送数据
    if(ss->k >= System_Info.IEC104_K)
    {
        return;
    }
    
	// 填充发送接收计数字段
    ss->sendBuf[APDU_TXID_IDX] = ss->sendNum & UINT8_MAX;
    ss->sendBuf[APDU_TXID_IDX + 1] = (ss->sendNum & 0xFF00) >> 8;
    ss->sendBuf[APDU_RXID_IDX] = ss->rcvNum  & UINT8_MAX;
    ss->sendBuf[APDU_RXID_IDX + 1] = (ss->rcvNum  & 0xFF00) >> 8;

    // 填充类别
    ss->sendBuf[APDU_UTYPE_IDX] = M_ME_NC_1;
    // 填充发送原因
    getSplitData(&ss->sendBuf[APDU_REASON_IDX], ASDU_REASON_LEN, 3);
    // 填充公共地址
    getSplitData(&ss->sendBuf[APDU_COMADDR_IDX], ASDU_COMADDR_LEN, System_Info.IEC104_Address);

	// 上传一包数据
	Change_YC_Left=dataUploadAuto(REMOTE_MESURE_END_ADDR+1-Change_YC_Left, REMOTE_MESURE_END_ADDR, MAX_MESURE2_PRE_PKG, M_ME_NC_1,ss);
	if(Change_YC_Left==0)
	{
		Change_YC_Left=REMOTE_MESURE_END_ADDR-REMOTE_MESURE_BEGIN_ADDR+1;
	}
}

void IEC104_APDU_TimeOutProc(void)// 超时处理函数
{
	for(uint8_t i=0;i<TCP_IEC104_LINK_NUM;i++)
	{
		if(g_iec104_ss[i]!=NULL)
		{
			if(g_iec104_ss[i]->t3 != 0)
			{
				g_iec104_ss[i]->t3--;
			}
			else
			{
				// t3时间内未收到任何报文，超时，发送TESTFR
				uint8_t sendBuf[IEC_104_HEAD_LENGTH + IEC_104_MIN_LENGTH] = {IEC_104_HEAD, 0x04, 0x43, 0x00, 0x00, 0x00};
				g_iec104_ss[i]->t3 = System_Info.IEC104_T3;
				IEC104_linkSendData(sendBuf, sizeof(sendBuf),g_iec104_ss[i]);
				// 启动I帧超时计数
				g_iec104_ss[i]->sentUI = 1;
				g_iec104_ss[i]->t1 = System_Info.IEC104_T1;
			}

			if(g_iec104_ss[i]->sentUI == 1)
			{
				if(g_iec104_ss[i]->t1 != 0)
				{
					g_iec104_ss[i]->t1--;
				}
				else
				{
					IEC104_linkClose(g_iec104_ss[i]); // 发送一个U或I帧后t1时间内未收到确认，超时断开TCP
				}
			}
		}
	}
}
void IEC104_ASDU_SendData(uint8_t *buf, uint8_t len, S_IEC104Body_T *ss)// ASDU发送函数
{
    ss->sentUI = 1;// 启动I帧超时计数
    ss->t1 = System_Info.IEC104_T1;
    ss->k++;// I帧发送计数
    IEC104_linkSendData(buf, len, ss);
}
void IEC104_APDU_U_Proc(S_IEC104Body_T *ss)
{
    uint8_t index = 0;
    ss->sendBuf[APDU_HEAD_IDX] = IEC_104_HEAD;
    if(ss->rcvBuf[APCI_UTYPE_IDX] & IEC_104_STARTDT)
    {
        index = 6;
        ss->sendBuf[1] = 0x04;
        ss->sendBuf[2] = 0x0b;
        ss->sendBuf[3] = 0x00;
        ss->sendBuf[4] = 0x00;
        ss->sendBuf[5] = 0x00;
        ss->active = 1;
    }
    else if(ss->rcvBuf[APCI_UTYPE_IDX] & IEC_104_STOPDT)
    {
        index = 6;
        ss->sendBuf[1] = 0x04;
        ss->sendBuf[2] = 0x23;
        ss->sendBuf[3] = 0x00;
        ss->sendBuf[4] = 0x00;
        ss->sendBuf[5] = 0x00;
        ss->active = 0;
    }
    else if(ss->rcvBuf[APCI_UTYPE_IDX] & IEC_104_TESTFR)
    {
        index = 6;
        ss->sendBuf[1] = 0x04;
        ss->sendBuf[2] = 0x83;
        ss->sendBuf[3] = 0x00;
        ss->sendBuf[4] = 0x00;
        ss->sendBuf[5] = 0x00;
    }
    else if(ss->rcvBuf[APCI_UTYPE_IDX] & IEC_104_TESTFR_CON)
    {
        ss->sentUI = 0;// 收到S帧，停止t1超时计数
    }
    
    if(index > 0)
    {
        IEC104_linkSendData(ss->sendBuf, index, ss);
    }
}
void IEC104_APDU_S_Proc(void)// 不处理，计数由IEC104_APDU_CountRefresh刷新
{
}
void IEC104_APDU_CountRefresh(uint8_t tranType, uint8_t apduType, S_IEC104Body_T *ss)// 发送接收计数刷新// IEC104Body IEC104协议主结构体
{
    if(tranType == IEC_104_RECV)
    {
        // 接收到的发送方接收序号
        uint16_t rcvNum = ss->rcvBuf[APCI_RCVNUM_IDX] + (ss->rcvBuf[APCI_RCVNUM_IDX + 1] << 8);

        if(apduType == IEC_104_FORMAT_I)
        {
            // 接收到的发送方发送序号
            uint16_t sendNum = ss->rcvBuf[APCI_SENDNUM_IDX] + (ss->rcvBuf[APCI_SENDNUM_IDX + 1] << 8);
            if(sendNum != ss->rcvNum || rcvNum != ss->sendNum)   // RTU接收到的数据可能有丢失// RTU发送的数据可能有丢失
            {
//                 IEC104_linkClose(ss);//关闭连接  ccg 2014.5.10  关闭从站对序号错误的处理，由主站主动断开链接
            }
            else
            {
                ss->rcvNum += 2;
                ss->ackNum = rcvNum;
            }
        }
        else if(apduType == IEC_104_FORMAT_S)
        {
            if(rcvNum != ss->sendNum) // RTU发送的数据可能有丢失
            {
// 				IEC104_linkClose(ss);//关闭连接
            }
            else
            {
                ss->ackNum = rcvNum;
            }
        }
    }
    else if(tranType == IEC_104_SEND && apduType == IEC_104_FORMAT_I)// IEC_104_SEND
    {
        ss->sendNum += 2;
    }
}
