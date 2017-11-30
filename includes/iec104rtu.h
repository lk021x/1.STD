#ifndef _IEC104RTU_H_
#define _IEC104RTU_H_

// 超时时间 必须满足t3 > t1
#define  TIMEOUT_T1          15
#define  TIMEOUT_T3          20

// 发送MAX_K个数据都未得到确认报文后停止后续发送
#define  MAX_K               12

// ASDU帧格式自定义 -------------------
#define  ASDU_REASON_LEN     2
#define  ASDU_MSGADDR_LEN    3

#define  ASDU_COMADDR_LEN    2
#define  ASDU_COMADDR_MASK   0xFFFF // 若ASDU_COMADDR_LEN为1，该宏定义值需为0xFF
// ------------------------------------

// 信号变位信息结构，变位上传用
typedef struct
{
    uint32_t addr;    // 信号地址
    uint8_t  data;    // 变位数据
    uint8_t  time[7]; // time格式：毫秒低字节 毫秒高字节 分 时 日 月 年 
} ChangedSignal;


// ASDU响应发送命令队列长度
#define  IEC_104_CMD_QUEUE_LEN   5


#define  IEC_104_RCV_BUF_LEN     256    // 接收缓冲区长度，作为RTU，可根据最大命令长度设小些
#define  IEC_104_SEND_BUF_LEN    256    // 发送缓冲区长度


#define  IEC104_PORT             2404   // 端口号
#define  IEC_104_HEAD_LENGTH     2      // 包头+包长度字段占字节数
#define  IEC_104_MIN_LENGTH      4      // APDU最小长度，同时也是控制字段长度


#define  IEC_104_HEAD            0x68
#define  IEC_104_STARTDT         0x04
#define  IEC_104_STOPDT          0x10
#define  IEC_104_TESTFR          0x40
#define  IEC_104_TESTFR_CON      0x80

// APDU帧中头部数据偏移下标
#define  APDU_HEAD_IDX           0
#define  APDU_LEN_IDX            1
#define  APDU_TXID_IDX           2
#define  APDU_RXID_IDX           4
#define  APDU_UTYPE_IDX          6
#define  APDU_VARLEN_IDX         7
#define  APDU_REASON_IDX         8
#define  APDU_COMADDR_IDX        (APDU_REASON_IDX + ASDU_REASON_LEN)    // 10
#define  APDU_FIRST_MSGADDR_IDX  (APDU_COMADDR_IDX + ASDU_COMADDR_LEN)  // 12
#define  APDU_FIRST_MSGDATA_IDX  (APDU_FIRST_MSGADDR_IDX + ASDU_MSGADDR_LEN)



// APCI帧中各数据偏移下标，不含头
#define  APCI_UTYPE_IDX          0
#define  APCI_SENDNUM_IDX        0
#define  APCI_RCVNUM_IDX         2

// ASDU帧中各数据偏移下标，不含头
#define  ASDU_TYPE_IDX           4
#define  ASDU_VARLEN_IDX         5
#define  ASDU_REASON_IDX         6
#define  ASDU_COMADDR_IDX        (ASDU_REASON_IDX + ASDU_REASON_LEN)         // 8
#define  ASDU_FIRST_MSGADDR_IDX  (ASDU_COMADDR_IDX + ASDU_COMADDR_LEN)       // 10
#define  ASDU_FIRST_MSGDATA_IDX  (ASDU_FIRST_MSGADDR_IDX + ASDU_MSGADDR_LEN) // 13
#define  ASDU_QOI_IDX            13


#define  IEC_104_APDU_TYPE_MASK  0x03

// APDU类型
#define  IEC_104_FORMAT_I         0
#define  IEC_104_FORMAT_S         1
#define  IEC_104_FORMAT_U         3

// 发送接收动作标志
#define  IEC_104_SEND            0
#define  IEC_104_RECV            1

#define  IEC_104_ERR             (-1)
#define  IEC_104_OK              0


// 未知ASDU类型
#define  ASDU_FORMAT_OK          0
#define  UNKNOWN_ASDU_TYPE       44
#define  UNKNOWN_ASDU_REASON     45
#define  UNKNOWN_ASDU_COMADDR    46
#define  UNKNOWN_ASDU_MSGADDR    47
#define  IS_UNKNOW_ASDU(type)    ((type == UNKNOWN_ASDU_TYPE) || \
                                  (type == UNKNOWN_ASDU_REASON) || \
                                  (type == UNKNOWN_ASDU_COMADDR) || \
                                  (type == UNKNOWN_ASDU_MSGADDR))


// ASDU类别标志
#define  M_SP_NA_1               1          // 单点遥信
#define  M_BO_NA_1               7          // 远动终端状态
#define  M_ME_NB_1               11         // 遥测，标度化值
#define  M_ME_NC_1               13         // 浮点遥测
#define  M_IT_NA_1               15         // 电能累积量
#define  M_SP_TB_1               30         // 遥信变位
#define  C_SC_NA_1               45         // 单点遥控
#define  C_SE_NB_1               49         // 遥调，标度化值
#define  C_SE_NC_1               50         // 遥调，浮点值
#define  C_IC_NA_1               100        // 总召唤
#define  C_CI_NA_1               101        // 累计量总召唤
#define  C_CS_NA_1               103        // 时间同步
#define  C_SE_ND_1               136        //多点遥调



#define  MAX_SIGNAL_PER_PKG      60         // 每帧可以发送的最大遥信数据个数，不得大于60
#define  MAX_MESURE_PER_PKG      48         // 每帧可以发送的最大遥测数据个数，不得大于48
#define  MAX_MESURE1_PRE_PKG     40         // 每帧可以发送的最大规一化遥测数据个数，不得大于40
#define  MAX_MESURE2_PRE_PKG     30         // 每帧可以发送的最大规一化遥测数据个数，不得大于30
#define  MAX_SIGUPLOAD_PER_PKG   22         // 主动上传遥信时最大遥信数据个数，不得大于22


// 命令队列，组发送用
typedef struct
{
    uint8_t  cmdType;
    uint8_t  param;
    uint16_t reason;
    uint8_t msgNum;
} CmdQueue;


typedef struct
{
    int       sock;                      		// 通信socket句柄
    uint8_t   active;                           // 连接激活标志，接收到STARTDT后激活
    uint8_t   sentUI;                           // U或I帧发送完成标志，发送后置位
    uint8_t   k;                                // 发送k个I格式报文均未收到确认，停止发送
    uint8_t   rcvLen;                           // 接收缓冲区中有效数据长度
    uint8_t   rcvBuf[IEC_104_RCV_BUF_LEN];      // 接收缓冲区
    uint8_t   sendBuf[IEC_104_SEND_BUF_LEN];    // 发送缓冲区
    uint8_t   sendLen;                          // 发送缓冲区中待发送数据长度
    uint16_t  sendNum;                          // 发送序号
    uint16_t  rcvNum;                           // 接收序号
    uint16_t  ackNum;                           // 响应序号
    bool      callbackflag;
    CmdQueue  cmdQueue[IEC_104_CMD_QUEUE_LEN];  // 待发送命令队列
    uint8_t   cmdQHead;                         // 头指针
    uint8_t   cmdQTail;                         // 尾指针
    uint8_t   t1;                               // 发送或测试APDU的超时
    uint8_t   t3;                               // 长期空闲t3>t1状态下发送测试帧的超时
    bool 	  Start_Call_All;					// 总招标志
} S_IEC104Body_T;

void IEC104Body_Reset(S_IEC104Body_T *ss);
void IO_Process_Data_From_PC_By_NET_IEC104(uint8_t *buf, uint8_t len, S_IEC104Body_T *ss);
void IEC104_APDU_CountRefresh(uint8_t tranType, uint8_t apduType, S_IEC104Body_T *ss);
void IEC104_APDU_I_Proc(S_IEC104Body_T *ss);
void IEC104_APDU_U_Proc(S_IEC104Body_T *ss);
void IEC104_ASDU_Response(S_IEC104Body_T *ss);// 响应处理，所有RTU的ASDU响应帧均通过该函数发出，该函数处理cmdQueue队列中的待发送数据帧
uint8_t IEC104_ASDU_CallAll(S_IEC104Body_T *ss);// 总召唤
uint8_t IEC104_ASDU_RemotePulse(S_IEC104Body_T *ss);// 遥脉 电度量 目前仅支持读 不支持冻结和复位
uint8_t IEC104_ASDU_RemoteControl(S_IEC104Body_T *ss);// 遥控
uint8_t IEC104_ASDU_RemoteAdjust(uint8_t cmdType, S_IEC104Body_T *ss);// 遥调
uint8_t IEC104_ASDU_TimeSet(S_IEC104Body_T *ss);
void IEC104_ASDU_UnknownASDU(uint8_t unknownType, S_IEC104Body_T *ss);// 未知命令
uint32_t getCombinedData(uint8_t *buf, uint8_t len);// 将数组buf中的各字节拼接成一个32位无符号数，低字节低位 高字节高位
void getSplitData(uint8_t *buf, uint8_t len, uint32_t data);// 将一个32位无符号数分解成数组buf中的各字节，低字节低位 高字节高位// len为指定的分解后的个数
uint32_t getRemoteSignal(uint32_t beginAddr, uint8_t *buf, uint8_t num, S_IEC104Body_T *ss);
uint32_t getRemoteMesure(uint32_t beginAddr, uint8_t *buf, uint8_t num, S_IEC104Body_T *ss);
uint32_t getRemoteMesure1(uint32_t beginAddr, uint8_t *buf, uint8_t num, S_IEC104Body_T *ss);
uint32_t getRemotePulse(uint32_t beginAddr, uint8_t *buf, uint8_t num, S_IEC104Body_T *ss);
void IEC104_UploadRemoteSignal(uint16_t devAddr, ChangedSignal *cs, uint8_t num, S_IEC104Body_T *ss);// 遥信量变位上传// devAddr：发生信号变位的设备地址// num：变位信号个数
uint32_t dataUpload(uint32_t startAddr, uint32_t endAddr, uint8_t groupLen, uint8_t cmdType, S_IEC104Body_T *ss);// 返回发送的数据包数// startAddr：待发送数据的首地址
void IEC104_UploadRemoteMesure(S_IEC104Body_T *ss);// 遥测值上传
void IEC104_UploadRemoteMesureChanged(S_IEC104Body_T *ss);//变化遥测值上传
void IEC104_ASDU_SendData(uint8_t *buf, uint8_t len, S_IEC104Body_T *ss);// ASDU发送函数
void IEC104_linkSendData(uint8_t *buf, uint8_t len, S_IEC104Body_T *ss);
void IEC104_APDU_TimeOutProc(void);// 超时处理函数
void IEC104_APDU_S_Proc(void);// 不处理，计数由IEC104_APDU_CountRefresh刷新
void IEC104_linkSent(void);


#endif
