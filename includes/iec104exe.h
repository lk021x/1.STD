#ifndef _IEC104EXE_H_
#define _IEC104EXE_H_

// 遥信地址
#define REMOTE_SIGNAL_BEGIN_ADDR    0x0001
#define REMOTE_SIGNAL_END_ADDR      0x0096

// 遥测地址
#define REMOTE_MESURE_BEGIN_ADDR    0x4001
#define REMOTE_MESURE_END_ADDR      0x47C0

// 标度化值遥测地址
#define REMOTE_MESURE1_BEGIN_ADDR   0x4001
#define REMOTE_MESURE1_END_ADDR     0x47C0

// 遥控地址
#define REMOTE_CONTROL_BEGIN_ADDR   0x6001
#define REMOTE_CONTROL_END_ADDR     0x601C

// 浮点遥调地址
#define REMOTE_ADJUST_BEGIN_ADDR    0x6201
#define REMOTE_ADJUST_END_ADDR      0x6211

// 标度化值遥调地址
#define REMOTE_ADJUST1_BEGIN_ADDR   0x6301
#define REMOTE_ADJUST1_END_ADDR     0x6304

// 累计量
#define REMOTE_PLUSE_BEGIN_ADDR     0x6401
#define REMOTE_PLUSE_END_ADDR       0x6411

#define EXE_SUCCESS                 7
#define EXE_FAULT                   0x40   // P/N = 1

#define COMADDR_OK                  0
#define COMADDR_INVALID             (-1)



uint16_t doRemoteControl(uint16_t devAddr,uint32_t addr, uint8_t action);
uint16_t doRemoteAdjust(uint16_t devAddr,uint32_t addr, uint32_t value);
uint16_t doSetTime(uint16_t devAddr,const uint8_t *buf);
uint8_t  doGetRemoteSignal(uint16_t devAddr,uint32_t addr);
uint32_t doGetRemoteMesure(uint16_t devAddr,uint32_t addr);
uint32_t doGetRemotePulse(uint16_t devAddr, uint32_t addr);
void     doChangeSignalUpload(void);
void doChangeMeasureUpload(void);                                               //ò￡2a±???é?′?

#endif
