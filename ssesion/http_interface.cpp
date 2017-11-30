#include "http_def.h"
#include "utils.h"
#include "http_server.h"
#include "iconv.h"
#include <locale.h>
#include <string>

//-------------------------------------------------------------------------------
//webserver
LogTime R_time;

char p_000[6*1024*1024];    				//曲线图数据
char a_000[80];      						//ajax确认回复
char s_000[4096];     						//SetValue();
char jsCharts_temp[4096];                   //用于数据缓存

uint8_t Cur_PCS_Num=1;                      //当前PCS的编号: 从1开始
uint8_t Cur_PV_Num=1;                       //当前PV的编号: 从1开始
uint8_t Cur_PVS_Num=1;                      //当前PVS的编号: 从1开始
uint8_t Cur_DCDC_Num=1;                     //当前DCDC的编号: 从1开始
uint8_t Cur_BMS_Num=1;                      //当前BMS的编号: 从1开始
uint8_t Cur_AMMETER_Num=1;                  //当前电表的编号: 从1开始

bool Serial_Param_Change[6]={false,false,false,false,false,false};
bool PCS_Svr_Param_Change[PCS_NUM]={false};
bool PV_Svr_Param_Change[PV_NUM]={false};

uint16_t Param_flag[100];       			//保存要保存的参数值和保存标识
int32_t Param_value[100];      			//保存要保存的参数值和保存标识
uint8_t Webserver_Language=0xAA;            //0x55:中文 0xAA：English
uint8_t ExportST = 0;						//数据导出状态 0 - NO, 1 - YES
gt_web_page_show webPageShow;

uint8_t save_flag_int=0;                    //异步判断是否保存参数值的标识(数值型)

struct h_resource h_Rsc[]={//html资源数组
    {h_000,sizeof(h_000)},
    {h_001,sizeof(h_001)},
    {h_002,sizeof(h_002)},
    {h_003,sizeof(h_003)},
    {h_004,sizeof(h_004)},
    {h_005,sizeof(h_005)},
    {h_006,sizeof(h_006)},
    {h_007,sizeof(h_007)},
    {h_008,sizeof(h_008)},
    {h_009,sizeof(h_009)},
    {h_010,sizeof(h_010)},
    {h_011,sizeof(h_011)},
    {h_012,sizeof(h_012)},
    {h_013,sizeof(h_013)},
    {h_014,sizeof(h_014)},
    {h_015,sizeof(h_015)},
    {h_016,sizeof(h_016)},
    {h_017,sizeof(h_017)},
    {h_018,sizeof(h_018)},
    {h_019,sizeof(h_019)},
    {h_020,sizeof(h_020)},
    {h_021,sizeof(h_021)},
    {h_022,sizeof(h_022)},
    {h_023,sizeof(h_023)},
    {h_024,sizeof(h_024)},
    {h_025,sizeof(h_025)},
    {h_026,sizeof(h_026)},
    {h_027,sizeof(h_027)},
    {h_028,sizeof(h_028)},
    {h_029,sizeof(h_029)},
    {h_030,sizeof(h_030)},
    {h_031,sizeof(h_031)},
    {h_032,sizeof(h_032)},
    {h_033,sizeof(h_033)},
    {h_034,sizeof(h_034)},
    {h_035,sizeof(h_035)},
    {h_036,sizeof(h_036)},
    {h_037,sizeof(h_037)},
    {h_038,sizeof(h_038)},
    {h_039,sizeof(h_039)},
    {h_040,sizeof(h_040)},
    {h_041,sizeof(h_041)},
    {h_042,sizeof(h_042)},
    {h_043,sizeof(h_043)},
    {h_044,sizeof(h_044)},
    {h_045,sizeof(h_045)},
    {h_046,sizeof(h_046)},
    {h_047,sizeof(h_047)},
    {h_048,sizeof(h_048)},
    {h_049,sizeof(h_049)},
    {h_050,sizeof(h_050)},
    {h_051,sizeof(h_051)},
    {h_052,sizeof(h_052)},
    {h_053,sizeof(h_053)},
    {h_054,sizeof(h_054)},
    {h_055,sizeof(h_055)},
    {h_056,sizeof(h_056)}
};
struct j_resource j_Rsc[]={//javascript资源数组
    {j_000,sizeof(j_000)},
    {j_001,sizeof(j_001)},
    {j_002,sizeof(j_002)},
    {j_003,sizeof(j_003)},
    {j_004,sizeof(j_004)},
    {j_005,sizeof(j_005)},
    {j_006,sizeof(j_006)},
    {j_007,sizeof(j_007)},
    {j_008,sizeof(j_008)},
    {j_009,sizeof(j_009)},
    {j_010,sizeof(j_010)},
    {j_011,sizeof(j_011)},
    {j_012,sizeof(j_012)},
    {j_013,sizeof(j_013)},
    {j_014,sizeof(j_014)},
    {j_015,sizeof(j_015)}
};
struct c_resource c_Rsc[]={//css资源数组
    {c_000,sizeof(c_000)}
};
struct i_resource i_Rsc[]={//图片资源数组
    {i_000,sizeof(i_000)},
    {i_001,sizeof(i_001)},
    {i_002,sizeof(i_002)},
    {i_003,sizeof(i_003)},
    {i_004,sizeof(i_004)},
    {i_005,sizeof(i_005)},
    {i_006,sizeof(i_006)},
    {i_007,sizeof(i_007)},
    {i_008,sizeof(i_008)},
    {i_009,sizeof(i_009)},
    {i_010,sizeof(i_010)},
    {i_011,sizeof(i_011)},
    {i_012,sizeof(i_012)},
    {i_013,sizeof(i_013)},
    {i_014,sizeof(i_014)},
    {i_015,sizeof(i_015)},
    {i_016,sizeof(i_016)},
    {i_017,sizeof(i_017)},
    {i_018,sizeof(i_018)},
    {i_019,sizeof(i_019)},
    {i_020,sizeof(i_020)},
    {i_021,sizeof(i_021)},
    {i_022,sizeof(i_022)},
    {i_023,sizeof(i_023)},
    //新增 2017/5/12 lk
    {i_024,sizeof(i_024)}, //dialog_cb.png
    {i_025,sizeof(i_025)}, //dialog_closebtn.gif
    {i_026,sizeof(i_026)}, //dialog_closebtn_over.gif
    {i_027,sizeof(i_027)}, //dialog_ct.png
    {i_028,sizeof(i_028)}, //dialog_lb.png
    {i_029,sizeof(i_029)}, //dialog_lt.png
    {i_030,sizeof(i_030)}, //dialog_mlm.png
    {i_031,sizeof(i_031)}, //dialog_mrm.png
    {i_032,sizeof(i_032)}, //dialog_rb.png
    {i_033,sizeof(i_033)}, //dialog_rt.png
    {i_034,sizeof(i_034)}, //icon_alert.gif
    {i_035,sizeof(i_035)}, //icon_dialog.gif
    {i_036,sizeof(i_036)},  //icon_query.gif
    {i_037,sizeof(i_037)},  //故障
    {i_038,sizeof(i_038)}   //告警
};
struct s_resource s_Rsc[]={//初始化数据缓冲区
    {s_000,sizeof(s_000)}
};
struct a_resource a_Rsc[]={//ajax返回确认
    {a_000,sizeof(a_000)}
};
struct p_resource p_Rsc[]={//用于导出配置文件、历史记录、曲线图、柱状图、历史发电量
    {p_000,sizeof(p_000)}
};

const char* Event_Name[36][2]=
{
	{"",""},
	{"初始停机","Initial Stop"},
	{"微网供电启动","MG Starting"},
	{"电网供电启动","Grid Starting"},
	{"微网供电运行","MG Running"},
	{"电网供电运行","Grid Running"},
	{"故障","Fault"},
	{"停机中","Stopping"},
	{"停机","Stopped"},
	{"紧急停机","Emergency Stop"},   //9
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},						//16
	{"",""},
	{"启动中","Starting"},
	{"正在同步","Synchronizing"},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},						//32
	{"序号","No."},
	{"日期&时间","Date&Time"},
	{"事件名称","Event Name"}
};
const char* Fault_Name[68][2]=
{
	{"",""},
	{"储能变流器故障","PCS Fault"},
	{"储能变流器通讯故障","PCS Comm Fault"},
	{"储能变流器地址冲突","PCS Address Conflict"},
	{"",""},
	{"光伏逆变器故障","PV Fault"},
	{"光伏逆变器通讯故障","PV Comm Fault"},
	{"光伏逆变器地址冲突","PV Address Conflict"},
	{"BMS通讯故障","BMS Comm Fault"},
	{"电网侧接触器故障","Contact Fault"},
	{"微网侧接触器故障","MG side Contact Fault"},
	{"",""},
	{"",""},
	{"负载防雷故障","SPD Fault"},
	{"电池电量低","Low Battery"},
	{"BMS故障","BMS Fault"},
	{"自检失败","Self Check Fail"},
	{"自检超时","Self Check Timeout"},
	{"微网供电启动超时","MG Start Timeout"},
	{"微网供电启动失败","MG Start Fail"},
	{"电网供电启动超时","Grid Start Timeout"},
	{"电网供电启动失败","Grid Start Fail"},
	{"",""},
	{"同步失败","Sync Failed"},
	{"停机超时","Stop Timeout"},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},//32
	{"节点1故障","Node 1 Fault"},
	{"节点2故障","Node 2 Fault"},
	{"节点3故障","Node 3 Fault"},
	{"节点4故障","Node 4 Fault"},
	{"节点5故障","Node 5 Fault"},
	{"节点6故障","Node 6 Fault"},
	{"节点7故障","Node 7 Fault"},
	{"节点8故障","Node 8 Fault"},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"DSP通讯故障","DSP Com-fault"},//53
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"序号","No."},
	{"日期&时间","Date&Time"},
	{"故障名称","Fault Name"}
};

const char* Alarm_Name[36][2]=
{
	{"",""},
	{"储能变流器故障告警","PCS Fault Alarm"},
	{"储能变流器通讯故障告警","PCS Comm Fault Alarm"},
	{"光伏逆变器故障告警","PV Fault Alarm"},
	{"光伏逆变器通讯故障告警","PV Comm Fault Alarm"},
	{"电网侧接触器告警","Grid-side Contact Alarm"},
	{"微网侧接触器告警","MG-side Contact Alarm"},
	{"储能逆变器交流未连接","PCS AC-side Disconnect"},
	{"一组或多组电池电量低","One or More Low Battery"},
	{"一组或多组电池电量高","One or More High Battery"},
	{"BMS通讯故障告警","BMS Comm Fault Alarm"},
	{"BMS故障告警","BMS Fault Alarm"},
	{"BMS告警","BMS Alarm"},
	{"DCDC通讯故障告警","DCDC Comm Fault Alarm"},
	{"DCDC故障告警","DCDC Fault Alarm"},
	{"DSP通讯故障告警","DSP Comm Fault Alarm"},
	{"电表通讯故障告警","Ammeter Comm Fault Alarm"},
	{"汇流箱通讯故障告警","PVS Comm Fault Alarm"},
	{"环境检测仪通讯故障告警","Env-Detector Comm Fault Alarm"},
	{"",""},
	{"",""},
	{"节点1告警","Node 1 Alarm"},
	{"节点2告警","Node 2 Alarm"},
	{"节点3告警","Node 3 Alarm"},
	{"节点4告警","Node 4 Alarm"},
	{"节点5告警","Node 5 Alarm"},
	{"节点6告警","Node 6 Alarm"},
	{"节点7告警","Node 7 Alarm"},
	{"节点8告警","Node 8 Alarm"},
	{"",""},
	{"",""},
	{"",""},
	{"",""},//32
	{"序号","No."},
	{"日期&时间","Date&Time"},
	{"告警名称","Alarm Name"}
};

const char* GridStatus_Name[2][2]=
{
	{"无电","MisGrid"},
	{"有电","Grid"}
};

const char* ContactorStatus_Name[2][2]=
{
	{"断开","Opened"},
	{"闭合","Closed"}
};

const char* FaultStatus_Name[2][2]=
{
	{"正常","Normal"},
	{"故障","Fault"}
};

const char* PcsType_Name[5][2]=
{
	{"SC30/SC50", "SC30/SC50"},
	{"SC100", "SC100"},
	{"SC250", "SC250"},
	{"SC500", "SC500"},
	{"SC500TL", "SC500TL"}
};

const char* Enable_Name[2][2]=
{
	{"禁止","Disable"},
	{"使能","Enable"}
};

const char* BusType_Name[2][2]=
{
	{"交流母线","DC-BUS"},
	{"直流母线","AC-BUS"}
};

const char* RunMode_Name[3][2]=
{
	{"并网", "Grid"},
	{"离网", "Off-Grid"},
	{"并离网切换", "Grid-OnOFF"}
};

const char* CtrlMode_Name[14][2]=
{
	{"固定功率", "Fixed Power"},
	{"跟踪负载", "Tacking Load"},
	{"PCC点功率控制", "PCC Control"},
	{"防逆流", "Anti Reflux"},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"",""},
	{"削峰填谷","Peak Load Shifting"},
	{"交流限发","AC Limited Produce"},
	{"输出平滑","OutPut Smooth"},
	{"调频","FM"}
};

const char* GridMode_Name[6][2]=
{
	{"恒流", "CC-Mode"},
	{"恒压", "CV-Mode"},
	{"恒直流功率", "CP-Mode(DC)"},
	{"恒交流功率", "CP-Mode(AC)"},
	{"待机", "Standby-Mode"},
	{"停机", "Stop"}
};

const char* Export_Name[31][2]=
{
	{"序号","No."},
	{"日期/时间","Date/Time"},
	{"储能型号","PCS Type"},        //SC30/SC50(0),SC100,SC250,SC500,SC500TL
	{"系统额定功率","Sys Rated Power"},//
	{"并网使能","Grid Enable"},    //使能/禁止
	{"总线类型","Bus Type"},       //直流母线/交流母线
	{"分时使能","Time Enable"},    //使能/禁止
	{"工作状态","Working Status"},
	{"电网状态","Grid Status"},    //无电/有电
	{"电网电压","Grid Voltage"},
	{"电网频率","Grid Frequency"},
	{"电网功率","Grid Power"},
	{"电网接触器状态","Grid Contactor Status"},   //断开/闭合
	{"电网接触器故障状态","Grid Contactor Fault Status"},  //正常/故障
	{"微网接触器状态","MGrid Contactor Status"},  //断开/闭合
	{"微网接触器故障状态","MGrid COntactor Fault Status"}, //正常/故障
	{"运行模式","Running Mode"},  //0-并网 1-离网 2-并离网切换
	{"功率控制模式", "Power Control Mode"}, //0-固定功率 1-跟踪负载 2-PCC点功率控制 4-上位机调度
	{"运行功率","Operating Power"},
	{"储能并网工作模式","PCS Working Mode"},//0-恒流 1-恒压 2-恒直流功率 3-恒交流功率 4-待机 5-停机
	{"储能逆变器有功功率","PCS Active Power"},
	{"微网电压","MGrid Voltage"},
	{"微网频率","MGrid Frequency"},
	{"光伏逆变器功率","PV Power"},
	{"光伏逆变器电压","PV Voltage"},
	{"负载功率","Load Power"},
	{"负载电压","Load Voltage"},
	{"电池电压最大值","Battery Voltage Max"},
	{"电池电压最小值","Battery Voltage Min"},
	{"电池SOC最大值","Battery SOC Max"},
	{"电池SOC最小值","Battery SOC Min"}
};

size_t get_wchar_size(const char *str)
{
	size_t len = strlen(str);
	size_t size=0;
	size_t i;
	for(i=0; i < len; i++)
	{
	   if( str[size] >= 0 && str[size] <= 127 ) //不是全角字符
	   size+=sizeof(wchar_t);
	   else //是全角字符，是中文
	   {
		   size+=sizeof(wchar_t);
		   i+=2;
	   }
	}
	return size;
}
char *w2c(const wchar_t *pw)
{
	setlocale(LC_ALL,"zh_CN.utf8");
	if(!pw) return NULL;

	size_t size= wcslen(pw)*sizeof(wchar_t);
	char *pc;
	if(!(pc = (char*)malloc(size)))
	{
	   printf("malloc fail");
	   return NULL;
	}
	wcstombs(pc,pw,size);
	return pc;
}
wchar_t *c2w(const char *pc)
{
	setlocale(LC_ALL,"zh_CN.utf8");
	if(!pc) return NULL;

	//size_t size_of_ch = strlen(pc)*sizeof(char);
	size_t size_of_wc = get_wchar_size(pc);
	wchar_t *pw;
	if(!(pw = (wchar_t*)malloc(size_of_wc)))
	{
	   printf("malloc fail");
	   return NULL;
	}
	mbstowcs(pw,pc,size_of_wc);
	return pw;
}

bool utf8_to_unicode (char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
  char *encTo = (char*)"gb2312";  // 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换 IGNORE ：遇到无法转换字符跳过
  char *encFrom = (char*)"utf-8";  // 源编码

  // encTo -- 目标编码方式 encFrom -- 源编码方式
  iconv_t cd = iconv_open (encTo, encFrom);
  if (-1 == (int)cd)
  {
	  perror ("iconv");
	  return 0;
  }

  /* 由于iconv()函数会修改指针，所以要保存源指针 */
  char *tmpin = inbuf;
  char *tmpout = outbuf;

  /* 进行转换
   *@param cd iconv_open()产生的句柄
   *@param srcstart 需要转换的字符串
   *@param inlen 存放还有多少字符没有转换
   *@param tempoutbuf 存放转换后的字符串
   *@param outlen 存放转换后,tempoutbuf剩余的空间
   *
   * */
  iconv (cd, &tmpin, &inlen, &tmpout, &outlen);

  /* 存放转换后的字符串 */
  printf("outbuf=%s, ", outbuf);
  //存放转换后outbuf剩余的空间
  printf("outlen=%d\n", outlen);
  for (size_t i=0; i<outlen; i++){printf("%x\n", outbuf[i]);}

  /* 关闭句柄 */
  iconv_close (cd);

  return 0;
}

//--------- UTF-8转Unicode，单字 ----------------
int cU8xU(wchar_t* pOut,char *pText)
{
	int ret = 0;
	char* uchar = (char *)pOut;
	unsigned cIn = (unsigned char)pText[0];
	if(cIn<0x80){ // ASCII 0x00 ~ 0x7f
		pOut[0] = pText[0];
	}else if(cIn<0xdf){
		uchar[0] = (pText[0]<<6)|(pText[1]&0x3f);
		uchar[1] = (pText[0]>>2)&0x0f;
		ret = 1;
	}else if(cIn<=0xef){
		uchar[0] = (pText[1]<<6)|(pText[2]&0x3f);
		uchar[1] = (pText[0]<<4)|((pText[1]>>2)&0x0f);
		ret = 2;
	}else if(cIn<0xf7){
		uchar[0] = (pText[2]<<6)|(pText[3]&0x3f);
		uchar[1] = (pText[1]<<4)|((pText[2]>>2)&0x0f);
		uchar[2] = ((pText[0]<<2)&0x1c)|((pText[1]>>4)&0x03);
		ret = 3;
	}
	return ret;
}
// ------------ UTF-8转Unicode，字符串 ----------------
int sU8xU(wchar_t* pOut,char *pText,int Len){
	int i,j;
	for(i=0,j=0;i<Len;i++,j++){
		i+=cU8xU(&pOut[j],&pText[i]);
	}
	return j;
}
// ----------- Unicode转GB2312，单字 ------------------
int cUxG(char* pOut,wchar_t* pText)
{
	int ret = 0;
	if(pText[0]<0x80){        // ASCII  0x00 ~ 0x7f
		pOut[0] = (char)pText[0];
	}else{
		//::WideCharToMultiByte(CP_ACP,0,pText,1,pOut,sizeof(WCHAR),NULL,NULL);
		mbstowcs(pText,pOut,1); //单字节转多字节
		ret = 1;
	}
	return ret;
}
// ------------ UTF-8转GB2312，字符串 ------------------
int sU8xG(char* pOut,char* pText,int Len){
	int i,j;
	wchar_t buf;
	for(i=0,j=0;i<Len;i++,j++){
	if((unsigned)pText[0]<0x80){       // ASCII  0x00 ~ 0x7f
		pOut[j] = pText[i];
	}else{
		i += cU8xU(&buf,&pText[i]);
		j += cUxG(&pOut[j],&buf);
	}
	}
	return j;
}
// ---------- Unicode转UTF-8，单字 ------------------
int cUxU8(char* pOut,wchar_t* pText)
{
	int ret = 0;
	unsigned char* pchar = (unsigned char *)pText;
	if(pText[0]<=0x7f){         // ASCII  0x00 ~ 0x7f
		pOut[0] = (char)pchar[0];
	}else if(pText[0]<=0x7ff){  // 0x080 ~ 0x7ff
		pOut[0] = 0xc0|(pchar[1]<<2)|(pchar[0]>>6);
		pOut[1] = 0x80|(pchar[0]&0x3f);
		ret = 1;
	}else{                      // 0x0800 ~ 0xFFFF
		pOut[0] = 0xe0|(pchar[1]>>4);
		pOut[1] = 0x80|((pchar[1]&0x0f)<<2)|(pchar[0]>>6);
		pOut[2] = 0x80|(pchar[0]&0x3f);
		ret = 2;
	}
	return ret;
}
// ----------- GB2312转Unicode，单字 ------------------
int cGxU(wchar_t* pOut,char* pText){
	int ret = 0;
		if((unsigned)pText[0]<0x80){        // ASCII  0x00 ~ 0x7f
			pOut[0] = (wchar_t)pText[0];
	}else{
		wcstombs(pText,pOut,1);   //多字节转单字节
		ret = 1;
	}
	return ret;
}

// ------------ GB2312转UTF-8，字符串 ------------------
int sGxU8(char* pOut,char* pText,int Len)
{
	int i,j;
	wchar_t buf;
	for(i=0,j=0;i<Len;i++,j++){
		if((unsigned)pText[0]<0x80){       // ASCII  0x00 ~ 0x7f
			pOut[j] = pText[i];
		}else{
			i += cGxU(&buf,&pText[i]);
			j += cUxU8(&pOut[j],&buf);
		}
	}
	return j;
}

static int super_system(const char * cmd)
{
	FILE * fp;

	if (cmd == NULL) return -1;

	if ((fp = popen(cmd, "w") ) == NULL){   // "w" "r"
		//printf("POPEN error: %s\n", strerror(errno));
		return -1;
	}else{
		pclose(fp);
	}
	return 0;
}

uint32_t return_max_u32(uint32_t *arr,uint8_t k,uint8_t num)    //返回32位无符号数中，最大的数
{
    uint32_t max=arr[k];
    for(uint16_t i=k;i<num;i++)
    {
        if(max<arr[i])
        {
            max=arr[i];
        }
    }
    if(max==0)return 100;
    else return (((max-1)/100)+1)*100;
}
void set_param(uint16_t num, uint16_t id, int32_t val)         //设置参数数组的保存值
{
    Param_flag[num]=id;
    Param_value[num]=val;
    //save_flag_int=1;
    webserver_do_save_int();
}
uint16_t do_save_data_int(uint16_t id,int32_t INT32_Val)        //设置参数
{
    //int16_t S16_Val = (int16_t)INT32_Val;
    //uint8_t Address_used=0;
    static uint8_t mac_temp[6]={0};

    if(id < 0x0490)
    {
    	switch(id)
		{
			case 0x001F: //当前显示的PCS序号
				if(INT32_Val <= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number &&
					INT32_Val > 0)
				{
					Cur_PCS_Num=INT32_Val;
				}
			break;
			case 0x0040: //当前显示的PVS序号
				if(INT32_Val <= PVS_NUM && INT32_Val > 0)
				{
					Cur_PVS_Num=INT32_Val;
				}
			break;
			case 0x0050: //当前显示的BMS序号
				if(INT32_Val <= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number && INT32_Val > 0)
				{
					Cur_BMS_Num=INT32_Val;
				}
			break;
			case 0x0070: //当前显示的DCDC序号
				if(INT32_Val <= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number && INT32_Val > 0)
				{
					Cur_DCDC_Num=INT32_Val;
				}
			break;
			case 0x0130: //当前显示的PV序号
				if(INT32_Val <= EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number &&
					INT32_Val > 0)
				{
					Cur_PV_Num=INT32_Val;
				}
			break;
			case 0x0157://IP地址[0]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.ip[0] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_IP_0_ADDRESS, &System_Info.Net.ip[0], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x0158://IP地址[1]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.ip[1] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_IP_1_ADDRESS, &System_Info.Net.ip[1], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x0159://IP地址[2]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.ip[2] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_IP_2_ADDRESS, &System_Info.Net.ip[2], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x015A://IP地址[3]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.ip[3] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_IP_3_ADDRESS, &System_Info.Net.ip[3], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x015B://子网掩码[0]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.submask[0] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_SUBMASK_0_ADDRESS, &System_Info.Net.submask[0], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x015C://子网掩码[1]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.submask[1] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_SUBMASK_1_ADDRESS, &System_Info.Net.submask[1], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x015D://子网掩码[2]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.submask[2] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_SUBMASK_2_ADDRESS, &System_Info.Net.submask[2], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x015E://子网掩码[3]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.submask[3] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_SUBMASK_3_ADDRESS, &System_Info.Net.submask[3], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x015F://网关[0]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.gateway[0] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_GATEWAY_0_ADDRESS, &System_Info.Net.gateway[0], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x0160://网关[1]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.gateway[1] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_GATEWAY_1_ADDRESS, &System_Info.Net.gateway[1], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x0161://网关[2]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.gateway[2] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_GATEWAY_2_ADDRESS, &System_Info.Net.gateway[2], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x0162://网关[3]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net.gateway[3] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET_GATEWAY_3_ADDRESS, &System_Info.Net.gateway[3], 1);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x0163://波特率
				if(1 <= INT32_Val && INT32_Val <= 4)
				{
					System_Info.Baud_Rate=(uint8_t)INT32_Val;
					(void)write_eeprom(BAUD_RATE_ADDRESS, &System_Info.Baud_Rate, 1);
					uint16_t baudrate=0;
					switch(System_Info.Baud_Rate)
					{
						case 1:baudrate=4800;break;
						case 2:baudrate=9600;break;
						case 3:baudrate=19200;break;
						case 4:baudrate=38400;break;
					}
					for(int i=0; i<6; i++)
					{
						if(ctrl.serialRev_thread[i].GetProtocol()==SERIAL_PROTOCOL_MODBUS_PCS)
							ctrl.serialRev_thread[i].Set_Baud(baudrate);
					}
				}
			break;
			case 0x0164://上位机通讯地址
				if((1 <= INT32_Val) && (INT32_Val <= 247))
				{
					System_Info.Modbus_Address=(uint8_t)INT32_Val;
					(void)write_eeprom(MODBUS_ADDRESS_ADDRESS, &System_Info.Modbus_Address, 1);
				}
			break;
			case 0x0165://端口号
				if((1 <= INT32_Val) && (INT32_Val <= UINT16_MAX) && (INT32_Val != WEBSERVER_PORT) && (INT32_Val != IEC104_PORT))
				{
					System_Info.Modbus_Port = INT32_Val;
					(void)write_eeprom(NET_PORT_ADDRESS, &System_Info.Modbus_Port, 2);
					Com_Data.Touch_Set_Net_Flag = true;
				}
			break;
			case 0x0166://BMS通讯方式
				if((INT32_Val==COM_BMS_MODE_CAN) || (INT32_Val==COM_BMS_MODE_UART485) || (INT32_Val==COM_BMS_MODE_NONE))
				{
	//              System_Info.IO_To_BMS_Mode=(uint8_t)INT32_Val;
	//              (void)write_eeprom(IO_BMS_COM_MODE_ADDRESS, &System_Info.IO_To_BMS_Mode, 1);
	//              if((uint8_t)INT32_Val==COM_BMS_MODE_NONE)
	//              {
	//                  System_Info.BMS_Fault_Contact_Enabled = SWITCH_OFF;
	//                  (void)write_eeprom(BMS_FAULT_CONTACT_ENABLED_ADDRESS, &System_Info.BMS_Fault_Contact_Enabled, 1);
	//                  System_Info.BMS_Warning_Contact_Enabled = SWITCH_OFF;
	//                  (void)write_eeprom(BMS_WARNING_CONTACT_ENABLED_ADDRESS, &System_Info.BMS_Warning_Contact_Enabled, 1);
	//              }
				}
			break;
			case 0x0167://BMS故障干接点使能
	//          if(System_Info.IO_To_BMS_Mode!=COM_BMS_MODE_NONE)
	//          {
	//              if((INT32_Val == SWITCH_OFF) || (INT32_Val == SWITCH_ON_OPEN) || (INT32_Val == SWITCH_ON_CLOSE))
	//              {
	//                  System_Info.BMS_Fault_Contact_Enabled = (uint8_t)INT32_Val;
	//                  (void)write_eeprom(BMS_FAULT_CONTACT_ENABLED_ADDRESS, &System_Info.BMS_Fault_Contact_Enabled, 1);
	//              }
	//          }
			break;
			case 0x0168://BMS告警干接点使能
	//          if(System_Info.IO_To_BMS_Mode!=COM_BMS_MODE_NONE)
	//          {
	//              if((INT32_Val == SWITCH_OFF) || (INT32_Val == SWITCH_ON_OPEN) || (INT32_Val == SWITCH_ON_CLOSE))
	//              {
	//                  System_Info.BMS_Warning_Contact_Enabled = (uint8_t)INT32_Val;
	//                  (void)write_eeprom(BMS_WARNING_CONTACT_ENABLED_ADDRESS, &System_Info.BMS_Warning_Contact_Enabled, 1);
	//              }
	//          }
			break;
			case 0x0169://BMS1通讯地址
	//          if((1 <= INT32_Val) && (INT32_Val <= 247))
	//          {
	//              System_Info.BMS_Address[0]=(uint8_t)INT32_Val;
	//              (void)write_eeprom(BMS_ADDRESS_ADDRESS, &System_Info.BMS_Address, 1);
	//          }
			break;
			case 0x016A://IEC104通讯地址
				if((1 <= INT32_Val) && (INT32_Val <= 65535))
				{
					System_Info.IEC104_Address=INT32_Val;
					(void)write_eeprom(IEC104_ADDRESS_ADDRESS, &System_Info.IEC104_Address, 2);
				}
			break;
			case 0x016B://IEC104超时时间T1
				if((1 <= INT32_Val) && (INT32_Val < System_Info.IEC104_T3))
				{
					System_Info.IEC104_T1=INT32_Val;
					(void)write_eeprom(IEC104_T1_ADDRESS, &System_Info.IEC104_T1, 2);
				}
			break;
			case 0x016C://IEC104超时时间T3
				if((System_Info.IEC104_T1 < INT32_Val) && (INT32_Val <= 255))
				{
					System_Info.IEC104_T3=INT32_Val;
					(void)write_eeprom(IEC104_T3_ADDRESS, &System_Info.IEC104_T3, 2);
				}
			break;
			case 0x016D://IEC104参数k
				if((System_Info.IEC104_W<=((uint16_t)INT32_Val*2/3))&&(INT32_Val<=32767))
				{
					System_Info.IEC104_K = INT32_Val;
					(void)write_eeprom(IEC104_K_ADDRESS,&System_Info.IEC104_K,2);
				}
			break;
			case 0x016E://IEC104参数w
				if((1<=INT32_Val)&&(INT32_Val<=((uint16_t)System_Info.IEC104_K*2/3)))
				{
					System_Info.IEC104_W = INT32_Val;
					(void)write_eeprom(IEC104_W_ADDRESS,&System_Info.IEC104_W,2);
				}
			break;
			case 0x016F://BMS2通讯地址
	//          if((1 <= INT32_Val) && (INT32_Val <= 247))
	//          {
	//              System_Info.BMS_Address[1]=(uint8_t)INT32_Val;
	//              (void)write_eeprom(BMS_ADDRESS_ADDRESS, &System_Info.BMS_Address, 1);
	//          }
			break;
			case 0x0170://系统时钟（年）
				if(2000<=INT32_Val&&INT32_Val<=2099)
				{
					Touch_Write_Time_Flag=true;
					Touch_Write_Time[0]=(uint8_t) (INT32_Val - 2000);//((uint8_t)(INT32_Val-2000));
				}
			break;
			case 0x0171://系统时钟（月）
				if(INT32_Val >= 0 && INT32_Val<=12)
				{
					Touch_Write_Time_Flag=true;
					Touch_Write_Time[1]=INT32_Val;//((uint8_t)INT32_Val);
				}
			break;
			case 0x0172://系统时钟（日）
				if(INT32_Val >= 0 && INT32_Val<=31)
				{
					Touch_Write_Time_Flag=true;
					Touch_Write_Time[2]=INT32_Val;//((uint8_t)INT32_Val);
				}
			break;
			case 0x0173://系统时钟（时）
				if(INT32_Val >= 0 && INT32_Val<=23)
				{
					Touch_Write_Time_Flag=true;
					Touch_Write_Time[3]=INT32_Val;//((uint8_t)INT32_Val);
				}
			break;
			case 0x0174://系统时钟（分）
				if(INT32_Val >= 0 && INT32_Val<=59)
				{
					Touch_Write_Time_Flag=true;
					Touch_Write_Time[4]=INT32_Val;//((uint8_t)INT32_Val);
				}
			break;
			case 0x0175: //恒流充电电流
				if(SYS_NO_FAULT)
				{
				}
			break;
			case 0x0176: //恒压充电限流值
				if(SYS_NO_FAULT)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Min &&
						INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Max)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I=INT32_Val;
						write_eeprom(CONST_V_MODE_I_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I, 2);
					}
				}
			break;
			case 0x0177://恒压充电电压
				if(SYS_NO_FAULT)
				{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min &&
						INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max)
					{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V=INT32_Val;
							write_eeprom(CONST_V_MODE_V_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V, 2);
					}
				}
			break;
			case 0x0178: //恒DC功率充电功率
				if(SYS_NO_FAULT)
				{

				}
			break;
			case 0x0179: //恒AC功率充电功率
				if(SYS_NO_FAULT)
				{
	//              if((S16_Val>=EMS_Info.EMS4x.EMS_Controller_Info.EM.Const_P_AC_Mode_P_Min)&&
	//                 (S16_Val<=EMS_Info.EMS4x.EMS_Controller_Info.EM.Const_P_AC_Mode_P_Max))
	//              {
	//                  EMS_Info.EMS4x.EMS_Controller_Info.EM.Const_P_AC_Mode_P = S16_Val;
	//                  write_eeprom(CONST_P_AC_MODE_P_VSG_ADDRESS,&EMS_Info.EMS4x.EMS_Controller_Info.EM.Const_P_AC_Mode_P,2);
	//              }
				}
			break;
			case 0x017A://恒流放电电流
				if(SYS_NO_FAULT)
				{

				}
			break;
			case 0x017B: //限压放电电压
				if(SYS_NO_FAULT)
				{

				}
			break;
			case 0x017C: //负载总用电量校正
				if(INT32_Val==0)
				{
					System_Run_Info.E_Load_Adjust=0;
				}
				else if(INT32_Val < 0)
				{
					if(System_Run_Info.Load_Energy_Acc>=(((uint64_t)(abs(INT32_Val)))*36000))
					{
						System_Run_Info.Load_Energy_Acc -= (((uint64_t)(abs(INT32_Val)))*36000);
						System_Run_Info.E_Load_Adjust=(int16_t)INT32_Val;
						System_Run_Info.Total_Load_Energy=(uint32_t)(System_Run_Info.Load_Energy_Acc/36000);
						write_eeprom(LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Load_Energy_Acc, 8);
						write_eeprom(TOTAL_LOAD_ENERGY_ADDRESS,&System_Run_Info.Total_Load_Energy,4);
					}
					else
					{
						System_Run_Info.E_Load_Adjust=0;
					}
				}
				else
				{
					if((System_Run_Info.Load_Energy_Acc+(((uint64_t)INT32_Val)*36000))>=System_Run_Info.Load_Energy_Acc)
					{
						System_Run_Info.Load_Energy_Acc += ((uint64_t)INT32_Val)*36000;
						System_Run_Info.E_Load_Adjust=(int16_t)INT32_Val;
						System_Run_Info.Total_Load_Energy=(uint32_t)(System_Run_Info.Load_Energy_Acc/36000);
						write_eeprom(LOAD_ENERGY_ACC_ADDRESS, &System_Run_Info.Load_Energy_Acc, 8);
						write_eeprom(TOTAL_LOAD_ENERGY_ADDRESS,&System_Run_Info.Total_Load_Energy,4);
					}
					else
					{
						System_Run_Info.E_Load_Adjust=0;
					}
				}
			break;
			case 0x017D://并网运行功率设置
				if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set=(int16_t)INT32_Val;
					write_eeprom(EMS_RUN_POWER_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set, 2);
				}
			break;
			case 0x017E: //PCC点有功控制
				EMS_Info.EMS4x.EMS_Controller_Info.EM.PCC_Active_Power_Giving=(int16_t)INT32_Val;
				write_eeprom(PCC_ACTIVE_POWER_GIVING_ADDRESS, &EMS_Info.EMS4x.EMS_Controller_Info.EM.PCC_Active_Power_Giving, 2);
			break;
			case 0x017F://PCC点无功控制
				EMS_Info.EMS4x.EMS_Controller_Info.EM.PCC_Reactive_Power_Giving=(int16_t)INT32_Val;
				write_eeprom(PCC_REACTIVE_POWER_GIVING_ADDRESS, &EMS_Info.EMS4x.EMS_Controller_Info.EM.PCC_Reactive_Power_Giving, 2);
			break;
			case 0x0180: //独立逆变电压
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Max)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage=INT32_Val;
					write_eeprom(MG_VOLTAGE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage, 2);
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Ctrl_Data[i].Ind_Inverter_V=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;
						Send_Data_To_PCS_0x10_02[i]=true;
					}
				}
			break;
			case 0x0181: //独立逆变频率
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Max)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency=INT32_Val;
					write_eeprom(MG_FREQUENCY_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency, 2);
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						PCS_Ctrl_Data[i].Ind_Inverter_F=EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency;
						Send_Data_To_PCS_0x10_02[i]=true;
					}
				}
			break;
			case 0x0182://pcs运行模式
				if ((INT32_Val == GRID_CONST_I_MODE) || (INT32_Val == GRID_CONST_V_MODE) || (INT32_Val == GRID_CONST_P_DC_MODE) || (INT32_Val == GRID_CONST_P_AC_MODE) || (INT32_Val == GRID_STANDBY_MODE))
				{
					System_Info.Config.PCS_Grid_Run_Mode = (uint8_t) INT32_Val;
					write_eeprom(PCS_GRID_RUN_MODE_ADDRESS, &System_Info.Config.PCS_Grid_Run_Mode, 1);
				}
			break;
			case 0x0183: //远程本地控制模式
				if(INT32_Val==RE_LO_MODE || INT32_Val==REMOTE_MODE || INT32_Val==LOCAL_MODE)
				{
					EMS_RL_Mode=INT32_Val;
					write_eeprom(EMS_RL_MODE_ADDRESS, &EMS_RL_Mode, 1);
				}
			break;
			case 0x0184:    //无功调节选择
				if((INT32_Val==QP)||(INT32_Val==PF)||(INT32_Val==SHUT_OFF))
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode=(int16_t)INT32_Val;
					write_eeprom(EMS_RUN_Q_ADJ_MODE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode, 2);

					if(SHUT_OFF==EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=0; // caoxh
						EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
						write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
						write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
					}
					else if(PF==EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode)
					{
						 EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=0; // caoxh
						 write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);
					}
					else if(QP==EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode)
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
						write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
					}

					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
					{
						//PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode=EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
						PCS_Info[i].PCS4x.RControl.Qvar_Adj_Mode=EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;
						PCS_Ctrl_Data[i].Power_Factor=EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
						Send_Data_To_PCS_0x10_02[i]=true;
					}
				}
			break;
			case 0x0185://功率因数
				if(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode==PF)
				{
					if (((abs((int16_t) INT32_Val)) >= P_Range[System_Info.Config.PCS_Type].Power_Factor.Min) &&
							((abs((int16_t) INT32_Val)) <= P_Range[System_Info.Config.PCS_Type].Power_Factor.Max))
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=(int16_t)INT32_Val;
						write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);

						EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=0; // caoxh
						write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);

						for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
						{
							PCS_Ctrl_Data[i].Power_Factor=EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;
							Send_Data_To_PCS_0x10_02[i]=true;
						}
					}
				}
			break;
			case 0x0186://无功功率
				if(EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode==QP)
				{
					if(((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*0.3)&&((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*0.3))
					{
						EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set=(int16_t)INT32_Val;
						write_eeprom(EMS_RUN_Q_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set, 2);

						EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor=P_Range[System_Info.Config.PCS_Type].Power_Factor.Dft; // caoxh
						write_eeprom(EMS_RUN_P_FACTOR_SET_VALUE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor, 2);
					}
				}
			break;
			case 0x0187: //并网模式设置 0：恒流模式、1：恒压模式 2：恒功率模式（DC）3：恒功率模式（AC） 4：待机
				if((INT32_Val==GRID_CONST_I_MODE)||(INT32_Val==GRID_CONST_V_MODE)||(INT32_Val==GRID_CONST_P_DC_MODE)
				|| (INT32_Val==GRID_CONST_P_AC_MODE)||(INT32_Val==GRID_STANDBY_MODE))
				{
					System_Info.Config.PCS_Grid_Run_Mode=(uint8_t)INT32_Val;
					write_eeprom(PCS_GRID_RUN_MODE_ADDRESS, & System_Info.Config.PCS_Grid_Run_Mode, 1);
				}
			break;
			case 0x0188:    //离网主被动模式设置
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val==ACTIVE_MODE || INT32_Val==REACTIVE_MODE)
					{
						System_Info.Config.EMS_Start_Type=(uint8_t)INT32_Val;
						write_eeprom(EMS_START_TYPE_ADDRESS, &System_Info.Config.EMS_Start_Type, 1);
					}
            	}
			break;
			case 0x0189:    //起停控制
				if(INT32_Val==START_COMMAND)
				{
					Sys_Ctrl.Sys_Start=true;
				}
				if(INT32_Val==STOP_COMMAND)
				{
					Sys_Ctrl.Manual_Stop=true;
				}
			break;
			case 0x018A:    //BMS总控制
				if(1==INT32_Val)
				{
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
					{
						BMS_DC_Contactor_off_cmd[i]=true;
						BMS_Switch_Control[i]=INT32_Val;
					}
				}
				else if(2==INT32_Val)
				{
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
					{
						BMS_DC_Contactor_on_cmd[i]=true;
						BMS_Switch_Control[i]=INT32_Val;
					}
				}
				else if(3==INT32_Val)
				{
					for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
					{
						BMS_All_Fault_Reset_cmd[i]=true;
						BMS_Switch_Control[i]=INT32_Val;
					}
				}
			break;
			case 0x018C:    //VSG输出电压给定值
				if(SYS_NO_FAULT)
				{
	//              if(INT32_Val>=VSG_Range[VSG_U0].Min&&INT32_Val<=VSG_Range[VSG_U0].Max)
	//              {
	//                  VSG_Para.VSG_u0_set = INT32_Val;
	//                  write_eeprom(VSG_U0_VALUE_ADDRESS,&VSG_Para.VSG_u0_set,2);
	//                  Send_Data_To_PCS_Broadcast = true;
	//              }
				}
			break;
			case 0x018D:    //VSG输出频率给定值
				if(SYS_NO_FAULT)
				{
	//              if(INT32_Val>=VSG_Range[VSG_W0].Min&&INT32_Val<=VSG_Range[VSG_W0].Max)
	//              {
	//                  VSG_Para.VSG_w0_set = INT32_Val;
	//                  write_eeprom(VSG_W0_VALUE_ADDRESS,&VSG_Para.VSG_w0_set,2);
	//                  Send_Data_To_PCS_Broadcast = true;
	//              }
				}
			break;
			case 0x018E:        //有功下垂控制系数
				if(SYS_NO_FAULT)
				{
	//              if(INT32_Val>=VSG_Range[VSG_P_DOWN_CONTROL_RATIO].Min&&INT32_Val<=VSG_Range[VSG_P_DOWN_CONTROL_RATIO].Max)
	//              {
	//                  VSG_Para.VSG_P_down_control_ratio = INT32_Val;
	//                  write_eeprom(VSG_P_DOWN_CONTROL_RATIO_ADDRESS,&VSG_Para.VSG_P_down_control_ratio,2);
	//                  Send_Data_To_PCS_Broadcast = true;
	//              }
				}
			break;
			case 0x018F:        //无功下垂控制系数
				if(SYS_NO_FAULT)
				{
	//              if(INT32_Val>=VSG_Range[VSG_Q_DOWN_CONTROL_RATIO].Min&&INT32_Val<=VSG_Range[VSG_Q_DOWN_CONTROL_RATIO].Max)
	//              {
	//                  VSG_Para.VSG_Q_down_control_ratio= INT32_Val;
	//                  write_eeprom(VSG_Q_DOWN_CONTROL_RATIO_ADDRESS,&VSG_Para.VSG_Q_down_control_ratio,2);
	//                  Send_Data_To_PCS_Broadcast = true;
	//              }
				}
			break;
			case 0x0190:        //二次调频给定值
				if(SYS_NO_FAULT)
				{
	//              int16_t temp=(int16_t)INT32_Val;
	//              if(temp>=VSG_Range[VSG_PREF].Min && temp<=VSG_Range[VSG_PREF].Max)
	//              {
	//                  VSG_Para.VSG_Pref_set = temp;
	//                  write_eeprom(VSG_PREF_VALUE_ADDRESS,&VSG_Para.VSG_Pref_set,2);
	//              }
				}
			break;
			case 0x0191:        //无功下发给定值
				if(SYS_NO_FAULT)
				{
	//              int16_t temp=(int16_t)INT32_Val;
	//              if(temp>=VSG_Range[VSG_QREF].Min&&temp<=VSG_Range[VSG_QREF].Max)
	//              {
	//                  VSG_Para.VSG_Qref_set = temp;
	//                  write_eeprom(VSG_QREF_VALUE_ADDRESS,&VSG_Para.VSG_Qref_set,2);
	//              }
				}
			break;
			case 0x019A: //恒压模式电压下限
				if((INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min) &&
				   (INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max) &&
				   (INT32_Val<EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max))
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min=INT32_Val;
					write_eeprom(CONST_V_MODE_V_MIN_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min, 2);
				}
				break;
			case 0x019B: //恒压模式电压上限
				if((INT32_Val>=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min) &&
				   (INT32_Val<=P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max) &&
				   (INT32_Val>EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min))
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max=INT32_Val;
					write_eeprom(CONST_V_MODE_V_MAX_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max, 2);
				}
				break;
			case 0x0208: //恢复出厂值
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val == 0xAA && Com_Data.Load_Default_Flag != 0xAA)
					{
						pthread_mutex_lock(&mutex_for_eeporm);
						super_system("rm -f /opt/eeprom_bak 2>null");
						usleep(500);
						Com_Data.Load_Default_Flag = 0xAA;
						Com_Data.Load_Defalut_Net_Flag = 0xAA;
						reboot = true;
						Sys_Ctrl.Sys_Stop = true;
						pthread_mutex_unlock(&mutex_for_eeporm);
					}
            	}
			break;
			case 0x0209://用户参数备份
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val == 0xAA)
					{
						pthread_mutex_lock(&mutex_for_eeporm);
						super_system("mkdir -p /opt/userbak/ 2>null");
						usleep(500);
						super_system("cp -f /opt/eeprom /opt/userbak/ 2>null");
						usleep(500);
						pthread_mutex_unlock(&mutex_for_eeporm);
					}
            	}
			break;
			case 0x020A://用户参数恢复
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val == 0xAA)
					{
						pthread_mutex_lock(&mutex_for_eeporm);
						super_system("rm -f /opt/eeprom_bak 2>null");
						usleep(500);
						super_system("cp -f /opt/userbak/eeprom /opt 2>null");
						usleep(500);
						reboot = true;
						Sys_Ctrl.Sys_Stop = true;
						pthread_mutex_unlock(&mutex_for_eeporm);
					}
            	}
			break;
			case 0x020B://MAC 锁定/解锁
				if (INT32_Val == 0 || INT32_Val == 1)
				{
					IAT_Info.Set_Info.MAC_Lock = INT32_Val;
					(void) write_eeprom(IAT_MAC_LOCK_ADDRESS, &IAT_Info.Set_Info.MAC_Lock, 2);
				}
			break;
			case 0x020C://系统重启
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val == 0xAA)
					{
						reboot = true;
						Sys_Ctrl.Sys_Stop = true;
					}
            	}
			break;
			case 0x0210://IP地址[0]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.ip[0] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_IP_0_ADDRESS, &System_Info.Net2.ip[0], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 0x0211://IP地址[1]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.ip[1] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_IP_1_ADDRESS, &System_Info.Net2.ip[1], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 0x0212://IP地址[2]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.ip[2] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_IP_2_ADDRESS, &System_Info.Net2.ip[2], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 0x0213://IP地址[3]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.ip[3] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_IP_3_ADDRESS, &System_Info.Net2.ip[3], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 0x0214://子网掩码[0]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.submask[0] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_SUBMASK_0_ADDRESS, &System_Info.Net2.submask[0], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 0x0215://子网掩码[1]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.submask[1] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_SUBMASK_1_ADDRESS, &System_Info.Net2.submask[1], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 0x0216://子网掩码[2]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.submask[2] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_SUBMASK_2_ADDRESS, &System_Info.Net2.submask[2], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 0x0217://子网掩码[3]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.submask[3] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_SUBMASK_3_ADDRESS, &System_Info.Net2.submask[3], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 0x0218://网关[0]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.gateway[0] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_GATEWAY_0_ADDRESS, &System_Info.Net2.gateway[0], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 0x0219://网关[1]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.gateway[1] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_GATEWAY_1_ADDRESS, &System_Info.Net2.gateway[1], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 0x021A://网关[2]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.gateway[2] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_GATEWAY_2_ADDRESS, &System_Info.Net2.gateway[2], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;
			case 0x021B://网关[3]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.Net2.gateway[3] = (uint8_t)INT32_Val;
					(void)write_eeprom(NET2_GATEWAY_3_ADDRESS, &System_Info.Net2.gateway[3], 1);
					Com_Data.Touch_Set_Net2_Flag = true;
				}
			break;

			case 0x0220://IP地址[0]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.NTP.ip[0] = (uint8_t)INT32_Val;
					(void)write_eeprom(NTP_IP_0_ADDRESS, &System_Info.NTP.ip[0], 1);
					//Com_Data.Touch_Set_NTP_Flag = true;
				}
			break;
			case 0x0221://IP地址[1]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.NTP.ip[1] = (uint8_t)INT32_Val;
					(void)write_eeprom(NTP_IP_1_ADDRESS, &System_Info.NTP.ip[1], 1);
					//Com_Data.Touch_Set_NTP_Flag = true;
				}
			break;
			case 0x0222://IP地址[2]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.NTP.ip[2] = (uint8_t)INT32_Val;
					(void)write_eeprom(NTP_IP_2_ADDRESS, &System_Info.NTP.ip[2], 1);
					//Com_Data.Touch_Set_NTP_Flag = true;
				}
			break;
			case 0x0223://IP地址[3]
				if(INT32_Val >= 0 && INT32_Val <= 255)
				{
					System_Info.NTP.ip[3] = (uint8_t)INT32_Val;
					(void)write_eeprom(NTP_IP_3_ADDRESS, &System_Info.NTP.ip[3], 1);
					//Com_Data.Touch_Set_NTP_Flag = true;
				}
			break;
			case 0x0224:
				if(INT32_Val > 0 && INT32_Val <= 65535)
				{
					System_Info.NTP.port = INT32_Val;
					(void)write_eeprom(NTP_PORT_ADDRESS, &System_Info.NTP.port, 2);
					//Com_Data.Touch_Set_NTP_Flag = true;
					char ipStr[16]={0};
					memset(ipStr, 0x00, 16);
					sprintf(ipStr,"%d.%d.%d.%d", System_Info.NTP.ip[0],System_Info.NTP.ip[1],System_Info.NTP.ip[2],System_Info.NTP.ip[3]);
				    ctrl.ntp.SetServer(ipStr, System_Info.NTP.port);
				}
			break;
			case 0x0225:
				if(INT32_Val == 0x55 || INT32_Val == 0xAA)
				{
					System_Info.NTP.enable = (uint8_t)INT32_Val;
					write_eeprom(NTP_ENABLE_ADDRESS, &System_Info.NTP.enable, 1);
					if(INT32_Val == 0xAA){
						ctrl.ntp.check_ntp_now();
					}
				}
			break;

			case 0x0230:
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[0] = (uint8_t)INT32_Val;
				}
				break;
			case 0x0231: //MAC地址
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[1] = (uint8_t)INT32_Val;
				}
				break;
			case 0x0232: //MAC地址
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[2] = (uint8_t)INT32_Val;
				}
				break;
			case 0x0233: //MAC地址
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[3] = (uint8_t)INT32_Val;
				}
				break;
			case 0x0234: //MAC地址
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[4] = (uint8_t)INT32_Val;
				}
				break;
			case 0x0235: //MAC地址
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[5] = (uint8_t)INT32_Val;
				}
				if (mac_temp[0] == 0xAC && mac_temp[1] == 0x19 && mac_temp[2] == 0x9F)
				{
					IAT_Info.Set_Info.MAC[0] = (mac_temp[0] << 8) | mac_temp[1];
					IAT_Info.Set_Info.MAC[1] = (mac_temp[2] << 8) | mac_temp[3];
					IAT_Info.Set_Info.MAC[2] = (mac_temp[4] << 8) | mac_temp[5];
					//printf("set mac %x %x %x\n",IAT_Info.Set_Info.MAC[0],IAT_Info.Set_Info.MAC[1],IAT_Info.Set_Info.MAC[2]);
					for (uint8_t i = 0; i < 3; i++)
					{
						(void) write_eeprom(IAT_MAC_ADDRESS + i * 2, &IAT_Info.Set_Info.MAC[i], 2);
					}
					for (uint8_t i = 0; i < 6; i++) System_Info.Net.mac[i] = mac_temp[i];
					Com_Data.Touch_Set_Net_Flag = true;
				}
				for(int i=0; i<6; i++) mac_temp[i] = 0x00;
				break;

			case 0x0240:
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[0] = INT32_Val;
				}
				break;
			case 0x0241: //MAC地址
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[1] = INT32_Val;
				}
				break;
			case 0x0242: //MAC地址
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[2] = INT32_Val;
				}
				break;
			case 0x0243: //MAC地址
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[3] = INT32_Val;
				}
				break;
			case 0x0244: //MAC地址
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[4] = INT32_Val;
				}
				break;
			case 0x0245: //MAC地址
				if (INT32_Val >= 0 && INT32_Val <= 0xff && IAT_Info.Set_Info.MAC_Lock != 1) //还未锁定
				{
					mac_temp[5] = INT32_Val;
				}
				if (mac_temp[0] == 0xAC && mac_temp[1] == 0x19 && mac_temp[2] == 0x9F)
				{
					IAT_Info.Set_Info.MAC2[0] = (mac_temp[0] << 8) | mac_temp[1];
					IAT_Info.Set_Info.MAC2[1] = (mac_temp[2] << 8) | mac_temp[3];
					IAT_Info.Set_Info.MAC2[2] = (mac_temp[4] << 8) | mac_temp[5];
					for (uint8_t i = 0; i < 3; i++)
					{
						(void) write_eeprom(IAT_MAC2_ADDRESS + i * 2, &IAT_Info.Set_Info.MAC2[i], 2);
					}
					for (uint8_t i = 0; i < 6; i++) System_Info.Net2.mac[i] = mac_temp[i];
					Com_Data.Touch_Set_Net2_Flag = true;
				}
				for(int i=0; i<6; i++) mac_temp[i] = 0x00;
				break;
			case 0x0301://页面语言
				if(INT32_Val == CHINESE || INT32_Val == ENGLISH)
				{
					Webserver_Language = (uint8_t)INT32_Val;
					(void)write_eeprom(WEBSERVER_LANGUAGE_ADDRESS, &Webserver_Language, 1);
				}
			break;
			case 0x0350: //储能变流器总台数
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].PCS_Total_Num.Max)
					{
						if(INT32_Val > EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number)
						{
							for(uint8_t i=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i<INT32_Val;i++)
							{
								uint8_t j=0;
								while(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number <= 247)
								{
									for(j=0;j<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;j++)
									{
										if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address ==
										   EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[j].Modubus_Address)
										{
											break;
										}
									}
									if(j == EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number)
									{
										EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number++;
										break;
									}
									else
									{
										EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address++;
									}
								}
							}
							//EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number=INT32_Val;
							write_eeprom(TOTAL_PCS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number, 2);
						}
						else
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number=INT32_Val;
							write_eeprom(TOTAL_PCS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number, 2);
						}
					}
            	}
			break;
			case 0x0351: //光伏变流器总台数
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].PV_Total_Num.Max)
					{
						if(INT32_Val > EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number)
						{
							for(uint8_t i=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i<INT32_Val;i++)
							{
								uint8_t j=0;
								while(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number <= 247)
								{
									for(j=0;j<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;j++)
									{
										if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address ==
										   EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[j].Modubus_Address)
										{
											break;
										}
									}
									if(j == EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number)
									{
										EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number++;
										break;
									}
									else
									{
										EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address++;
									}
								}
							}
							//EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number=INT32_Val;
							write_eeprom(TOTAL_PV_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number, 2);
						}
						else
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number=INT32_Val;
							write_eeprom(TOTAL_PV_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number, 2);
						}
					}
            	}
			break;
			case 0x0352: //DCDC台数
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].DCDC_Total_Num.Max)
					{
						if(INT32_Val > EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number)
						{
							for(uint8_t i=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i<INT32_Val;i++)
							{
								uint8_t j=0;
								while(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number <= 247)
								{
									for(j=0;j<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;j++)
									{
										if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address ==
										   EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[j].Modubus_Address)
										{
											break;
										}
									}
									if(j == EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number)
									{
										EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number++;
										break;
									}
									else
									{
										EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address++;
									}
								}
							}
							//EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number=INT32_Val;
							write_eeprom(TOTAL_DCDC_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number, 2);
						}
						else
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number=INT32_Val;
							write_eeprom(TOTAL_DCDC_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number, 2);
						}
					}
            	}
			break;
			case 0x0353: //PVS台数
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].PVS_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].PVS_Total_Num.Max)
				{
					EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number=INT32_Val;
					write_eeprom(TOTAL_PVS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number, 2);
					for (uint8_t i = 0; i < EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number; i++)
					{
						if(INT32_Val > EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number)
						{
							for(uint8_t i=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number;i<INT32_Val;i++)
							{
								uint8_t j=0;
								while(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number <= 247)
								{
									for(j=0;j<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number;j++)
									{
										if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address ==
										   EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[j].Modubus_Address)
										{
											break;
										}
									}
									if(j == EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number)
									{
										EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number++;
										break;
									}
									else
									{
										EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address++;
									}
								}
							}
							//EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number=INT32_Val;
							write_eeprom(TOTAL_PVS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number, 2);
						}
						else
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number=INT32_Val;
							write_eeprom(TOTAL_PVS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number, 2);
						}
					}
				}
			break;
			case 0x0354: //BMS台数
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].BMS_Total_Num.Max)
					{
						if(INT32_Val > EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number)
						{
							for(uint8_t i=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i<INT32_Val;i++)
							{
								uint8_t j=0;
								while(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number <= 247)
								{
									for(j=0;j<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;j++)
									{
										if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address ==
										   EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[j].Modubus_Address)
										{
											break;
										}
									}
									if(j == EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number)
									{
										EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number++;
										break;
									}
									else
									{
										EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address++;
									}
								}
							}
							//EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number=INT32_Val;
							write_eeprom(TOTAL_BMS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number, 2);
						}
						else
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number=INT32_Val;
							write_eeprom(TOTAL_BMS_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number, 2);
						}
					}
            	}
			break;
			case 0x0355: //电表总台数
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].AMMETER_Total_Num.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].AMMETER_Total_Num.Max)
					{
						if(INT32_Val > EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number)
						{
							for(uint8_t i=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;i<INT32_Val;i++)
							{
								uint8_t j=0;
								while(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number <= 247)
								{
									for(j=0;j<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;j++)
									{
										if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address ==
										   EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[j].Modubus_Address)
										{
											break;
										}
									}
									if(j == EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number)
									{
										EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number++;
										break;
									}
									else
									{
										EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address++;
									}
								}
							}
							//EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number=INT32_Val;
							write_eeprom(TOTAL_AMMETER_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number, 2);
						}
						else
						{
							EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number=INT32_Val;
							write_eeprom(TOTAL_AMMETER_NUM_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number, 2);
						}
					}
            	}
			break;
            case 0x0360://最大逆流功率
            	if((INT32_Val >= 0)&&(INT32_Val <= 30000))
                {
            		if(Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode >= 10)
            		{
						if(Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode == ACSIDE_PVLIMIT)
						{
							if(Time_Div_Info.Time_Div_Set_Info[0].Run_Power < INT32_Val)
							{
								Max_anti_reflux_power = INT32_Val;
								write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
							}
						}
						else
						{
							Max_anti_reflux_power = INT32_Val;
							write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
						}
            		}
            		else
            		{
            			if(System_Info.Config.Time_Divide_Function == 1)
            			{
							int i=0;
							for(i=0; i<10; i++)
							{
								if((Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode == PCC_POWER_CTRL) && (Time_Div_Info.Time_Div_Set_Info[i].Run_Power >= INT32_Val))
								{
									break;
								}
							}
							if(i == 10)
							{
								Max_anti_reflux_power= INT32_Val;
								write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
							}
            			}
            			else if(System_Info.Config.Time_Divide_Function == 2)
            			{
							int i=0;
							for(i=0; i<7; i++)
							{
								if((Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode == PCC_POWER_CTRL) && (Time_Div_Info.Time_Div_Set_Info[i].Run_Power >= INT32_Val))
								{
									break;
								}
							}
							if(i == 7)
							{
								Max_anti_reflux_power= INT32_Val;
								write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
							}
            			}
            			else
            			{
							int i=0;
							for(i=0; i<1; i++)
							{
								if((Time_Div_Info.Time_Div_Set_Info[i].Power_Ctrl_Mode == PCC_POWER_CTRL) && (Time_Div_Info.Time_Div_Set_Info[i].Run_Power >= INT32_Val))
								{
									break;
								}
							}
							if(i == 1)
							{
								Max_anti_reflux_power= INT32_Val;
								write_eeprom(Max_anti_reflux_power_ADDRESS,&Max_anti_reflux_power,2);
							}
            			}
            		}
                }
                break;
            case 0x0361://网口下发周期
            	if((INT32_Val >= 0) && (INT32_Val <= 2000))
            	{
            		EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay = INT32_Val;
            		write_eeprom(PCS_NET_SETPARA_DELAY_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay, 2);
            	}
            	break;
            case 0x0362://网口设置周期
            	if((INT32_Val >= 1) && (INT32_Val <= 50))
            	{
            		EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle = INT32_Val;
            		write_eeprom(PCS_NET_SET_CYCLE_ADDRESS, &EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle, 2);
            	}
            	break;
			case 0x036F: //电表协议选择
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val == 0 || INT32_Val == 1)
					{
						System_Info.Config.Ammeter_Protocol_Type = INT32_Val;
						write_eeprom(AMMETER_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.Ammeter_Protocol_Type, 1);
					    for(int i=0;i<6;i++)
					    {
					    	if(comParam[i].protocol == SERIAL_PROTOCOL_AMMETER)
					    	{
					    		ctrl.serialRev_thread[i].SetComParam(comParam[i]);
					    		ctrl.serialRev_thread[i].Set_Baud(comParam[i].baud);
					    	}
					    }
					}
            	}
			break;
			case 0x0370: //soc均衡使能控制
				if((INT32_Val==0xAA )||( INT32_Val==0x55))
				{
					System_Info.Config.Balancing_Enable = INT32_Val;
					write_eeprom(BALANCING_ENABLE_ADDRESS, &System_Info.Config.Balancing_Enable, 1);
					if(System_Info.Config.Balancing_Enable == ENABLE) //SOC均衡使能，同时使能SOC控制
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable  = ENABLE;
						write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable = DISABLE;
						write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
					}
				}
			break;
			case 0x0371: //PCS离网启动策略
            	//if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val== 0)||(INT32_Val==1))
					{
						System_Info.Config.PCS_Island_Ctrl_Mode=(uint8_t)INT32_Val;
						write_eeprom(PCS_ISLAND_CTRL_MODE_ADDRESS, &System_Info.Config.PCS_Island_Ctrl_Mode, 1);
					}
            	}
			break;
			case 0x0380://协议类型
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val==NEW_PROTOCOL || INT32_Val==OLD_PROTOCOL)
					{
						System_Info.Config.PCS_Protocol_Type=(uint8_t)INT32_Val;
						write_eeprom(PCS_PROTOCOL_TYPE_ADDRESS, &System_Info.Config.PCS_Protocol_Type, 1);
					}
            	}
			break;
			case 0x0381://母线类型
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val==0 || INT32_Val==1)
					{
						System_Info.Config.Bus_Type=(uint8_t)INT32_Val;
						write_eeprom(BUS_TYPE_ADDRESS, &System_Info.Config.Bus_Type, 1);
						if(System_Info.Config.Bus_Type == 1){  //交流母线
							webPageShow.pcs = 1;
							webPageShow.pv = 1;
							webPageShow.dcdc = 0;
							webPageShow.ammeter = 1;
							write_eeprom(WEB_PAGE_PCS_ST_ADDRESS, &webPageShow.pcs, 1);
							write_eeprom(WEB_PAGE_PV_ST_ADDRESS, &webPageShow.pv, 1);
							write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);
							write_eeprom(WEB_PAGE_AMMETER_ST_ADDRESS, &webPageShow.ammeter, 1);
							System_Info.Config.DCDC_Type=0;
							write_eeprom(DCDC_TYPE_ADDRESS, &System_Info.Config.DCDC_Type, 1);
						}else{
							webPageShow.pcs = 1;
							webPageShow.pv = 0;
							webPageShow.dcdc = 0;
							webPageShow.ammeter = 1;
							write_eeprom(WEB_PAGE_PCS_ST_ADDRESS, &webPageShow.pcs, 1);
							write_eeprom(WEB_PAGE_PV_ST_ADDRESS, &webPageShow.pv, 1);
							write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);
							write_eeprom(WEB_PAGE_AMMETER_ST_ADDRESS, &webPageShow.ammeter, 1);
						}
					}
            	}
			break;
			case 0x0382://是否并网
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val==0 || INT32_Val==1)
					{
						Grid_Connect_Type=(uint8_t)INT32_Val;
						write_eeprom(GRID_CONNECT_TYPE_ADDRESS, &Grid_Connect_Type, 1);
					}
            	}
			break;
			case 0x0383://直流母线充电使能
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val==0 || INT32_Val==1)
					{
						System_Info.Config.DC_Bus_Charge_Permit=(uint8_t)INT32_Val;
						write_eeprom(DC_BUS_CHARGE_PERMIT_ADDRESS, &System_Info.Config.DC_Bus_Charge_Permit, 1);
					}
            	}
			break;
			case 0x0384://系统功率设置
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if(INT32_Val<=SYS_POWER_SET_MAX)
					{
						System_Info.Config.System_Power_Set=INT32_Val;
						write_eeprom(SYS_POWER_SET_ADDRESS, &System_Info.Config.System_Power_Set, 2);
					}
            	}
			break;
			case 0x0385://储能逆变器型号
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==PCS_Power_SC30_50)||(INT32_Val==PCS_Power_SC100)||(INT32_Val==PCS_Power_SC250)
					||(INT32_Val==PCS_Power_SC500)|| (INT32_Val==PCS_Power_SC500TL)||(INT32_Val==PCS_Power_SC1000))
					{
						System_Info.Config.PCS_Type=INT32_Val;
						Com_Data.Load_Default_Flag = 0xAA;//设置完需回复出厂设置
						Com_Data.Load_Defalut_Net_Flag = 0x55;
						write_eeprom(SYS_PCS_POWER_TYPE_ADDRESS, &System_Info.Config.PCS_Type, 1);
						reboot = true;
						Sys_Ctrl.Sys_Stop = true;
					}
            	}
			break;
			case 0x0386://优先级选择
				if(INT32_Val==0 || INT32_Val==1)
				{
					System_Info.Config.EMS_Run_Priority=(uint8_t)INT32_Val;
					write_eeprom(EMS_RUN_MODE_ADDRESS, &System_Info.Config.EMS_Run_Priority, 1);
				}
			break;
			case 0x0387:  //PV通讯模式设置
            	//if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val== PV_COMM_SERIAL)||(INT32_Val==PV_COMM_LAN))
					{
						System_Info.Config.PV_Comm_Type=(uint8_t)INT32_Val;
						write_eeprom(PV_COMM_ADDRESS, & System_Info.Config.PV_Comm_Type, 1);
					}
            	}
			break;
			case 0x0388://VSG使能
	//          if(INT32_Val==1)
	//          {
	//              System_Info.VSG_func_enable=1; // 1-vsg模式
	//              //Sys_Ctrl.VSG_Mode = true; //屏下发切换时不启动
	//          }
	//          else
	//          {
	//              System_Info.VSG_func_enable=0; // 0-并网模式
	//              //Sys_Ctrl.OnGrid_Mode=true; //屏下发切换时不启动
	//          }
	//          write_eeprom(VSG_FUNC_ENABLE_ADDRESS,&System_Info.VSG_func_enable,1);
			break;
			case 0x0389://分时电价控制使能
            	//if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==0) || (INT32_Val==1) ||(INT32_Val==2))
					{
						System_Info.Config.Time_Divide_Function = (uint16_t)INT32_Val;
						if(System_Info.Config.Time_Divide_Function != 0)
						{
							Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode = CONST_POWER; //从高级功能退出
							write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode, 1);
						}
						write_eeprom(TIME_DIVIDE_FUNCTION_ADDRESS, &System_Info.Config.Time_Divide_Function, 2);
						if(INT32_Val != 0) Clear_Day_Curve_Array();
					}
            	}
			break;
			case 0x038A://BMS类型设置
            	//if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val== 0)||(INT32_Val==1)||(INT32_Val==2))
					{
						System_Info.Config.BMS_Type=(uint8_t)INT32_Val;
						write_eeprom(BMS_TYPE_ADDRESS, & System_Info.Config.BMS_Type, 1);
						if(System_Info.Config.BMS_Type == 0){
							webPageShow.bms=0;
							write_eeprom(WEB_PAGE_BMS_ST_ADDRESS, &webPageShow.bms, 1);
						}else{
							webPageShow.bms=1;
							write_eeprom(WEB_PAGE_BMS_ST_ADDRESS, &webPageShow.bms, 1);
						}
					}
            	}
			break;
			case 0x038B: //DCDC类型
            	//if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==0 )||( INT32_Val==1)||( INT32_Val==2))
					{
						System_Info.Config.DCDC_Type=INT32_Val;
						write_eeprom(DCDC_TYPE_ADDRESS, &System_Info.Config.DCDC_Type, 1);
						if(System_Info.Config.DCDC_Type == 0){
							webPageShow.dcdc = 0;
							write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);
						}else{
							webPageShow.dcdc = 1;
							write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);
						}
					}
            	}
			break;
			case 0x038C: //Meter通讯使能/禁止
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==0xAA )||( INT32_Val==0x55))
					{
						System_Info.Config.Meter_Comm_Enable=INT32_Val;
						write_eeprom(METER_COMM_ENABLE_ADDRESS, &System_Info.Config.Meter_Comm_Enable, 1);
						if(System_Info.Config.Meter_Comm_Enable == 0x55){
							webPageShow.ammeter=0;
							write_eeprom(WEB_PAGE_AMMETER_ST_ADDRESS, &webPageShow.ammeter, 1);
						}else{
							webPageShow.ammeter=1;
							write_eeprom(WEB_PAGE_AMMETER_ST_ADDRESS, &webPageShow.ammeter, 1);
						}
					}
            	}
			break;
			case 0x038D: //PCS通讯模式选择
            	//if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val== PCS_COMM_SERIAL && Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode != PEAK_LOAD_SHIFTING)||
							(INT32_Val==PCS_COMM_LAN))
					{
						System_Info.Config.PCS_Comm_Type=(uint8_t)INT32_Val;
						write_eeprom(PCS_COMM_ADDRESS, & System_Info.Config.PCS_Comm_Type, 1);
						if(INT32_Val== PCS_COMM_SERIAL){
							ctrl.serialSend_thread.setThreadPriority(SCHED_FIFO);
							ctrl.serialSend_thread.setThreadPriorityParam(99);
						}else{
							ctrl.serialSend_thread.setThreadPriority(SCHED_RR);
							ctrl.serialSend_thread.setThreadPriorityParam(7);
						}
					}
            	}
			break;
			case 0x038E: //配电功能使能/禁止
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==0xAA )||( INT32_Val==0x55))
					{
						System_Info.Config.Power_Distribution_Enable=INT32_Val;
						write_eeprom(POWER_DISTRIBUTION_ENABLE_ADDRESS, &System_Info.Config.Power_Distribution_Enable, 1);
					}
            	}
			break;
			case 0x038F: //SOC控制使能
				if((INT32_Val==0xAA )||( INT32_Val==0x55))
				{
					EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable=INT32_Val;
					write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
					if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable==ENABLE)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable=DISABLE;
						write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
					}
					else if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable==DISABLE)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable=ENABLE;
						write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
					}
				}
			break;
			case 0x0390://web是否显示pcs界面
				if(INT32_Val==0 || INT32_Val==1)
				{
					webPageShow.pcs=(uint8_t)INT32_Val;
					write_eeprom(WEB_PAGE_PCS_ST_ADDRESS, &webPageShow.pcs, 1);
				}
			break;
			case 0x0391://web是否显示pv界面
				if(INT32_Val==0 || INT32_Val==1)
				{
					webPageShow.pv=(uint8_t)INT32_Val;
					write_eeprom(WEB_PAGE_PV_ST_ADDRESS, &webPageShow.pv, 1);
				}
			break;
			case 0x0392://web是否显示dcdc界面
				if(INT32_Val==0 || INT32_Val==1)
				{
					webPageShow.dcdc=(uint8_t)INT32_Val;
					write_eeprom(WEB_PAGE_DCDC_ST_ADDRESS, &webPageShow.dcdc, 1);
				}
			break;
			case 0x0393://web是否显示pvs界面
				if(INT32_Val==0 || INT32_Val==1)
				{
					webPageShow.pvs=(uint8_t)INT32_Val;
					write_eeprom(WEB_PAGE_PVS_ST_ADDRESS, &webPageShow.pvs, 1);
				}
			break;
			case 0x0394://web是否显示bms界面
				if(INT32_Val==0 || INT32_Val==1)
				{
					webPageShow.bms=(uint8_t)INT32_Val;
					write_eeprom(WEB_PAGE_BMS_ST_ADDRESS, &webPageShow.bms, 1);
				}
			break;
			case 0x0395://web是否显示ammeter界面
				if(INT32_Val==0 || INT32_Val==1)
				{
					webPageShow.ammeter=(uint8_t)INT32_Val;
					write_eeprom(WEB_PAGE_AMMETER_ST_ADDRESS, &webPageShow.ammeter, 1);
				}
			break;
			case 0x0396://web是否显示实时监控界面
				if(INT32_Val==0 || INT32_Val==1)
				{
					webPageShow.realtimeMonitor=(uint8_t)INT32_Val;
					write_eeprom(WEB_PAGE_REALTIMEMONITOR_ST_ADDRESS, &webPageShow.realtimeMonitor, 1);
				}
			break;
			case 0x0397://web是否显示电池控制界面
				if(INT32_Val==0 || INT32_Val==1)
				{
					webPageShow.batteryctrl=(uint8_t)INT32_Val;
					write_eeprom(WEB_PAGE_BATTERYCONTROL_ST_ADDRESS, &webPageShow.batteryctrl, 1);
				}
			break;
			case 0x039A://web是否显示环境仪界面
				if(INT32_Val==0 || INT32_Val==1)
				{
					webPageShow.env=(uint8_t)INT32_Val;
					write_eeprom(WEB_PAGE_ENV_ST_ADDRESS, &webPageShow.env, 1);
				}
			break;

            case 0x03A0://KP
                if((INT32_Val<=6000)&&(INT32_Val>=1))
                {
                    wInverterAcVoltDQKp = (uint16_t)INT32_Val;
                    write_eeprom(KP_ADDRESS,&wInverterAcVoltDQKp,2);
                }
                break;
            case 0x03A1://KI
                if(INT32_Val<=6000&&(INT32_Val>=1))
                {
                    wInverterAcVoltDQKi = (uint16_t)INT32_Val;
                    write_eeprom(KI_ADDRESS,&wInverterAcVoltDQKi,2);
                }
                break;
            case 0x03A2://PV KP
                if((INT32_Val<=6000)&&(INT32_Val>=1))
                {
                    wInverterAcVoltDQKp_PVLimit = (uint16_t)INT32_Val;
                    write_eeprom(KP_PVLIMIT_ADDRESS,&wInverterAcVoltDQKp_PVLimit,2);
                }
                break;
            case 0x03A3://PV KI
                if(INT32_Val<=6000&&(INT32_Val>=1))
                {
                    wInverterAcVoltDQKi_PVLimit = (uint16_t)INT32_Val;
                    write_eeprom(KI_PVLIMIT_ADDRESS,&wInverterAcVoltDQKi_PVLimit,2);
                }
                break;
            case 0x03A4://
                if((INT32_Val<=1000)&&(INT32_Val>=1))
                {
                    Balancing_Kp = (uint16_t)INT32_Val;
                    write_eeprom(BALANCING_KP_ADDRESS,&Balancing_Kp,2);
                }
                break;
            case 0x03A5://zero v max
                if((INT32_Val<=500)&&(INT32_Val>=50))
                {
                    System_Info.Zero_V_Max = (uint16_t)INT32_Val;
                    write_eeprom(ZERO_V_MAX_ADDRESS,&System_Info.Zero_V_Max,2);
                }
                break;
            case 0x03A6://回调比例
                if((INT32_Val<=10000)&&(INT32_Val>=101))
                {
                    Parameter_recovery_kp= (uint16_t)INT32_Val;
                    write_eeprom(Parameter_recovery_kp_ADDRESS,&Parameter_recovery_kp,2);
                }
                break;
            case 0x03A7://回调延时
                if((INT32_Val<=100)&&(INT32_Val>=1))
                {
                    Parameter_recovery_dalay_time= (uint16_t)INT32_Val;
                    write_eeprom(Parameter_recovery_dalay_time_ADDRESS,&Parameter_recovery_dalay_time,2);
                }
                break;
            case 0x03B0: //光伏广播使能
                if((INT32_Val==ENABLE)||( INT32_Val==DISABLE))
                {
                    Send_Data_To_PV_Broadcast_Enable=INT32_Val;
                    write_eeprom(Send_Data_To_PV_Broadcast_Enable_ADDRESS, &Send_Data_To_PV_Broadcast_Enable, 2);
                }
            break;
            case 0x03B1: //消峰填谷使能
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==ENABLE)||( INT32_Val==DISABLE))
					{
						Sys_Start_Flag=false;
						if(INT32_Val==ENABLE)
						{
							Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode=PEAK_LOAD_SHIFTING;
							System_Info.Config.PCS_Comm_Type=PCS_COMM_LAN;
							write_eeprom(PCS_COMM_ADDRESS, & System_Info.Config.PCS_Comm_Type, 1);
						}
						else if(INT32_Val==DISABLE)
						{
							Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode=CONST_POWER;
						}
						write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode,1);
						Time_Div_Info.Time_Div_Set_Info[0].Run_Mode = GRID_MODE;
						write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Run_Mode,1);
						Time_Div_Info.Time_Div_Set_Info[0].Run_Power = 0;
						write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Run_Power,2);
						System_Info.Config.Time_Divide_Function = 0;
						write_eeprom(TIME_DIVIDE_FUNCTION_ADDRESS, &System_Info.Config.Time_Divide_Function, 2);
						if(INT32_Val==ENABLE)
						{
							Clear_Day_Curve_Array();
						}
					}
            	}
            break;
            case 0x03B2:
            	if((INT32_Val >= 0)&&(INT32_Val <= 30000))
				{
            		if(Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power < INT32_Val)
            		{
            			Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power = (int16_t)INT32_Val;
            			write_eeprom(PEAK_VALLEY_1_START_DISCHARGE_POWER_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power,2);
            		}
				}
            break;
            case 0x03B3:
            	if((INT32_Val >= 0)&&(INT32_Val <= 30000))
				{
            		if(Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power > INT32_Val)
            		{
            			Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power = (int16_t)INT32_Val;
            			write_eeprom(PEAK_VALLEY_1_START_CHARGE_POWER_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power,2);
            		}
				}
            break;
            case 0x03B5: //交流限发使能
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==ENABLE)||( INT32_Val==DISABLE))
					{
						Sys_Start_Flag=false;
						if(INT32_Val==ENABLE)
						{
							Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode=ACSIDE_PVLIMIT;
						}
						else if(INT32_Val==DISABLE)
						{
							Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode=CONST_POWER;
						}
						write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode,1);
						Time_Div_Info.Time_Div_Set_Info[0].Run_Mode = GRID_MODE;
						write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Run_Mode,1);
						Time_Div_Info.Time_Div_Set_Info[0].Run_Power = 0;
						write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Run_Power,2);
						System_Info.Config.Time_Divide_Function = 0;
						write_eeprom(TIME_DIVIDE_FUNCTION_ADDRESS, &System_Info.Config.Time_Divide_Function, 2);
						if(INT32_Val==ENABLE)
						{
							Clear_Day_Curve_Array();
						}
					}
            	}
            break;
            case 0x03B6:
            	if((INT32_Val >= 0)&&(INT32_Val <= 30000)&&(INT32_Val < Max_anti_reflux_power))
				{
					Time_Div_Info.Time_Div_Set_Info[0].Run_Power = INT32_Val;
					write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Run_Power,2);
				}
            break;
            case 0x03B7:
				if ((((int16_t)INT32_Val) >= -(int16_t)((float)System_Info.Config.System_Power_Set * 1.1) &&
					 ((int16_t)INT32_Val) <=  (int16_t)((float)System_Info.Config.System_Power_Set * 1.1)))
				{
					Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power = (int16_t)INT32_Val;
					write_eeprom(RAMP_RATE_CTRL_PCS_AUTOCHARGE_POWER, &Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power, 2);
				}
        	break;
            case 0x03B8: //输出平滑使能
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==ENABLE)||( INT32_Val==DISABLE))
					{
						Sys_Start_Flag=false;
						if(INT32_Val==ENABLE)
						{
							Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode=RAMP_RATE_CONTROL;
						}
						else if(INT32_Val==DISABLE)
						{
							Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode=CONST_POWER;
						}
						write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode,1);
						Time_Div_Info.Time_Div_Set_Info[0].Run_Mode = GRID_MODE;
						write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Run_Mode,1);
						Time_Div_Info.Time_Div_Set_Info[0].Run_Power = 0;
						write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Run_Power,2);
						System_Info.Config.Time_Divide_Function = 0;
						write_eeprom(TIME_DIVIDE_FUNCTION_ADDRESS, &System_Info.Config.Time_Divide_Function, 2);
						if(INT32_Val==ENABLE)
						{
							Clear_Day_Curve_Array();
						}
					}
            	}
            break;
            case 0x03B9: //额定光伏功率
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val >= 0)&&(INT32_Val <= 1000000))
					{
						Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power = INT32_Val;
						write_eeprom(RAMP_RATE_CTRL_PV_RATED_POWER_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power,4);
					}
            	}
            break;
            case 0x03BA: //斜率
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val > 0)&&(INT32_Val <= 100))
					{
						Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio = INT32_Val;
						write_eeprom(RAMP_RATE_CTRL_RAMP_RATIO_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio,2);
					}
            	}
            break;
            case 0x03BB: //斜率时间
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
				{
					if((INT32_Val > 0)||(INT32_Val <= 60))
					{
						Time_Div_Info.Time_Div_Set_Info[0].Ramp_time = INT32_Val;
						write_eeprom(RAMP_RATE_CTRL_RAMP_TIME_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Ramp_time,2);
					}
				}
            break;
            case 0x03BC: //SOC上限
            	if((INT32_Val>0)&&(INT32_Val<=1000)
            	  &&(INT32_Val>Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit))
            	{
            		Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit = INT32_Val;
            		write_eeprom(RAMP_RATE_CTRL_SOC_UPLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit, 2);
            	}
            break;
            case 0x03BD: //SOC下限
            	if((INT32_Val>0)&&(INT32_Val<=1000)
            	  &&(Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit>INT32_Val))
            	{
            		Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit = INT32_Val;
            		write_eeprom(RAMP_RATE_CTRL_SOC_LOWLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit, 2);
            	}
            break;
            case 0x03BE: //Vbat上限
            	if( (INT32_Val >= P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Min) &&
            			(INT32_Val <= P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Max) &&
            			  	  (INT32_Val > Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit) )
            	{
            		Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit = INT32_Val;
            		write_eeprom(RAMP_RATE_CTRL_VBAT_UPLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit, 2);
            	}
            break;
            case 0x03BF: //Vbat下限
            	if((INT32_Val >= P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Min) &&
            			(INT32_Val <= P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Max) &&
            				(Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit > INT32_Val) )
            	{
            		Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit = INT32_Val;
            		write_eeprom(RAMP_RATE_CTRL_VBAT_LOWLIMIT_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit, 2);
            	}
            break;
            case 0x03C0: //调频使能
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==ENABLE)||( INT32_Val==DISABLE))
					{
						Sys_Start_Flag=false;
						if(INT32_Val==ENABLE)
						{
							Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode=FM;
						}
						else if(INT32_Val==DISABLE)
						{
							Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode=CONST_POWER;
						}
						write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode,1);
						Time_Div_Info.Time_Div_Set_Info[0].Run_Mode = GRID_MODE;
						write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Run_Mode,1);
						Time_Div_Info.Time_Div_Set_Info[0].Run_Power = 0;
						write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS,&Time_Div_Info.Time_Div_Set_Info[0].Run_Power,2);
						System_Info.Config.Time_Divide_Function = 0;
						write_eeprom(TIME_DIVIDE_FUNCTION_ADDRESS, &System_Info.Config.Time_Divide_Function, 2);
						if(INT32_Val==ENABLE)
						{
							Clear_Day_Curve_Array();
						}
					}
            	}
            break;
            case 0x03C1:
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val == 0) || (INT32_Val == 1))
					{
						Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital = INT32_Val;
						write_eeprom(FM_ANALOG_DIGITAL_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital, 2);
					}
            	}
            break;
            case 0x03C2:
            	if((INT32_Val <= 10)&&(INT32_Val > 0) && (INT32_Val > Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin))
            	{
            		Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax = INT32_Val;
            		write_eeprom(FM_ANALOG_VMAX_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax, 2);
            	}
            break;
            case 0x03C3:
            	if((INT32_Val >= 0) &&(INT32_Val < 10)&& (INT32_Val < Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax))
            	{
            		Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin = INT32_Val;
            		write_eeprom(FM_ANALOG_VMIN_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin, 2);
            	}
            break;
            case 0x03C4:
            	if((INT32_Val >= 300) &&(INT32_Val <= 30000))
            	{
            		Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle = INT32_Val;
            		write_eeprom(PEAK_VALLEY_1_UPDATE_CYCLE_ADDRESS, &Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle, 2);
            	}
            break;
            case 0x0470:
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==ENABLE)||(INT32_Val==DISABLE))
					{
						System_Info.EMS_Meter_Com_Count[0] = METER_SIGNAL_FULL;
						Meter_Com_Err[0] = false;
						System_Info.Config.Meter_Position_Enable[0] = INT32_Val;
						write_eeprom(Meter_Position_Enable_ADDRESS0 + 0, &System_Info.Config.Meter_Position_Enable[0], 1);
					}
            	}
        	break;
            case 0x0471:
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==ENABLE)||(INT32_Val==DISABLE))
					{
						System_Info.EMS_Meter_Com_Count[1] = METER_SIGNAL_FULL;
						Meter_Com_Err[1] = false;
						System_Info.Config.Meter_Position_Enable[1] = INT32_Val;
						write_eeprom(Meter_Position_Enable_ADDRESS0 + 1, &System_Info.Config.Meter_Position_Enable[1], 1);
					}
            	}
        	break;
            case 0x0472:
            	if((!GRID_RUNNING)&&(!ISLAND_RUNNING))
            	{
					if((INT32_Val==ENABLE)||(INT32_Val==DISABLE))
					{
						System_Info.EMS_Meter_Com_Count[2] = METER_SIGNAL_FULL;
						Meter_Com_Err[2] = false;
						System_Info.Config.Meter_Position_Enable[2] = INT32_Val;
						write_eeprom(Meter_Position_Enable_ADDRESS0 + 2, &System_Info.Config.Meter_Position_Enable[2], 1);
					}
            	}
        	break;
			default:
			break;
		}
    }
    else if(id >= 490 && id < 0x500)
    {
    	uint8_t index = 0, sel = 0;
    	index = (id-0x490)/2;
    	sel = (id-0x490)%2;
    	switch(sel)
    	{
    		case 0x00:
    			if((INT32_Val==ALARM_PIN)||(INT32_Val==FAULT_PIN)||(INT32_Val==NODEF_PIN))
    			{
    				System_Info.Config.InputPin_Function[index]=INT32_Val;
    				write_eeprom(InputPin_Function_ADDRESS0 + index, &System_Info.Config.InputPin_Function[index], 1);
    			}
    		break;
    		case 0x01:
    			if((INT32_Val==NORMAL_OPEN)||(INT32_Val==NORMAL_CLOSE))
    			{
    				System_Info.Config.InputPin_NC_NO[index]=INT32_Val;
    				write_eeprom(InputPin_NC_NO_ADDRESS0 + index, &System_Info.Config.InputPin_NC_NO[index], 1);
    			}
    		break;
			default:
			break;
    	}
    }
    else if(id >= 0x500 && id < 0x5FF)
    {
    	uint8_t index = 0, sel = 0;
    	index = (id-0x500)/16;
    	sel = (id-0x500)%16;
    	switch(sel)
    	{
			case 0x03://时间段1起始时间:时
				if(INT32_Val>=0 && INT32_Val<=23)
				{
					Time_Div_Info.Time_Div_Set_Info[index].Start_time.hour = (uint8_t)INT32_Val;
					write_eeprom(TIME_DIV_START_TIME_HOUR_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Start_time.hour,1);
				}
			break;
			case 0x04://时间段1起始时间:分
				if(INT32_Val>=0 && INT32_Val<=59)
				{
					Time_Div_Info.Time_Div_Set_Info[index].Start_time.min = (uint8_t)INT32_Val;
					write_eeprom(TIME_DIV_START_TIME_MIN_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Start_time.min,1);
				}
			break;
			case 0x08://时间段1结束时间:时
				if(INT32_Val>=0 && INT32_Val<=23)
				{
					Time_Div_Info.Time_Div_Set_Info[index].End_time.hour = (uint8_t)INT32_Val;
					write_eeprom(TIME_DIV_END_TIME_HOUR_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].End_time.hour,1);
				}
			break;
			case 0x09://时间段1结束时间:分
				if(INT32_Val>=0 && INT32_Val<=59)
				{
					Time_Div_Info.Time_Div_Set_Info[index].End_time.min = (uint8_t)INT32_Val;
					write_eeprom(TIME_DIV_END_TIME_MIN_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].End_time.min,1);
				}
			break;
			case 0x0A://时间段1运行模式  0-并网 1-离网 2-并离网切换
				if((INT32_Val==GRID_MODE)|| (INT32_Val==ISLAND_MODE)|| (INT32_Val==GRID_ISLAND_MODE))
				{
					Time_Div_Info.Time_Div_Set_Info[index].Run_Mode = (uint8_t)INT32_Val;
					write_eeprom(TIME_DIV_RUN_MODE_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Run_Mode,1);
				}
			break;
			case 0x0B://时间段1控制模式
				//if((INT32_Val==0)|| (INT32_Val==1)|| (INT32_Val==2)||(INT32_Val==3))
				if(INT32_Val>=0 && INT32_Val<=13)
				{
					if(INT32_Val == PCC_POWER_CTRL)
					{
						if(Time_Div_Info.Time_Div_Set_Info[index].Run_Power < Max_anti_reflux_power)
						{
							Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode= (uint8_t)INT32_Val;
							if(Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode>=10)
							{
								System_Info.Config.Time_Divide_Function = 0;  //高级功能关闭分时电价
							}
							write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode,1);
						}
					}
					else
					{
						Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode= (uint8_t)INT32_Val;
						if(Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode>=10)
						{
							System_Info.Config.Time_Divide_Function = 0;  //高级功能关闭分时电价
						}
						write_eeprom(TIME_DIV_POWER_CTRL_MODE_0_ADDRESS+index*7,&Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode,1);
					}
				}
			break;
			case 0x0C://时间段1运行功率
				if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
				{
					if(Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode == PCC_POWER_CTRL) {
						if(INT32_Val < Max_anti_reflux_power){
							Time_Div_Info.Time_Div_Set_Info[index].Run_Power = INT32_Val;
							write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+index*2,&Time_Div_Info.Time_Div_Set_Info[index].Run_Power,2);
						}
					}else{
						Time_Div_Info.Time_Div_Set_Info[index].Run_Power = INT32_Val;
						write_eeprom(TIME_DIV_RUN_P_SET_0_ADDRESS+index*2,&Time_Div_Info.Time_Div_Set_Info[index].Run_Power,2);
					}
				}
			break;
			case 0x0D:
				if((((int16_t)INT32_Val)>=-(int16_t)((float)System_Info.Config.System_Power_Set*1.1)) && (((int16_t)INT32_Val)<=(int16_t)((float)System_Info.Config.System_Power_Set*1.1)))
				{
					Time_Div_Info.Time_Div_Set_Info[index].Mgrid_Run_Power = INT32_Val;
					write_eeprom(TIME_DIV_MGRID_RUN_P_SET_0_ADDRESS+index*2, &Time_Div_Info.Time_Div_Set_Info[index].Mgrid_Run_Power, 2);
				}
			break;
			default:
			break;
    	}
    }
    else if(id >= 0x600 && id < 0x63F)
    {
    	switch(id)
    	{
			case 0x0600: //SOC控制使能
				if((INT32_Val==0xAA )||( INT32_Val==0x55))
				{
					EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable=INT32_Val;
					write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
					if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable==ENABLE)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable=DISABLE;
						write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
					}
					else if(EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable==DISABLE)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable=ENABLE;
						write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
					}
				}
			break;
			case 0x0601: //SOC上限2级
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Max)
				{
					if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2=INT32_Val;
						write_eeprom(SOC_UPLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2, 2);
					}
				}
			break;
			case 0x0602: //SOC上限1级
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Max)
				{
					if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1 &&
						INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1=INT32_Val;
						write_eeprom(SOC_UPLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1, 2);
					}
				}
			break;
			case 0x0603: //SOC下限1级
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Max)
				{
					if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2 &&
						INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1 &&
						INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1=INT32_Val;
						write_eeprom(SOC_LOWLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1, 2);
					}
				}
			break;
			case 0x0604: //SOC下限2级
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Max)
				{
					if(INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1 &&
						INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2=INT32_Val;
						write_eeprom(SOC_LOWLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2, 2);
					}
				}
			break;
			case 0x0605: //SOC上限2级恢复
			break;
			case 0x0606: //SOC上限1级恢复
			break;
			case 0x0607: //SOC下限1级恢复
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Max)
				{
					if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover=INT32_Val;
						write_eeprom(SOC_LOWLIMIT1_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover, 2);
					}
				}
			break;
			case 0x0608: //SOC下限2级恢复
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Max)
				{
					if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover=INT32_Val;
						write_eeprom(SOC_LOWLIMIT2_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover, 2);
					}
				}
			break;
			case 0x0609: //SOC滞环控制
				if(INT32_Val<=100)
				{
					EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis = INT32_Val;
					write_eeprom(SOC_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis, 2);
				}
			break;
			case 0x060A: //SOC控制延迟时间
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Max)
				{
					EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay=INT32_Val;
					write_eeprom(SOC_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay, 2);
				}
			break;
			case 0x060B: //SOC恢复延迟时间
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Max)
				{
					EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay=INT32_Val;
					write_eeprom(SOC_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay, 2);
				}
			break;
			case 0x0610: //电池电压控制使能
				if((INT32_Val==0xAA) || (INT32_Val==0x55))
				{
					EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable=INT32_Val;
					write_eeprom(VBAT_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable, 2);
					if(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable==ENABLE)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable=DISABLE;
						write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
					}
					else if(EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable==DISABLE)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable=ENABLE;
						write_eeprom(SOC_CONTROL_ENABLE_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable, 2);
					}
				}
			break;
			case 0x0611: //电池电压上限2级
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Max)
				{
					if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2=INT32_Val;
						write_eeprom(VBAT_UPLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2, 2);
					}
				}
			break;
			case 0x0612: //电池电压上限1级
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Max)
				{
					if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 &&
						INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1=INT32_Val;
						write_eeprom(VBAT_UPLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1, 2);
					}
				}
			break;
			case 0x0613: //电池电压下限1级
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Max)
				{
					if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2 &&
						INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1 &&
						INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1=INT32_Val;
						write_eeprom(VBAT_LOWLIMIT1_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1, 2);
					}
				}
			break;
			case 0x0614: //电池电压下限2级
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Max)
				{
					if(INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 &&
					   INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2=INT32_Val;
						write_eeprom(VBAT_LOWLIMIT2_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2, 2);
					}
				}
			break;
			case 0x0615: //电池电压上限2级恢复
			break;
			case 0x0616: //电池电压上限1级恢复
			break;
			case 0x0617: //电池电压下限1级恢复
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Max)
				{
					if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1 &&
						INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover=INT32_Val;
						write_eeprom(VBAT_LOWLIMIT1_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover, 2);
					}
				}
			break;
			case 0x0618: //电池电压下限2级恢复
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Max)
				{
					if(INT32_Val>EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2 &&
						INT32_Val<EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover)
					{
						EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover=INT32_Val;
						write_eeprom(VBAT_LOWLIMIT2_RECOVER_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover, 2);
					}
				}
			break;

			case 0x0619: //VBat滞环控制
				if(INT32_Val>=0 && INT32_Val<=500)
				{
					EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis= INT32_Val;
					write_eeprom(VBAT_FLOAT_HYSTERESIS_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis, 2);
				}
			break;

			case 0x061A: //电池电压控制延迟时间
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Max)
				{
					EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay=INT32_Val;
					write_eeprom(VBAT_CONTROL_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay, 2);
				}
			break;
			case 0x061B: //电池电压恢复延迟时间
				if(INT32_Val>=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Min && INT32_Val<=P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Max)
				{
					EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay=INT32_Val;
					write_eeprom(VBAT_CONTROL_RECOVER_DELAY_ADDRESS, &EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay, 2);
				}
			break;
			default:
			break;
    	}
    }
    else if(id >= 0x640 && id <= 0x67B)
    {
    	uint8_t index = 0;
    	index = id-0x640;
		if(INT32_Val>=1 && INT32_Val<=247)
		{
			//for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
			//{
				//if(i!=index)
				//{
					//if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[i].Modubus_Address==INT32_Val)
					//{
					//	Address_used=1;
					//}
				//}
			//}
			//if(Address_used==0)
			//{
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[index].Modubus_Address=INT32_Val;
				write_eeprom(PCS0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[index].Modubus_Address, 1);
			//}
		}
		if(PCS_Svr_Param_Change[index]==true)
		{
			PCS_Svr_Param_Change[index]=false;
			char ipStr[16]={0};
			sprintf(ipStr,"%d.%d.%d.%d", System_Info.Server[index].ip[0],System_Info.Server[index].ip[1],
					System_Info.Server[index].ip[2],System_Info.Server[index].ip[3]);
			ctrl.Client[index].SetServer(ipStr, System_Info.Server_Modbus_Port[index]);
		}
    }
    else if(id >= 0x6B0 && id <= 0x6EB)
    {
    	uint8_t index = 0;
    	index = id-0x6B0;
		if(INT32_Val>=1 && INT32_Val<=247)
		{
			//for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
			//{
				//if(i!=index)
				//{
					//if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[i].Modubus_Address==INT32_Val)
					//{
					//	Address_used=1;
					//}
				//}
			//}
			//if(Address_used==0)
			//{
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[index].Modubus_Address=INT32_Val;
				write_eeprom(PV0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[index].Modubus_Address, 1);
			//}
		}
		if(PV_Svr_Param_Change[index]==true)
		{
			PV_Svr_Param_Change[index]=false;
			char ipStr[16]={0};
			sprintf(ipStr,"%d.%d.%d.%d", System_Info.PVServer[index].ip[0],System_Info.PVServer[index].ip[1],
					System_Info.PVServer[index].ip[2],System_Info.PVServer[index].ip[3]);
			ctrl.PvClient[index].SetServer(ipStr, System_Info.PVServer_Modbus_Port[index]);
		}
    }
    else if(id >= 0x6F0 && id <= 0x70E)
    {
    	uint8_t index = 0;
    	index = id-0x6F0;
		if(INT32_Val>=1 && INT32_Val<=247)
		{
			//for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;i++)
			//{
				//if(i!=index)
				//{
					//if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[i].Modubus_Address==INT32_Val)
					//{
					//	Address_used=1;
					//}
				//}
			//}
			//if(Address_used==0)
			//{
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[index].Modubus_Address=INT32_Val;
				write_eeprom(DCDC0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[index].Modubus_Address, 1);
			//}
		}
    }
    else if(id >= 0x0760 && id <= 0x0791)
    {
    	uint8_t index = 0;
    	index = id-0x0760;
		if(INT32_Val>=1 && INT32_Val<=247)
		{
			//for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number;i++)
			//{
				//if(i!=index)
				//{
					//if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[i].Modubus_Address==INT32_Val)
					//{
					//	Address_used=1;
					//}
				//}
			//}
			//if(Address_used==0)
			//{
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[index].Modubus_Address=INT32_Val;
				write_eeprom(PVS0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[index].Modubus_Address, 1);
			//}
		}
    }
    else if(id >= 0x840 && id <= 0x87B)
    {
    	uint8_t index = 0;
    	index = id-0x840;
		if(INT32_Val>=1 && INT32_Val<=247)
		{
			//for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;i++)
			//{
				//if(i!=index)
				//{
					//if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[i].Modubus_Address==INT32_Val)
					//{
					//	Address_used=1;
					//}
				//}
			//}
			//if(Address_used==0)
			//{
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[index].Modubus_Address=INT32_Val;
				write_eeprom(BMS0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[index].Modubus_Address, 1);
			//}
		}
    }
    else if(id >= 0x750 && id <= 0x75B)
    {
    	uint8_t index = 0;
    	index = id-0x750;
		if(INT32_Val>=1 && INT32_Val<=247)
		{
			//for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;i++)
			//{
				//if(i!=index)
				//{
					//if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[i].Modubus_Address==INT32_Val)
					//{
					//	Address_used=1;
					//}
				//}
			//}
			//if(Address_used==0)
			//{
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[index].Modubus_Address=INT32_Val;
				write_eeprom(AMMETER0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[index].Modubus_Address, 1);
			//}
		}
    }
    else if(id == 0x900 && id < (0x900+ENV_NUM))
    {
    	uint8_t index = 0;
    	index = id-0x900;
		if(INT32_Val>=1 && INT32_Val<=247)
		{
			//for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_ENV_Number;i++)
			//{
				//if(i!=index)
				//{
					//if(EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[i].Modubus_Address==INT32_Val)
					//{
					//	Address_used=1;
					//}
				//}
			//}
			//if(Address_used==0)
			//{
				EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[index].Modubus_Address=INT32_Val;
				write_eeprom(ENV0_ADDRESS+index, &EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[index].Modubus_Address, 1);
			//}
		}
    }
    else if(id >= 0x1000 && id <= 0x112B)  //目标服务器参数
    {
    	uint8_t index = 0, sel = 0;
    	index = (id-0x1000)/5;
    	sel = (id-0x1000)%5;
    	switch(sel)
    	{
			case 0x00: //IP地址0
				if(INT32_Val>=0 && INT32_Val<=255)
				{
					PCS_Svr_Param_Change[index]=true;
					if(System_Info.Server[index].ip[0]!=INT32_Val)
					{
						System_Info.Server[index].ip[0]=INT32_Val;
						write_eeprom(SERVER_0_IP_0_ADDRESS+index*4, &System_Info.Server[index].ip[0], 1);
					}
				}
			break;
			case 0x01: //IP地址1
				if(INT32_Val>=0 && INT32_Val<=255)
				{
					PCS_Svr_Param_Change[index]=true;
					if(System_Info.Server[index].ip[1]!=INT32_Val)
					{
						System_Info.Server[index].ip[1]=INT32_Val;
						write_eeprom(SERVER_0_IP_1_ADDRESS+index*4, &System_Info.Server[index].ip[1], 1);
					}
				}
			break;
			case 0x02: //IP地址2
				if(INT32_Val>=0 && INT32_Val<=255)
				{
					PCS_Svr_Param_Change[index]=true;
					if(System_Info.Server[index].ip[2]!=INT32_Val)
					{
						System_Info.Server[index].ip[2]=INT32_Val;
						write_eeprom(SERVER_0_IP_2_ADDRESS+index*4, &System_Info.Server[index].ip[2], 1);
					}
				}
			break;
			case 0x03: //IP地址3
				if(INT32_Val>=0 && INT32_Val<=255)
				{
					PCS_Svr_Param_Change[index]=true;
					if(System_Info.Server[index].ip[3]!=INT32_Val)
					{
						System_Info.Server[index].ip[3]=INT32_Val;
						write_eeprom(SERVER_0_IP_3_ADDRESS+index*4, &System_Info.Server[index].ip[3], 1);
					}
				}
			break;
			case 0x04: //端口号
				if((1<=INT32_Val)&&(INT32_Val<= UINT16_MAX))
				{
					PCS_Svr_Param_Change[index]=true;
					if(System_Info.Server_Modbus_Port[index]!=INT32_Val)
					{
						System_Info.Server_Modbus_Port[index]=INT32_Val;
						write_eeprom(SERVER_0_PORT_ADDRESS+index*2, &System_Info.Server_Modbus_Port[index], 2);
					}
				}
			break;
			default:
			break;
    	}
    }
    else if(id >= 0x2000 && id <= 0x2017)
    {
    	switch(id)
    	{
			case 0x2000: //串口1波特率
				if((INT32_Val == 4800 || INT32_Val == 9600 || INT32_Val == 19200 || INT32_Val == 38400) && (comParam[0].baud!=INT32_Val))
				{
					Serial_Param_Change[0]=true;
					comParam[0].baud = INT32_Val;
					write_eeprom(COM1_PARAM_BAUD_ADDRESS, &comParam[0].baud, 2);
				}
			break;
			case 0x2001: //串口1设备类型
				if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[0].protocol!=(SERIAL_PROTOCOL_TYPE)INT32_Val)
				{
					Serial_Param_Change[0]=true;
					comParam[0].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
					comParam[0].vmin = 255;
					write_eeprom(COM1_PARAM_BAUD_ADDRESS+2, &comParam[0].vmin, 2);
					write_eeprom(COM1_PARAM_BAUD_ADDRESS+6, &comParam[0].protocol, 1);
				}
			break;
			case 0x2002: //串口1起始ID
				//if(INT32_Val>=1 && INT32_Val <= comParam[0].eId && comParam[0].sId!=INT32_Val)
				if(INT32_Val>=1 && INT32_Val<=247 && comParam[0].sId!=INT32_Val)
				{
					Serial_Param_Change[0]=true;
					comParam[0].sId = INT32_Val;
					Set_Dev_ID(comParam[0].protocol, comParam[0].sId);
					write_eeprom(COM1_PARAM_BAUD_ADDRESS+4, &comParam[0].sId, 1);
				}
			break;
			case 0x2003: //串口1结束ID
				//if(INT32_Val>=1 && INT32_Val >= comParam[0].sId && comParam[0].eId!=INT32_Val)
				if(INT32_Val>=1 && INT32_Val<=247 && comParam[0].eId!=INT32_Val)
				{
					Serial_Param_Change[0]=true;
					comParam[0].eId = INT32_Val;
					write_eeprom(COM1_PARAM_BAUD_ADDRESS+5, &comParam[0].eId, 1);
				}
			break;
			case 0x2004: //串口2波特率
				if((INT32_Val == 4800 || INT32_Val == 9600 || INT32_Val == 19200 || INT32_Val == 38400) && (comParam[1].baud!=INT32_Val))
				{
					Serial_Param_Change[1]=true;
					comParam[1].baud = INT32_Val;
					write_eeprom(COM2_PARAM_BAUD_ADDRESS, &comParam[1].baud, 2);
				}
			break;
			case 0x2005: //串口2设备类型
				if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[1].protocol != (SERIAL_PROTOCOL_TYPE)INT32_Val)
				{
					Serial_Param_Change[1]=true;
					comParam[1].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
					comParam[1].vmin = 255;
					write_eeprom(COM2_PARAM_BAUD_ADDRESS+2, &comParam[1].vmin, 2);
					write_eeprom(COM2_PARAM_BAUD_ADDRESS+6, &comParam[1].protocol, 1);
				}
			break;
			case 0x2006: //串口2起始ID
				//if(INT32_Val>=1 && INT32_Val <= comParam[1].eId && comParam[1].sId!=INT32_Val)
				if(INT32_Val>=1 && INT32_Val<=247 && comParam[1].sId!=INT32_Val)
				{
					Serial_Param_Change[1]=true;
					comParam[1].sId = INT32_Val;
					Set_Dev_ID(comParam[1].protocol, comParam[1].sId);
					write_eeprom(COM2_PARAM_BAUD_ADDRESS+4, &comParam[1].sId, 1);
				}
			break;
			case 0x2007: //串口2结束ID
				//if(INT32_Val>=1 && INT32_Val >= comParam[1].sId && comParam[1].eId!=INT32_Val)
				if(INT32_Val>=1 && INT32_Val<=247 && comParam[1].eId!=INT32_Val)
				{
					Serial_Param_Change[1]=true;
					comParam[1].eId = INT32_Val;
					write_eeprom(COM2_PARAM_BAUD_ADDRESS+5, &comParam[1].eId, 1);
				}
			break;
			case 0x2008: //串口3波特率
				if((INT32_Val == 4800 || INT32_Val == 9600 || INT32_Val == 19200 || INT32_Val == 38400) && (comParam[2].baud!=INT32_Val))
				{
					Serial_Param_Change[2]=true;
					comParam[2].baud = INT32_Val;
					write_eeprom(COM3_PARAM_BAUD_ADDRESS, &comParam[2].baud, 2);
				}
			break;
			case 0x2009: //串口3设备类型
				if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[2].protocol!=(SERIAL_PROTOCOL_TYPE)INT32_Val)
				{
					Serial_Param_Change[2]=true;
					comParam[2].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
					comParam[2].vmin = 255;
					write_eeprom(COM3_PARAM_BAUD_ADDRESS+2, &comParam[2].vmin, 2);
					write_eeprom(COM3_PARAM_BAUD_ADDRESS+6, &comParam[2].protocol, 1);
				}
			break;
			case 0x200A: //串口3起始ID
				//if(INT32_Val>=1 && INT32_Val <= comParam[2].eId && comParam[2].sId!=INT32_Val)
				if(INT32_Val>=1 && INT32_Val<=247 && comParam[2].sId!=INT32_Val)
				{
					Serial_Param_Change[2]=true;
					comParam[2].sId = INT32_Val;
					Set_Dev_ID(comParam[2].protocol, comParam[2].sId);
					write_eeprom(COM3_PARAM_BAUD_ADDRESS+4, &comParam[2].sId, 1);
				}
			break;
			case 0x200B: //串口3结束ID
				//if(INT32_Val>=1 && INT32_Val >= comParam[2].sId && comParam[2].eId!=INT32_Val)
				if(INT32_Val>=1 && INT32_Val<=247 && comParam[2].eId!=INT32_Val)
				{
					Serial_Param_Change[2]=true;
					comParam[2].eId = INT32_Val;
					write_eeprom(COM3_PARAM_BAUD_ADDRESS+5, &comParam[2].eId, 1);
				}
			break;
			case 0x200C: //串口4波特率
				if((INT32_Val == 4800 || INT32_Val == 9600 || INT32_Val == 19200 || INT32_Val == 38400) && (comParam[3].baud!=INT32_Val))
				{
					Serial_Param_Change[3]=true;
					comParam[3].baud = INT32_Val;
					write_eeprom(COM4_PARAM_BAUD_ADDRESS, &comParam[3].baud, 2);
				}
			break;
			case 0x200D: //串口4设备类型
				if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[3].protocol!=(SERIAL_PROTOCOL_TYPE)INT32_Val)
				{
					Serial_Param_Change[3]=true;
					comParam[3].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
					comParam[3].vmin = 255;
					write_eeprom(COM4_PARAM_BAUD_ADDRESS+2, &comParam[3].vmin, 2);
					write_eeprom(COM4_PARAM_BAUD_ADDRESS+6, &comParam[3].protocol, 1);
				}
			break;
			case 0x200E: //串口4起始ID
				//if(INT32_Val>=1 && INT32_Val <= comParam[3].eId && comParam[3].sId!=INT32_Val)
				if(INT32_Val>=1 && INT32_Val<=247 && comParam[3].sId!=INT32_Val)
				{
					Serial_Param_Change[3]=true;
					comParam[3].sId = INT32_Val;
					Set_Dev_ID(comParam[3].protocol, comParam[3].sId);
					write_eeprom(COM4_PARAM_BAUD_ADDRESS+4, &comParam[3].sId, 1);
				}
			break;
			case 0x200F: //串口4结束ID
				//if(INT32_Val>=1 && INT32_Val >= comParam[3].sId && comParam[3].eId!=INT32_Val)
				if(INT32_Val>=1 && INT32_Val<=247 && comParam[3].eId!=INT32_Val)
				{
					Serial_Param_Change[3]=true;
					comParam[3].eId = INT32_Val;
					write_eeprom(COM4_PARAM_BAUD_ADDRESS+5, &comParam[3].eId, 1);
				}
			break;
			case 0x2010: //串口5波特率
				if((INT32_Val == 4800 || INT32_Val == 9600 || INT32_Val == 19200 || INT32_Val == 38400) && (comParam[4].baud!=INT32_Val))
				{
					Serial_Param_Change[4]=true;
					comParam[4].baud = INT32_Val;
					write_eeprom(COM5_PARAM_BAUD_ADDRESS, &comParam[4].baud, 2);
				}
			break;
			case 0x2011: //串口5设备类型
				if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[4].protocol !=(SERIAL_PROTOCOL_TYPE)INT32_Val)
				{
					Serial_Param_Change[4]=true;
					comParam[4].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
					comParam[4].vmin = 255;
					write_eeprom(COM5_PARAM_BAUD_ADDRESS+2, &comParam[4].vmin, 2);
					write_eeprom(COM5_PARAM_BAUD_ADDRESS+6, &comParam[4].protocol, 1);
				}
			break;
			case 0x2012: //串口5起始ID
				//if(INT32_Val>=1 && INT32_Val <= comParam[4].eId && comParam[4].sId!=INT32_Val)
				if(INT32_Val>=1 && INT32_Val<=247 && comParam[4].sId!=INT32_Val)
				{
					Serial_Param_Change[4]=true;
					comParam[4].sId = INT32_Val;
					Set_Dev_ID(comParam[4].protocol, comParam[4].sId);
					write_eeprom(COM5_PARAM_BAUD_ADDRESS+4, &comParam[4].sId, 1);
				}
			break;
			case 0x2013: //串口5结束ID
				//if(INT32_Val>=1 && INT32_Val >= comParam[4].sId && comParam[4].eId!=INT32_Val)
				if(INT32_Val>=1 && INT32_Val<=247 && comParam[4].eId!=INT32_Val)
				{
					Serial_Param_Change[4]=true;
					comParam[4].eId = INT32_Val;
					write_eeprom(COM5_PARAM_BAUD_ADDRESS+5, &comParam[4].eId, 1);
				}
			break;
			case 0x2014: //串口6波特率
				if((INT32_Val == 4800 || INT32_Val == 9600 || INT32_Val == 19200 || INT32_Val == 38400) && (comParam[5].baud!=INT32_Val))
				{
					Serial_Param_Change[5]=true;
					comParam[5].baud = INT32_Val;
					write_eeprom(COM6_PARAM_BAUD_ADDRESS, &comParam[5].baud, 2);
				}
			break;
			case 0x2015: //串口6设备类型
				if(INT32_Val>=0 && INT32_Val <= SERIAL_PROTOCOL_MODBUS_NONE && comParam[5].protocol!=(SERIAL_PROTOCOL_TYPE)INT32_Val)
				{
					Serial_Param_Change[5]=true;
					comParam[5].protocol = (SERIAL_PROTOCOL_TYPE)INT32_Val;
					if(comParam[5].protocol == SERIAL_PROTOCOL_MODBUS_TOUCH) comParam[5].vmin = 8;
					else if(comParam[5].protocol == SERIAL_PROTOCOL_MODBUS_MONITOR) comParam[5].vmin = 255;
					write_eeprom(COM6_PARAM_BAUD_ADDRESS+2, &comParam[5].vmin, 2);
					write_eeprom(COM6_PARAM_BAUD_ADDRESS+6, &comParam[5].protocol, 1);
				}
			break;
			case 0x2016: //串口6起始ID
				//if(INT32_Val <= comParam[5].eId)
				if(INT32_Val >=1 && INT32_Val<=247 && comParam[5].sId!=INT32_Val)
				{
					Serial_Param_Change[5]=true;
					comParam[5].sId = INT32_Val;
					write_eeprom(COM6_PARAM_BAUD_ADDRESS+4, &comParam[5].sId, 1);
				}
			break;
			case 0x2017: //串口6结束ID
				/*if(INT32_Val>=1 && INT32_Val >= comParam[5].sId)
				{
					comParam[5].eId = INT32_Val;
					write_eeprom(COM6_PARAM_BAUD_ADDRESS+5, &comParam[5].eId, 1);
				}*/
			break;
			default:
			break;
    	}
    }
    else if(id >= 0x3000 && id <= 0x312B)  //目标服务器参数
	{
		uint8_t index = 0, sel = 0;
		index = (id-0x3000)/5;
		sel = (id-0x3000)%5;
		switch(sel)
		{
			case 0x00: //IP地址0
				if(INT32_Val>=0 && INT32_Val<=255)
				{
					PV_Svr_Param_Change[index]=true;
					if(System_Info.PVServer[index].ip[0]!=INT32_Val)
					{
						System_Info.PVServer[index].ip[0]=INT32_Val;
						write_eeprom(SERVER_PV_0_IP_0_ADDRESS+index*4, &System_Info.PVServer[index].ip[0], 1);
					}
				}
			break;
			case 0x01: //IP地址1
				if(INT32_Val>=0 && INT32_Val<=255)
				{
					PV_Svr_Param_Change[index]=true;
					if(System_Info.PVServer[index].ip[1]!=INT32_Val)
					{
						System_Info.PVServer[index].ip[1]=INT32_Val;
						write_eeprom(SERVER_PV_0_IP_1_ADDRESS+index*4, &System_Info.PVServer[index].ip[1], 1);
					}
				}
			break;
			case 0x02: //IP地址2
				if(INT32_Val>=0 && INT32_Val<=255)
				{
					PV_Svr_Param_Change[index]=true;
					if(System_Info.PVServer[index].ip[2]!=INT32_Val)
					{
						System_Info.PVServer[index].ip[2]=INT32_Val;
						write_eeprom(SERVER_PV_0_IP_2_ADDRESS+index*4, &System_Info.PVServer[index].ip[2], 1);
					}
				}
			break;
			case 0x03: //IP地址3
				if(INT32_Val>=0 && INT32_Val<=255)
				{
					PV_Svr_Param_Change[index]=true;
					if(System_Info.PVServer[index].ip[3]!=INT32_Val)
					{
						System_Info.PVServer[index].ip[3]=INT32_Val;
						write_eeprom(SERVER_PV_0_IP_3_ADDRESS+index*4, &System_Info.PVServer[index].ip[3], 1);
					}
				}
			break;
			case 0x04: //端口号
				if((1<=INT32_Val)&&(INT32_Val<= UINT16_MAX))
				{
					PV_Svr_Param_Change[index]=true;
					if(System_Info.PVServer_Modbus_Port[index]!=INT32_Val)
					{
						System_Info.PVServer_Modbus_Port[index]=INT32_Val;
						write_eeprom(SERVER_PV_0_PORT_ADDRESS+index*2, &System_Info.PVServer_Modbus_Port[index], 2);
					}
				}
			break;
			default:
			break;
		}
	}

    return ~0;
}
uint32_t b[10];
uint32_t get_param_by_id(uint32_t addr, uint8_t id)
{
    uint32_t value = 0;

    if(addr > 0 && addr < 0x0020)
    {
        switch(addr)
        {
            case 0x0002:value = 0;break;
            case 0x0003:value = 0;break;//System_Power_Set*10;break;
            //case 0x0004:value = (uint32_t)DIN(GRID_CALL);break;
            case 0x0004:value = (uint32_t)(Grid_Exist?1:0);break;
            case 0x0005:value = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power;break;
            case 0x0006:value = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Reactive_Power;break;
            case 0x0007:value = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Powerfactor;break;
            case 0x0008:value = EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power;break;
            case 0x0009:value = EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Reactive_Power;break;
            case 0x000A:value = EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Powerfactor;break;
            case 0x000B:value = EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power;break;
            case 0x000C:value = EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Reactive_Power;break;
            case 0x000D:value = EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Powerfactor;break;
            case 0x000E:value = EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Charge_Energy;break;
            case 0x000F:value = EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PCS_Total_Discharge_Energy;break;
            case 0x0010:value = EMS_Info.EMS3x.EMS_Controller_Info.RMeasure.PV_Total_Active_Energy;break;
            case 0x0011:value = System_Run_Info.Total_Load_Energy;break;
            case 0x0012:value = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Online_Num;break;
            case 0x0013:value = EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Online_Num;break;
            case 0x0014:value = 0;break;
            case 0x0015:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;break;
            case 0x0016:value = (EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage<200)?0:EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Voltage;break;
            case 0x0017:value = EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency;break;
            case 0x0018:value = (uint32_t)DIN(Grid_Contacter);break;
            case 0x0019:value = (uint32_t)DIN(MGrid_Contacter);break;
            case 0x001F:value = Cur_PCS_Num;break;
        }
    }
    else if(addr >= 0x0020 && addr <= 0x0031)       //储能逆变器信息
    {
        switch(addr)
        {
            case 0x0020:value =PCS_Info[id].PCS3x.Rmeasure.DC_Voltage;break;//当前显示储能变流器直流电压
            case 0x0021:value =PCS_Info[id].PCS3x.Rmeasure.DC_Current;break;//当前显示储能变流器直流电流
            case 0x0022:value =(uint32_t)PCS_Info[id].PCS3x.Rmeasure.DC_Power;break;//当前显示储能变流器直流功率
            case 0x0023:value =PCS_Info[id].PCS3x.Rmeasure.VAB;break;//当前显示储能变流器VAB
            case 0x0024:value =PCS_Info[id].PCS3x.Rmeasure.VBC;break;//当前显示储能变流器VBC
            case 0x0025:value =PCS_Info[id].PCS3x.Rmeasure.VCA;break;//当前显示储能变流器VCA
            case 0x0026:value =PCS_Info[id].PCS3x.Rmeasure.Grid_Frequency;break;//当前显示储能变流器电网频率
            case 0x0027:value =PCS_Info[id].PCS3x.Rmeasure.IA;break;//当前显示储能变流器IA
            case 0x0028:value =PCS_Info[id].PCS3x.Rmeasure.IB;break;//当前显示储能变流器IB
            case 0x0029:value =PCS_Info[id].PCS3x.Rmeasure.IC;break;//当前显示储能变流器IC
            case 0x002A:value =PCS_Info[id].PCS3x.Rmeasure.Active_Power;break;//当前显示储能变流器有功功率
            case 0x002B:value =PCS_Info[id].PCS3x.Rmeasure.Reactive_Power;break;//当前显示储能变流器无功功率
            case 0x002C:value =PCS_Info[id].PCS3x.Rmeasure.Power_Factor;break;//当前显示储能变流器功率因数
            case 0x002D:value = PCS_Info[id].PCS3x.Rmeasure.Total_Charge_Energe;break;//当前显示储能变流器累计充电
            case 0x002E:value = PCS_Info[id].PCS3x.Rmeasure.Total_Discharge_Energe;break;//当前显示储能变流器累计放电
            case 0x002F:value = Run_Status(PCS_Info[id].PCS3x.RCommunication.Run_Status);break;//当前显示储能变流器工作状态
            case 0x0030:value = PCS_Info[id].PCS3x.RCommunication.Fault_Status1;break;//当前显示储能变流器故障状态1
            case 0x0031:value = PCS_Info[id].PCS3x.RCommunication.Fault_Status2;break;//当前显示储能变流器故障状态2
            default:value = 0; break;
        }
    }
    else if(addr == 0x0040)
    {
        value = Cur_PVS_Num;
    }
    else if(addr > 0x0040 && addr <= 0x0047)        //光伏汇流箱信息
    {
        switch(addr)
        {
            case 0x0041:value = PVS_Info[id].DC_Vol;break;
            case 0x0042:value = PVS_Info[id].Total_I;break;
            case 0x0043:value = PVS_Info[id].Total_P_DC;break;
            case 0x0044:value = PVS_Info[id].Output_Pt_Day;break;
            case 0x0045:value = PVS_Info[id].Output_Pt_Total;break;
            case 0x0046:value = PVS_Info[id].Room_T;break;
            case 0x0047:value = PVS_Info[id].Work_Status;break;
            default:value = 0; break;
        }
    }
    else if(addr == 0x0050)
    {
        value = Cur_BMS_Num;
    }
    else if(addr > 0x0050 && addr <= 0x006D)        //BMS信息
    {
        switch(addr)
        {
            case 0x0051:value = BMS_Info[id].Sys_Voltage;break;//
            case 0x0052:value = BMS_Info[id].Sys_Current;break;//
            case 0x0053:value = BMS_Info[id].Sys_SOC;break;//
            case 0x0054:value = BMS_Info[id].Sys_SOH;break;//
            case 0x0055:value = BMS_Info[id].Single_V_Min;break;//
            case 0x0056:value = BMS_Info[id].Single_V_Max;break;//
            case 0x0057:value = BMS_Info[id].Single_T_Min;break;//
            case 0x0058:value = BMS_Info[id].Single_T_Max;break;//
            case 0x0059:value = BMS_Info[id].Charge_Limit;break;//
            case 0x005A:value = BMS_Info[id].Discharge_Limit;break;//
            case 0x005B:value = (uint16_t)BMS_Info[id].Num_Of_Rack_In_Service;break;//
            case 0x005C:value = (uint16_t)BMS_Info[id].Num_Of_Rack_In_System;break;//
            case 0x005D:value = Run_Status_BMS(BMS_Info[id].Sys_Mode);break;//68ah/94ah
            case 0x005E:value = BMS_Info[id].Alarm1;break;//94ah
            case 0x005F:value = BMS_Info[id].Alarm2;;break;//94ah
            case 0x0060:value = BMS_Info[id].Alarm3;;break;//94ah
            case 0x0061:value = BMS_Info[id].Alarm4;;break;//94ah
            case 0x0062:value = BMS_Info[id].Protection1;break;//94ah
            case 0x0063:value = BMS_Info[id].Protection2;break;//94ah
            case 0x0064:value = BMS_Info[id].Protection3;break;//94ah
            case 0x0065:value = BMS_Info[id].Protection4;break;//94ah
            case 0x0066:value = 0;break;//
            case 0x0067:value = 0;break;//
            case 0x0068:value = BMS_Info[id].Rack_Fault_Word;break;//68ah
            case 0x0069:value = BMS_Info[id].Sys_Alarm_Word;break;//68ah
            case 0x006A:value = BMS_Info[id].Rack_Alarm_Word;break;//68ah
            case 0x006B:value = 0;break;//
            case 0x006C:value = 0;break;//
            case 0x006D:value = 0;break;//
            default:value = 0; break;
        }
    }
    else if(addr == 0x0070)
    {
        value = Cur_DCDC_Num;
    }
    else if(addr >= 0x0071 && addr <= 0x00BC)       //DCDC信息
    {
        switch(addr)
        {
            case 0x0071:value = (uint32_t)DCDC_Info[id].Module_1.Module_Array_V;break;//阵列电压1
            case 0x0072:value = (uint32_t)DCDC_Info[id].Module_1.Module_Array_I;break;//阵列电流1
            case 0x0073:value = (uint32_t)DCDC_Info[id].Module_1.Module_Bat_V;break;//电池电压1
            case 0x0074:value = (uint32_t)DCDC_Info[id].Module_1.Module_Charge_I;break;//充电电流1
            case 0x0075:value = DCDC_Info[id].Module_1.Module_Charge_P;break;//充电功率1
            case 0x0076:value = (uint32_t)DCDC_Info[id].Module_1.Module_In_Temp;break;//机内温度1
            case 0x0077:value = (uint32_t)DCDC_Info[id].Module_1.Module_Bat_Temp;break;//电池温度1
            case 0x0078:
                if(DCDC_Com_Err[id])
                    value = (uint32_t)6;
                else
                    value = (uint32_t)DCDC_Info[id].Module_1.Module_Dev_Status;
                break;//设备状态1

            case 0x0085:value = (uint32_t)DCDC_Info[id].Module_2.Module_Array_V;break;//阵列电压2
            case 0x0086:value = (uint32_t)DCDC_Info[id].Module_2.Module_Array_I;break;//阵列电流2
            case 0x0087:value = (uint32_t)DCDC_Info[id].Module_2.Module_Bat_V;break;//电池电压2
            case 0x0088:value = (uint32_t)DCDC_Info[id].Module_2.Module_Charge_I;break;//充电电流2
            case 0x0089:value = DCDC_Info[id].Module_2.Module_Charge_P;break;//充电功率2
            case 0x008A:value = (uint32_t)DCDC_Info[id].Module_2.Module_In_Temp;break;//机内温度2
            case 0x008B:value = (uint32_t)DCDC_Info[id].Module_2.Module_Bat_Temp;break;//电池温度2
            case 0x008C:
                if(DCDC_Com_Err[id])
                    value = (uint32_t)6;
                else
                    value = (uint32_t)DCDC_Info[id].Module_2.Module_Dev_Status;
                break;//设备状态2

            case 0x0099:value = (uint32_t)DCDC_Info[id].Module_3.Module_Array_V;break;//阵列电压3
            case 0x009A:value = (uint32_t)DCDC_Info[id].Module_3.Module_Array_I;break;//阵列电流3
            case 0x009B:value = (uint32_t)DCDC_Info[id].Module_3.Module_Bat_V;break;//电池电压3
            case 0x009C:value = (uint32_t)DCDC_Info[id].Module_3.Module_Charge_I;break;//充电电流3
            case 0x009D:value = DCDC_Info[id].Module_3.Module_Charge_P;break;//充电功率3
            case 0x009E:value = (uint32_t)DCDC_Info[id].Module_3.Module_In_Temp;break;//机内温度3
            case 0x009F:value = (uint32_t)DCDC_Info[id].Module_3.Module_Bat_Temp;break;//电池温度3
            case 0x00A0:
                if(DCDC_Com_Err[id])
                    value = (uint32_t)6;
                else
                    value = (uint32_t)DCDC_Info[id].Module_3.Module_Dev_Status;
                break;//设备状态3

            case 0x00AD:value = (uint32_t)DCDC_Info[id].Module_4.Module_Array_V;break;//阵列电压4
            case 0x00AE:value = (uint32_t)DCDC_Info[id].Module_4.Module_Array_I;break;//阵列电流4
            case 0x00AF:value = (uint32_t)DCDC_Info[id].Module_4.Module_Bat_V;break;//电池电压4
            case 0x00B0:value = (uint32_t)DCDC_Info[id].Module_4.Module_Charge_I;break;//充电电流4
            case 0x00B1:value = DCDC_Info[id].Module_4.Module_Charge_P;break;//充电功率4
            case 0x00B2:value = (uint32_t)DCDC_Info[id].Module_4.Module_In_Temp;break;//机内温度4
            case 0x00B3:value = (uint32_t)DCDC_Info[id].Module_4.Module_Bat_Temp;break;//电池温度4
            case 0x00B4:
                if(DCDC_Com_Err[id])
                    value = (uint32_t)6;
                else
                    value = (uint32_t)DCDC_Info[id].Module_4.Module_Dev_Status;
                break;//设备状态4
            default:value = 0; break;
        }
    }
    else if(addr == 0x00C1)
    {
    	value = Cur_AMMETER_Num;
    }
    else if(addr >= 0x00C2 && addr <= 0x00CF)
    {
    	switch(addr)
    	{
    		case 0x00C2:value = (uint16_t)Meter_Info[id].VAB;break;
    		case 0x00C3:value = (uint16_t)Meter_Info[id].VBC;break;
    		case 0x00C4:value = (uint16_t)Meter_Info[id].VCA;break;
    		case 0x00C5:value = -(int32_t)Meter_Info[id].Total_active_P;break;
    		case 0x00C6:value = -(int32_t)Meter_Info[id].Total_Q;break;
    		case 0x00C7:value = (int16_t)Meter_Info[id].Total_P_factor;break;
    		default:value = 0; break;
    	}
    }
    else if(addr >= 0x00F0 && addr < 0x0130)
    {
    	switch(addr)
    	{
			case 0x00F1:value = (uint16_t)(DCDC_Com_Err[id]?0:DCDC_Info[id].Module_1.Device_Status);break; //设备状态1
			case 0x00F2:value = (uint16_t)((DCDC_Info[id].Module_1.Fault_Status==0x00)?0:1);break; //故障
			case 0x00F3:value = (uint16_t)DCDC_Info[id].Module_1.Bus_Bar_V;break;//母线电压
			case 0x00F4:value = (uint16_t)DCDC_Info[id].Module_1.Bus_Bar_I;break;//母线电流
			case 0x00F5:value = (uint16_t)DCDC_Info[id].Module_1.Battery_V;break;//蓄电池电压
			case 0x00F6:value = (uint16_t)DCDC_Info[id].Module_1.Battery_I;break;//蓄电池电流
			case 0x00F7:value = (int16_t)DCDC_Info[id].Module_1.Module1_T;break;//模组1温度
			case 0x00F8:value = (int16_t)DCDC_Info[id].Module_1.Module2_T;break;//模组2温度

			case 0x0101:value = (uint16_t)(DCDC_Com_Err[id]?0:DCDC_Info[id].Module_2.Device_Status);break; //设备状态2
			case 0x0102:value = (uint16_t)((DCDC_Info[id].Module_2.Fault_Status==0x00)?0:1);break; //故障
			case 0x0103:value = (uint16_t)DCDC_Info[id].Module_2.Bus_Bar_V;break;//母线电压
			case 0x0104:value = (uint16_t)DCDC_Info[id].Module_2.Bus_Bar_I;break;//母线电流
			case 0x0105:value = (uint16_t)DCDC_Info[id].Module_2.Battery_V;break;//蓄电池电压
			case 0x0106:value = (uint16_t)DCDC_Info[id].Module_2.Battery_I;break;//蓄电池电流
			case 0x0107:value = (int16_t)DCDC_Info[id].Module_2.Module1_T;break;//模组1温度
			case 0x0108:value = (int16_t)DCDC_Info[id].Module_2.Module2_T;break;//模组2温度

			case 0x0111:value = (uint16_t)(DCDC_Com_Err[id]?0:DCDC_Info[id].Module_3.Device_Status);break; //设备状态3
			case 0x0112:value = (uint16_t)((DCDC_Info[id].Module_3.Fault_Status==0x00)?0:1);break; //故障
			case 0x0113:value = (uint16_t)DCDC_Info[id].Module_3.Bus_Bar_V;break;//母线电压
			case 0x0114:value = (uint16_t)DCDC_Info[id].Module_3.Bus_Bar_I;break;//母线电流
			case 0x0115:value = (uint16_t)DCDC_Info[id].Module_3.Battery_V;break;//蓄电池电压
			case 0x0116:value = (uint16_t)DCDC_Info[id].Module_3.Battery_I;break;//蓄电池电流
			case 0x0117:value = (int16_t)DCDC_Info[id].Module_3.Module1_T;break;//模组1温度
			case 0x0118:value = (int16_t)DCDC_Info[id].Module_3.Module2_T;break;//模组2温度

			case 0x0121:value = (uint16_t)(DCDC_Com_Err[id]?0:DCDC_Info[id].Module_4.Device_Status);break; //设备状态4
			case 0x0122:value = (uint16_t)((DCDC_Info[id].Module_4.Fault_Status==0x00)?0:1);break; //故障
			case 0x0123:value = (uint16_t)DCDC_Info[id].Module_4.Bus_Bar_V;break;//母线电压
			case 0x0124:value = (uint16_t)DCDC_Info[id].Module_4.Bus_Bar_I;break;//母线电流
			case 0x0125:value = (uint16_t)DCDC_Info[id].Module_4.Battery_V;break;//蓄电池电压
			case 0x0126:value = (uint16_t)DCDC_Info[id].Module_4.Battery_I;break;//蓄电池电流
			case 0x0127:value = (int16_t)DCDC_Info[id].Module_4.Module1_T;break;//模组1温度
			case 0x0128:value = (int16_t)DCDC_Info[id].Module_4.Module2_T;break;//模组2温度
            default:value = 0; break;
    	}
    }
    else if(addr == 0x130)
    {
        value = Cur_PV_Num;
    }
    else if(addr >= 0x0131 && addr <= 0x0140)       //光伏逆变器信息
    {
        switch(addr)
        {
            case 0x0131:value = PV_Info[id].PV3x.RunParameter.DC_Voltage1;break;//当前显示光伏逆变器直流电压1
            case 0x0132:value = PV_Info[id].PV3x.RunParameter.DC_Current1;break;//当前显示光伏逆变器直流电流1
            case 0x0133:value = PV_Info[id].PV3x.RunParameter.DC_Voltage2;break;//当前显示光伏逆变器直流电压2
            case 0x0134:value = PV_Info[id].PV3x.RunParameter.DC_Current2;break;//当前显示光伏逆变器直流电流2
            case 0x0135:value = PV_Info[id].PV3x.RunParameter.VA;break;//当前显示光伏逆变器电网电压VA
            case 0x0136:value = PV_Info[id].PV3x.RunParameter.VB;break;//当前显示光伏逆变器电网电压VB
            case 0x0137:value = PV_Info[id].PV3x.RunParameter.VC;break;//当前显示光伏逆变器电网电压VC
            case 0x0138:value = PV_Info[id].PV3x.RunParameter.Frequency;break;//当前显示光伏逆变器电网频率
            case 0x0139:value = PV_Info[id].PV3x.RunParameter.Total_Active_Power;break;//当前显示光伏逆变器有功功率
            case 0x013A:value = PV_Info[id].PV3x.RunParameter.Reactive_Power;break;//当前显示光伏逆变器无功功率
            case 0x013B:value = PV_Info[id].PV3x.RunParameter.Power_Factor;break;//当前显示光伏逆变器功率因数
            case 0x013C:value = PV_Info[id].PV3x.RunParameter.Total_Generation;break;//当前显示光伏逆变器累计发电
            case 0x013D:value = Run_Status_For_PV(id, PV_Info[id].PV3x.RunParameter.Inverter_Status);break;//当前显示光伏逆变器工作状态
            case 0x013E:value = PV_Info[id].PV4x.ParaSet.Power_ONOFF;break;//当前显示光伏逆变器开关机状态
            case 0x013F:value = PV_Info[id].PV4x.ParaSet.PowerLimit_Switch;break;//当前显示光伏逆变器限功率开关状态
            case 0x0140:value = PV_Info[id].PV4x.ParaSet.PowerLimit_Set;break;//当前显示光伏逆变器限功率值
            default:value = 0; break;
        }
    }
    else if(addr >= 0x014F && addr <= 0x03FF)
    {
        switch(addr)
        {
            case 0x014F:value = (uint16_t)(FAULT?6:(EMGSTOP?9:Run_Status(EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Total_Status)));break;   //系统状态
            case 0x0150:value = System_Info.Config.EMS_Run_Priority;break;                              //运行模式  0-电网优先；1-微网优先
            case 0x0151:value = System_Info.Config.EMS_Start_Type;break;                                //启动类型  0-主动模式；1-被动模式
            case 0x0152:value = EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word1;break;    //故障1
            case 0x0153:value = EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Fault_Word2;break;    //故障2
            case 0x0154:value = EMS_Info.EMS3x.EMS_Controller_Info.RCommunication.Alarm_Word;break;     //告警
            case 0x0155:value = 0;break;                                                                //充电量补偿值
            case 0x0156:value = 0;break;                                                                //放电量补偿值
            case 0x0157:value = System_Info.Net.ip[0];break;                                            //IP地址[0]
            case 0x0158:value = System_Info.Net.ip[1];break;                                            //IP地址[1]
            case 0x0159:value = System_Info.Net.ip[2];break;                                            //IP地址[2]
            case 0x015A:value = System_Info.Net.ip[3];break;                                            //IP地址[3]
            case 0x015B:value = System_Info.Net.submask[0];break;                                       //子网掩码[0]
            case 0x015C:value = System_Info.Net.submask[1];break;                                       //子网掩码[1]
            case 0x015D:value = System_Info.Net.submask[2];break;                                       //子网掩码[2]
            case 0x015E:value = System_Info.Net.submask[3];break;                                       //子网掩码[3]
            case 0x015F:value = System_Info.Net.gateway[0];break;                                       //网关[0]
            case 0x0160:value = System_Info.Net.gateway[1];break;                                       //网关[1]
            case 0x0161:value = System_Info.Net.gateway[2];break;                                       //网关[2]
            case 0x0162:value = System_Info.Net.gateway[3];break;                                       //网关[3]
            case 0x0163:value = System_Info.Baud_Rate;break;                                            //波特率
            case 0x0164:value = System_Info.Modbus_Address;break;                                       //上位机通讯地址
            case 0x0165:value = System_Info.Modbus_Port;break;                                          //modbus端口号
            case 0x0166:value = 2;break;//System_Info.IO_To_BMS_Mode;break;                             //BMS通讯方式
            case 0x0167:value = 0;break;//System_Info.BMS_Fault_Contact_Enabled;break;                  //BMS故障干接点
            case 0x0168:value = 0;break;//System_Info.BMS_Warning_Contact_Enabled;break;                //BMS告警干接点
            case 0x0169:value = 0;break;                                        //BMS1通讯地址
            case 0x016A:value = System_Info.IEC104_Address;break;                                       //IEC104通讯地址
            case 0x016B:value = System_Info.IEC104_T1;break;                                            //IEC104超时时间T1
            case 0x016C:value = System_Info.IEC104_T3;break;                                            //IEC104超时时间T3
            case 0x016D:value = System_Info.IEC104_K;break;                                             //IEC104参数k
            case 0x016E:value = System_Info.IEC104_W;break;                                             //IEC104参数w
            case 0x016F:value = 0;break;                                        //BMS2通讯地址

            case 0x0170:value = (showTime.tm_year)+2000;break;                                  //年
            case 0x0171:value = (showTime.tm_mon);break;                                        //月
            case 0x0172:value = (showTime.tm_day);break;                                        //日
            case 0x0173:value = (showTime.tm_hour);break;                                       //时
            case 0x0174:value = (showTime.tm_min);break;                                        //分

            case 0x0175:value = 0;break;//恒流模式电流
            case 0x0176:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_I;break;
            case 0x0177:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V;break;
            case 0x0178:value = 0;break;//(int32_t)EMS_Info.EMS4x.EMS_Controller_Info.EM.Const_P_DC_Mode_P;break; //恒直流功率功率值
            case 0x0179:value = 0;break;//(int32_t)EMS_Info.EMS4x.EMS_Controller_Info.EM.Const_P_AC_Mode_P;break; //恒交流功率功率值

            case 0x017A:value = 0;break;        //恒流放电电流(保留)
            case 0x017B:value = 0;break;        //限压放电电压(保留)
            case 0x017C:value = System_Run_Info.E_Load_Adjust;break;        //负载总用电量校正
            case 0x017D:value = EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Power_Set;break;           //并网运行功率设置

            case 0x0180:value = EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Voltage;break;                  //独立逆变电压
            case 0x0181:value = EMS_Info.EMS4x.AGC_Info.SysParameter.MG_Frequency;break;                //独立逆变频率
            case 0x0182:value = Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Run_Mode;break;   //工作模式                                //负载总用电量校正
            case 0x0183:value = EMS_RL_Mode;break;                                                      //远程/本地方式设置
            case 0x0184:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Qvar_Adj_Mode;break;               //无功调节方式
            case 0x0185:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Power_Factor;break;                //功率因数
            case 0x0186:value = EMS_Info.EMS4x.AGC_Info.SysParameter.EMS_Run_Q_Set;break;               //无功功率
            case 0x0187:value = System_Info.Config.PCS_Grid_Run_Mode;break;                             //并网模式设置
            case 0x0188:value = System_Info.Config.EMS_Start_Type;break;                                //启动类型：0:主动1:被动
            case 0x0189:value = 0;break;                                                                //起停状态
            case 0x018A:value = 0;break;                                                                //BMS控制状态
            case 0x018B:value = Time_Div_Info.Time_Div_Set_Info[Time_Div_Info.Period].Power_Ctrl_Mode;break;        //当前并网控制模式

            case 0x018C:value = 0;break;//VSG_Para.VSG_u0_set;break;                                                //VSG输出电压给定值
            case 0x018D:value = 0;break;//VSG_Para.VSG_w0_set;break;                                                //VSG输出频率给定值
            case 0x018E:value = 0;break;//VSG_Para.VSG_P_down_control_ratio;break;                              //有功下垂控制系数
            case 0x018F:value = 0;break;//VSG_Para.VSG_Q_down_control_ratio;break;                              //无功下垂控制系数
            case 0x0190:value = 0;break;//VSG_Para.VSG_Pref_set;break;                                          //二次调频给定值
            case 0x0191:value = 0;break;//VSG_Para.VSG_Qref_set;break;                                         	//无功下发给定值
            case 0x019A:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Min;break;			//恒压模式电压下限
            case 0x019B:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Const_V_Mode_V_Max;break;			//恒压模式电压上限

            case 0x020B:value = IAT_Info.Set_Info.MAC_Lock;break;										//MAC锁定状态
            case 0x020C:value = (GRID_RUNNING||ISLAND_RUNNING)?1:0;break;								//系统运行状态
            case 0x0210:value = System_Info.Net2.ip[0];break;                                            //IP地址[0]
            case 0x0211:value = System_Info.Net2.ip[1];break;                                            //IP地址[1]
            case 0x0212:value = System_Info.Net2.ip[2];break;                                            //IP地址[2]
            case 0x0213:value = System_Info.Net2.ip[3];break;                                            //IP地址[3]
            case 0x0214:value = System_Info.Net2.submask[0];break;                                       //子网掩码[0]
            case 0x0215:value = System_Info.Net2.submask[1];break;                                       //子网掩码[1]
            case 0x0216:value = System_Info.Net2.submask[2];break;                                       //子网掩码[2]
            case 0x0217:value = System_Info.Net2.submask[3];break;                                       //子网掩码[3]
            case 0x0218:value = System_Info.Net2.gateway[0];break;                                       //网关[0]
            case 0x0219:value = System_Info.Net2.gateway[1];break;                                       //网关[1]
            case 0x021A:value = System_Info.Net2.gateway[2];break;                                       //网关[2]
            case 0x021B:value = System_Info.Net2.gateway[3];break;                                       //网关[3]

            case 0x0220:value = System_Info.NTP.ip[0];break;
            case 0x0221:value = System_Info.NTP.ip[1];break;
            case 0x0222:value = System_Info.NTP.ip[2];break;
            case 0x0223:value = System_Info.NTP.ip[3];break;
            case 0x0224:value = System_Info.NTP.port;break;
            case 0x0225:value = System_Info.NTP.enable;break;

            case 0x0230:value = System_Info.Net.mac[0];break;
            case 0x0231:value = System_Info.Net.mac[1];break;
            case 0x0232:value = System_Info.Net.mac[2];break;
            case 0x0233:value = System_Info.Net.mac[3];break;
            case 0x0234:value = System_Info.Net.mac[4];break;
            case 0x0235:value = System_Info.Net.mac[5];break;

            case 0x0240:value = System_Info.Net2.mac[0];break;
            case 0x0241:value = System_Info.Net2.mac[1];break;
            case 0x0242:value = System_Info.Net2.mac[2];break;
            case 0x0243:value = System_Info.Net2.mac[3];break;
            case 0x0244:value = System_Info.Net2.mac[4];break;
            case 0x0245:value = System_Info.Net2.mac[5];break;

            case 0x0206:value = 0;break;                                                         		//是否是超级密码登录  X
            case 0x0282:value = 0;break;                                                         		//当前用户标识		   X

            case 0x02B0:value = PCS_Com_Err[id];break;													//PCS通讯状态
            case 0x02B1:value = PV_Com_Err[id];break;													//PV通讯状态
            case 0x02B2:value = DCDC_Com_Err[id];break;													//DCDC通讯状态
            case 0x02B3:value = BMS_Com_Err[id];break;													//BMS通讯状态
            case 0x02B4:value = Meter_Com_Err[id];break;												//Meter通讯状态
            case 0x02B5:value = PVS_Com_Err[id];break;													//PVS通讯状态
            case 0x02B6:value = ENV_Com_Err[id];break;													//ENV通讯状态

            case 0x0300:value = ExportST;break;															//数据导出状态
            case 0x0301:value = Webserver_Language;break;                                               //页面语言

            case 0x0309:value = PCS_Pic_Direction;break;												//PCS图片流动方向			 用于主界面显示
            case 0x0310:value = ((System_Info.Config.Power_Distribution_Enable==ENABLE)?DIN(Grid_Contacter ):( GRID_RUNNING                 ?1:0));break; //电网侧接触器状态 用于主界面显示
            case 0x0311:value = ((System_Info.Config.Power_Distribution_Enable==ENABLE)?DIN(MGrid_Contacter):((ISLAND_RUNNING||GRID_RUNNING)?1:0));break; //微网侧接触器状态 用于主界面显示
            case 0x0312:value = Grid_Pic_Direction;break;                                               //电网图片滚动方向          用于主界面显示
            case 0x0313:value = MGrid_Pic_Direction;break;                                              //微网图片流动方向          用于主界面显示
            case 0x0314:value = PV_Pic_Direction;break;                                                 //光伏逆变器图片滚动方向     用于主界面显示
            case 0x0315:value = Load_Pic_Direction;break;                                               //负载图片滚动方向          用于主界面显示
            case 0x0316:value = (uint32_t)(!DC_BUS_SYS);break;                                          //是否交流母线系统？        用于主界面显示
            case 0x0317:value = (Meter_Info[0].VAB+Meter_Info[0].VBC+Meter_Info[0].VCA)/3;break;        //电网电压      用于主界面显示
            case 0x0318:value = (uint32_t)EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Voltage;break;          //负载电压                  用于主界面显示
            case 0x0319:value = EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power;break;               //负载功率                  用于主界面显示
            case 0x031A:value = EMS_Info.EMS3x.SOC_Info.RMeasure.PCS_AC_Voltage_Max;break;              //PCS交流电压               用于主界面显示
            case 0x031B:value = EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power;break;                //PCS功率                 用于主界面显示
            case 0x031C:value = System_Run_Info.PV_DC_Voltage;break;                                    //PV直流电压                用于主界面显示
            case 0x031D:value = EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power;break;                 //PV功率                  用于主界面显示
            case 0x031E:value = EMS_Info.EMS3x.SOC_Info.RMeasure.Bat_SOC_Max;break;                     //BMS soc                   用于主界面显示
            case 0x031F:value = 0;break;                                              					//BMS 健康度               用于主界面显示
            case 0x0320:value = System_Run_Info.PCS_DC_Voltage;break;                                   //PCS直流电压               用于主界面显示
            case 0x0321:value = EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Reactive_Power;break;				//负载无功功率
            case 0x0322:value = EMS_Info.EMS3x.AGC_Info.RMeasure.Grid_Active_Power;break; 				//电网功率

            case 0x0330:value = System_Run_Info.Day_Run_Time;break;										//日运行时间
            case 0x0331:value = System_Run_Info.Total_Run_Time;break;    								//总运行时间
            case 0x0332:value = System_Run_Info.Day_MG_Run_Time;break;    								//日微网运行时间
            case 0x0333:value = System_Run_Info.Total_MG_Run_Time;break;    							//总微网运行时间
            case 0x0334:value = System_Run_Info.Day_Grid_Run_Time;break;    							//日电网运行时间
            case 0x0335:value = System_Run_Info.Total_Grid_Run_Time;break;    							//总电网运行时间
            case 0x0336:value = System_Run_Info.Day_Charge_Time;break;    								//日充电时间
            case 0x0337:value = System_Run_Info.Total_Charge_Time;break;    							//总充电运行时间
            case 0x0338:value = System_Run_Info.Day_Discharge_Time;break;    							//日放电时间
            case 0x0339:value = System_Run_Info.Total_Discharge_Time;break;    							//总放电运行时间
            case 0x033A:value = System_Run_Info.Day_Load_Supply_Time;break;    							//日负载供电运行时间
            case 0x033B:value = System_Run_Info.Total_Load_Supply_Time;break;    						//总负载供电运行时间
            case 0x033C:value = System_Run_Info.Day_Fault_Time;break;    								//日故障时间
            case 0x033D:value = System_Run_Info.Total_Fault_Time;break;    								//总故障时间
            case 0x033E:value = System_Run_Info.Day_Charge_Energy;break;    							//电池日充电量
            case 0x033F:value = System_Run_Info.Day_Discharge_Energy;break; 							//电池日放电量
            case 0x0340:value = System_Run_Info.Day_Load_Energy;break;      							//Load总放电量
            case 0x0341:value = System_Run_Info.Total_Load_Energy;break;    							//负载总用电量
            case 0x0342:value = System_Run_Info.Total_Charge_Energy;break;    							//电池总充电量
            case 0x0343:value = System_Run_Info.Total_Discharge_Energy;break;    						//电池总放电量
            case 0x0344:value = System_Run_Info.Day_PV_Energy;break;									//光伏日发电量
            case 0x0345:value = System_Run_Info.Total_PV_Energy;break;    								//光伏总发电量

            case 0x0350:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;break;            //储能变流器总台数
            case 0x0351:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;break;             //光伏逆变器总台数
            case 0x0352:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number;break;           //DCDC总台数
            case 0x0353:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number;break;    		//PVS总台数
            case 0x0354:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number;break;            //BMS总台数
            case 0x0355:value = EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number;break;          //电表总台数

			case 0x0360:value = Max_anti_reflux_power;break;           	 								//最大逆流功率
			case 0x0361:value = EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_SetPara_Delay;break; 		//网口下发周期 100~2000ms
			case 0x0362:value = EMS_Info.EMS4x.AGC_Info.SysParameter.PCS_Net_Set_Cycle;break;       	//网口设置周期 1~50

            case 0x036F:value = System_Info.Config.Ammeter_Protocol_Type;break;							//电表协议选择
            case 0x0370:value = System_Info.Config.Balancing_Enable;break;                              //均衡控制使能
            case 0x0371:value = System_Info.Config.PCS_Island_Ctrl_Mode;break;                          //PCS离网启动策略
            case 0x0372:value = EMS_Info.EMS4x.EMS_Controller_Info.Baudrate.PCS_Baud;break;             //PCS波特率
            case 0x0373:value = EMS_Info.EMS4x.EMS_Controller_Info.Baudrate.PV_Baud;break;              //PV波特率
            case 0x0380:value = System_Info.Config.PCS_Protocol_Type;break;                             //协议选择
            case 0x0381:value = System_Info.Config.Bus_Type;break;                                      //总线类型
            case 0x0382:value = Grid_Connect_Type;break;                                                //是否并网
            case 0x0383:value = System_Info.Config.DC_Bus_Charge_Permit;break;                          //直流母线充电使能
            case 0x0384:value = System_Info.Config.System_Power_Set;break;                              //系统功率
            case 0x0385:value = System_Info.Config.PCS_Type;break;                                      //PCS机器类型
            case 0x0386:value = System_Info.Config.EMS_Run_Priority;break;                              //优先级

            case 0x0387:value = System_Info.Config.PV_Comm_Type;;break;
            case 0x0388:value = 0;break;                                                                //VSG模式使能开关
            case 0x0389:value = System_Info.Config.Time_Divide_Function;break;                          //是否分时段控制
            case 0x038A:value = System_Info.Config.BMS_Type;break;                                      //BMS类型  0-禁止 1-68ah 2-94ah
            case 0x038B:value = System_Info.Config.DCDC_Type;break;                                     //DCDC 类型 0-禁止 1-单向 2-双向
            case 0x038C:value = System_Info.Config.Meter_Comm_Enable;break;                             //电表通讯使能
            case 0x038D:value = System_Info.Config.PCS_Comm_Type;break;                                 //PCS通讯模式
            case 0x038E:value = System_Info.Config.Power_Distribution_Enable;break;                     //配电功能使能
            case 0x038F:value = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable;break;          //SOC使能

            case 0x0390:value = webPageShow.pcs;break;                                                  //web是否显示pcs界面
            case 0x0391:value = webPageShow.pv;break;                                                   //web是否显示pv界面
            case 0x0392:value = webPageShow.dcdc;break;                                                 //web是否显示dcdc界面
            case 0x0393:value = webPageShow.pvs;break;                                                  //web是否显示pvs界面
            case 0x0394:value = webPageShow.bms;break;                                                  //web是否显示bms界面
            case 0x0395:value = webPageShow.ammeter;break;                                              //web是否显示电表界面
            case 0x0396:value = webPageShow.realtimeMonitor;break;                                      //web是否显示实时监控界面
            case 0x0397:value = webPageShow.batteryctrl;break;                                          //web是否显示电池控制界面
            case 0x0398:value = 0;break;
            case 0x0399:value = 0;break;
            case 0x039A:value = webPageShow.env;break;													//web是否显示环境仪界面

			case 0x03A0:value = wInverterAcVoltDQKp;break;         										//KP
			case 0x03A1:value = wInverterAcVoltDQKi;break;         										//KI
			case 0x03A2:value = wInverterAcVoltDQKp_PVLimit;break; 										//pv KP
			case 0x03A3:value = wInverterAcVoltDQKi_PVLimit;break;										//pv KI
			case 0x03A4:value = Balancing_Kp;break;                										//KP
			case 0x03A5:value = System_Info.Zero_V_Max;break;      										//zero v
			case 0x03A6:value = Parameter_recovery_kp;break;            								//回调比例
			case 0x03A7:value = Parameter_recovery_dalay_time;break;    								//回调延时
			case 0x03B0:value = Send_Data_To_PV_Broadcast_Enable;break; 								//光伏广播使能
			case 0x03B1:value = (Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode==PEAK_LOAD_SHIFTING)?0xAA:0x55;break;
			case 0x03B2:value = Time_Div_Info.Time_Div_Set_Info[0].Start_Discharge_Power;break;
			case 0x03B3:value = Time_Div_Info.Time_Div_Set_Info[0].Start_Charge_Power;break;
			case 0x03B5:value = (Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode==ACSIDE_PVLIMIT)?0xAA:0x55;break;
			case 0x03B6:value = Time_Div_Info.Time_Div_Set_Info[0].Run_Power;break;
			case 0x03B7:value = Time_Div_Info.Time_Div_Set_Info[0].PCS_Autocharge_Power;break;
			case 0x03B8:value = (Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode==RAMP_RATE_CONTROL)?0xAA:0x55;break;
			case 0x03B9:value = Time_Div_Info.Time_Div_Set_Info[0].PV_Rated_Power;break;
			case 0x03BA:value = Time_Div_Info.Time_Div_Set_Info[0].Ramp_ratio;break;
			case 0x03BB:value = Time_Div_Info.Time_Div_Set_Info[0].Ramp_time;break;
			case 0x03BC:value = Time_Div_Info.Time_Div_Set_Info[0].SOC_Uplimit;break;
			case 0x03BD:value = Time_Div_Info.Time_Div_Set_Info[0].SOC_Lowlimit;break;
			case 0x03BE:value = Time_Div_Info.Time_Div_Set_Info[0].Vbat_Uplimit;break;
			case 0x03BF:value = Time_Div_Info.Time_Div_Set_Info[0].Vbat_Lowlimit;break;
			case 0x03C0:value = (Time_Div_Info.Time_Div_Set_Info[0].Power_Ctrl_Mode==FM)?0xAA:0x55;break;
			case 0x03C1:value = Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Digital;break;
			case 0x03C2:value = Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmax;break;
			case 0x03C3:value = Time_Div_Info.Time_Div_Set_Info[0].FM_Analog_Vmin;break;
			case 0x03C4:value = Time_Div_Info.Time_Div_Set_Info[0].Update_Cycle;break;   //300~30000ms
            default: value=0;break;
        }
    }
    else if(addr >= 0x0400 && addr < 0x04FF)
    {
        switch(addr)
        {
            case 0x0401:value = 0;break;//(int16_t)EMS_Info.EMS4x.EMS_Controller_Info.EM.Const_V_Mode_I_Min;break;
            case 0x0402:value = 0;break;//(int16_t)EMS_Info.EMS4x.EMS_Controller_Info.EM.Const_V_Mode_I_Max;;break;
            case 0x0403:value = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min;break;          //恒压模式电压下限
            case 0x0404:value = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max;break;          //恒压模式电压上限
            case 0x0405:value = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Min;break;          //恒压模式电流下限
            case 0x0406:value = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_I.Max;break;          //恒压模式电流上限
            case 0x0407:value = 0;break;//(int16_t)EMS_Info.EMS4x.EMS_Controller_Info.EM.Const_P_AC_Mode_P_Min;break;  //恒功率模式功率下限 , DC/AC范围一样
            case 0x0408:value = 0;break;//(int16_t)EMS_Info.EMS4x.EMS_Controller_Info.EM.Const_P_AC_Mode_P_Max;break;  //恒功率模式功率上限
            case 0x0409:value = 0;break;        //保留，根据具体项目设定
            case 0x040A:value = 0;break;        //保留，根据具体项目设定
            case 0x040B:value = 0;break;        //保留，根据具体项目设定
            case 0x040C:value = 0;break;        //保留，根据具体项目设定
            case 0x040D:value = 0;break;        //保留，根据具体项目设定
            case 0x040E:value = 0;break;        //保留，根据具体项目设定
            case 0x040F:value = 0;break;        //保留，根据具体项目设定
            case 0x0410:value = P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Min;break;       //独立逆变电压下限，根据具体项目设定
            case 0x0411:value = P_Range[System_Info.Config.PCS_Type].MGrid_Voltage.Max;break;       //独立逆变电压上限，根据具体项目设定
            case 0x0412:value = P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Min;break;     //独立逆变频率下限，根据具体项目设定
            case 0x0413:value = P_Range[System_Info.Config.PCS_Type].MGrid_Frequency.Max;break;     //独立逆变频率上限，根据具体项目设定
            case 0x0414:value = (int16_t)(P_Range[System_Info.Config.PCS_Type].Power_Factor.Min);break; //功率因数下限
            case 0x0415:value = (int16_t)(P_Range[System_Info.Config.PCS_Type].Power_Factor.Max);break;     //功率因数上限
            case 0x0416:value = -(int16_t)((float)System_Info.Config.System_Power_Set*0.3);break;           //无功比例下限
            case 0x0417:value =  (int16_t)((float)System_Info.Config.System_Power_Set*0.3);break;           //无功比例上限
            case 0x0418:value = 0;break;//VSG_Range[VSG_U0].Min;break;
            case 0x0419:value = 0;break;//VSG_Range[VSG_U0].Max;break;
            case 0x041A:value = 0;break;//VSG_Range[VSG_W0].Min;break;
            case 0x041B:value = 0;break;//VSG_Range[VSG_W0].Max;break;
            case 0x041C:value = 0;break;//VSG_Range[VSG_P_DOWN_CONTROL_RATIO].Min;break;
            case 0x041D:value = 0;break;//VSG_Range[VSG_P_DOWN_CONTROL_RATIO].Max;break;
            case 0x041E:value = 0;break;//VSG_Range[VSG_Q_DOWN_CONTROL_RATIO].Min;break;
            case 0x041F:value = 0;break;//VSG_Range[VSG_Q_DOWN_CONTROL_RATIO].Max;break;
            case 0x0420:value = 0;break;//VSG_Range[VSG_PREF].Min;break;
            case 0x0421:value = 0;break;//VSG_Range[VSG_PREF].Max;break;
            case 0x0422:value = 0;break;//VSG_Range[VSG_QREF].Min;break;
            case 0x0423:value = 0;break;//VSG_Range[VSG_QREF].Max;break;
            case 0x0424:value = (int16_t)(-((float)System_Info.Config.System_Power_Set*1.1));break;         //EMS并网运行功率设置下限
            case 0x0425:value = (int16_t)((float)System_Info.Config.System_Power_Set*1.1);break;            //EMS并网运行功率设置上限
            case 0x0426:value = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min;break;
            case 0x0427:value = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max;break;
            case 0x0428:value = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Min;break;
            case 0x0429:value = P_Range[System_Info.Config.PCS_Type].Const_V_Mode_V.Max;break;

            case 0x0440:value = P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Min;break;    //SOC上限2级下限
            case 0x0441:value = P_Range[System_Info.Config.PCS_Type].SOC_Uplimit2.Max;break;    //SOC上限2级上限
            case 0x0442:value = P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Min;break;    //SOC上限1级下限
            case 0x0443:value = P_Range[System_Info.Config.PCS_Type].SOC_Uplimit1.Max;break;    //SOC上限1级上限
            case 0x0444:value = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Min;break;   //SOC下限1级下限
            case 0x0445:value = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1.Max;break;   //SOC下限1级上限
            case 0x0446:value = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Min;break;   //SOC下限2级下限
            case 0x0447:value = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2.Max;break;   //SOC下限2级上限
            case 0x0448:value=0;break;   //SOC上限2恢复下限
            case 0x0449:value=500;break;   //SOC上限2恢复上限
            case 0x044A:value=0;break;   //SOC上限1恢复下限
            case 0x044B:value=500;break;   //SOC上限1恢复上限
            case 0x044C:value = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Min;break;   //SOC下限1恢复下限
            case 0x044D:value = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit1_Recover.Max;break;   //SOC下限1恢复上限
            case 0x044E:value = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Min;break;   //SOC下限2恢复下限
            case 0x044F:value = P_Range[System_Info.Config.PCS_Type].SOC_Lowlimit2_Recover.Max;break;   //SOC下限2恢复上限
            case 0x0450:value=0;break; //SOC 滞环控制下限
            case 0x0451:value=100;break; //SOC 滞环控制上限
            case 0x0452:value = P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Min;break;   //SOC控制延迟时间下限
            case 0x0453:value = P_Range[System_Info.Config.PCS_Type].SOC_Control_Delay.Max;break;   //SOC控制延迟时间上限
            case 0x0454:value = P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Min;break;   //SOC控制恢复时间下限
            case 0x0455:value = P_Range[System_Info.Config.PCS_Type].SOC_Control_Recover_Delay.Max;break;   //SOC控制恢复时间上限

            case 0x0456:value = P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Min;break;   //电池电压上限2级下限
            case 0x0457:value = P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit2.Max;break;   //电池电压上限2级上限
            case 0x0458:value = P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Min;break;   //电池电压上限1级下限
            case 0x0459:value = P_Range[System_Info.Config.PCS_Type].VBAT_Uplimit1.Max;break;   //电池电压上限1级上限
            case 0x045A:value = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Min;break;  //电池电压下限1级下限
            case 0x045B:value = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1.Max;break;  //电池电压下限1级上限
            case 0x045C:value = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Min;break;  //电池电压下限2级下限
            case 0x045D:value = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2.Max;break;  //电池电压下限2级上限
            case 0x045E:value=3000;break;  //电池电压上限2恢复下限
            case 0x045F:value=5000;break;  //电池电压上限2恢复上限
            case 0x0460:value=3000;break;  //电池电压上限1恢复下限
            case 0x0461:value=6800;break;  //电池电压上限1恢复上限
            case 0x0462:value = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Min;break;  //电池电压下限1恢复下限
            case 0x0463:value = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit1_Recover.Max;break;  //电池电压下限1恢复上限
            case 0x0464:value = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Min;break;  //电池电压下限2恢复下限
            case 0x0465:value = P_Range[System_Info.Config.PCS_Type].VBAT_Lowlimit2_Recover.Max;break;  //电池电压下限2恢复上限
            case 0x0466:value=0;break; //电池电压 滞环控制下限
            case 0x0467:value=500;break; //电池电压 滞环控制上限
            case 0x0468:value = P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Min;break;  //电池电压控制延迟时间下限
            case 0x0469:value = P_Range[System_Info.Config.PCS_Type].VBAT_Control_Delay.Max;break;  //电池电压控制延迟时间上限
            case 0x046A:value = P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Min;break;  //电池电压控制恢复时间下限
            case 0x046B:value = P_Range[System_Info.Config.PCS_Type].VBAT_Control_Recover_Delay.Max;break;  //电池电压控制恢复时间上限

			case 0x0470:value = System_Info.Config.Meter_Position_Enable[0];break;
			case 0x0471:value = System_Info.Config.Meter_Position_Enable[1];break;
			case 0x0472:value = System_Info.Config.Meter_Position_Enable[2];break;

            case 0x0490:value = System_Info.Config.InputPin_Function[0];break;
            case 0x0491:value = System_Info.Config.InputPin_NC_NO[0];break;
            case 0x0492:value = System_Info.Config.InputPin_Function[1];break;
            case 0x0493:value = System_Info.Config.InputPin_NC_NO[1];break;
            case 0x0494:value = System_Info.Config.InputPin_Function[2];break;
            case 0x0495:value = System_Info.Config.InputPin_NC_NO[2];break;
            case 0x0496:value = System_Info.Config.InputPin_Function[3];break;
            case 0x0497:value = System_Info.Config.InputPin_NC_NO[3];break;
            case 0x0498:value = System_Info.Config.InputPin_Function[4];break;
            case 0x0499:value = System_Info.Config.InputPin_NC_NO[4];break;
            case 0x049A:value = System_Info.Config.InputPin_Function[5];break;
            case 0x049B:value = System_Info.Config.InputPin_NC_NO[5];break;
            case 0x049C:value = System_Info.Config.InputPin_Function[6];break;
            case 0x049D:value = System_Info.Config.InputPin_NC_NO[6];break;
            case 0x049E:value = System_Info.Config.InputPin_Function[7];break;
            case 0x049F:value = System_Info.Config.InputPin_NC_NO[7];break;
            default: value=0;break;
        }
    }
    else if(addr == 0x04FF)
    {
        value=0;
    }
    else if(addr >= 0x0500 && addr <= 0x05FF)
    {
        uint8_t index = 0;
        index = (addr-0x0500)/16;
        uint32_t Addr_r = 0x0500+((addr-0x0500))%16;
        if(index>11) return 0;
        switch(Addr_r)
        {
            case 0x0500: value = 0;break;   //年
            case 0x0501: value = 0;break;   //月
            case 0x0502: value = 0;break;   //日
            case 0x0503: value = Time_Div_Info.Time_Div_Set_Info[index].Start_time.hour;break;
            case 0x0504: value = Time_Div_Info.Time_Div_Set_Info[index].Start_time.min;break;
            case 0x0505: value = 0;break;   //年
            case 0x0506: value = 0;break;   //月
            case 0x0507: value = 0;break;   //日
            case 0x0508: value = Time_Div_Info.Time_Div_Set_Info[index].End_time.hour;break;
            case 0x0509: value = Time_Div_Info.Time_Div_Set_Info[index].End_time.min;break;
            case 0x050A:
            	if(System_Info.Config.Power_Distribution_Enable==0xAA)
            	{
            		value = Time_Div_Info.Time_Div_Set_Info[index].Run_Mode;
            	}
            	else if(System_Info.Config.Power_Distribution_Enable==0x55)
            	{
            		if(Time_Div_Info.Time_Div_Set_Info[index].Run_Mode==2)  //配电禁止的情况下没有并离网切换
            			Time_Div_Info.Time_Div_Set_Info[index].Run_Mode=0;
            		value = Time_Div_Info.Time_Div_Set_Info[index].Run_Mode;
            	}
            break;
            case 0x050B: value = Time_Div_Info.Time_Div_Set_Info[index].Power_Ctrl_Mode;break;
            case 0x050C: value = Time_Div_Info.Time_Div_Set_Info[index].Run_Power;break;
            case 0x050D: value = Time_Div_Info.Time_Div_Set_Info[index].Mgrid_Run_Power;break;
            default: value=0;break;
        }
    }
    else if(addr >= 0x0600 && addr < 0x0640)
    {
        switch(addr)
        {
            case 0x0600:value = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Enable;break;    //SOC控制使能
            case 0x0601:value = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit2;break;          //SOC上限2级
            case 0x0602:value = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Uplimit1;break;          //SOC上限1级
            case 0x0603:value = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1;break;         //SOC下限1级
            case 0x0604:value = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2;break;         //SOC下限2级
            case 0x0605:value = 700;break;                  //SOC上限2级恢复
            case 0x0606:value = 500;break;                  //SOC上限1级恢复
            case 0x0607:value = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit1_Recover;break;    //SOC下限1级恢复
            case 0x0608:value = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Lowlimit2_Recover;break;    //SOC下限2级恢复
            case 0x0609:value = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Float_Hysteresis;break;    //SOC滞环控制
            case 0x060A:value = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Delay;break;    //SOC控制延迟时间
            case 0x060B:value = EMS_Info.EMS4x.SOC_Info.RunParameter.SOC_Control_Recover_Delay;break;    //SOC控制恢复时间

            case 0x0610:value = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Enable;break;    //电池电压控制使能
            case 0x0611:value = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit2;break;    //电池电压上限2级
            case 0x0612:value = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Uplimit1;break;    //电池电压上限1级
            case 0x0613:value = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1;break;    //电池电压下限1级
            case 0x0614:value = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2;break;    //电池电压下限2级
            case 0x0615:value = 6800;break;                 //电池电压上限2级恢复
            case 0x0616:value = 6000;break;                 //电池电压上限1级恢复
            case 0x0617:value = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit1_Recover;break;    //电池电压下限1级恢复
            case 0x0618:value = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Lowlimit2_Recover;break;    //电池电压下限2级恢复
            case 0x0619:value = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Float_Hysteresis;break;    //电池电压下限2级恢复
            case 0x061A:value = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Delay;break;    //电池电压控制延迟时间
            case 0x061B:value = EMS_Info.EMS4x.SOC_Info.RunParameter.VBAT_Control_Recover_Delay;break;    //电池电压控制恢复时间
        }
    }
    else if(addr >= 0x0640 && addr <= 0x067B)        //外接设备modbus地址
    {
    	uint8_t index = 0;
    	index = addr-0x640;
    	value = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PCS_Address[index].Modubus_Address;
    }
    else if(addr >= 0x06B0 && addr <= 0x06EB)
    {
    	uint8_t index = 0;
    	index = addr-0x6B0;
    	value = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PV_Address[index].Modubus_Address;
    }
    else if(addr >= 0x06F0 && addr <= 0x070E)
    {
    	uint8_t index = 0;
    	index = addr-0x6F0;
    	value = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.DCDC_Address[index].Modubus_Address;
    }
    else if(addr >= 0x0840 && addr <= 0x087E)
    {
    	uint8_t index = 0;
    	index = addr-0x840;
    	value = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.BMS_Address[index].Modubus_Address;
    }
    else if(addr >= 0x0750 && addr <= 0x075B)
    {
    	uint8_t index = 0;
    	index = addr-0x750;
    	value = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.Meter_Address[index].Modubus_Address;
    }
    else if(addr >= 0x0760 && addr <= 0x0791)
    {
    	uint8_t index = 0;
    	index = addr-0x760;
    	value = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.PVS_Address[index].Modubus_Address;
    }
    else if(addr >= 0x900 && addr < 0x0900+ENV_NUM)
    {
    	uint8_t index = 0;
    	index = addr-0x900;
    	value = EMS_Info.EMS4x.EMS_Controller_Info.Equip_Address.ENV_Address[index].Modubus_Address;
    }
    else if(addr >= 0x1000 && addr < 0x2000)
    {
    	uint8_t index = 0, sel = 0;
    	index = (addr-0x1000)/5;
    	sel = (addr-0x1000)%5;
        switch(sel)
        {
            //目标服务器参数
            case 0x00:value = System_Info.Server[index].ip[0];break;//IP地址0
            case 0x01:value = System_Info.Server[index].ip[1];break;//IP地址1
            case 0x02:value = System_Info.Server[index].ip[2];break;//IP地址2
            case 0x03:value = System_Info.Server[index].ip[3];break;//IP地址3
            case 0x04:value = System_Info.Server_Modbus_Port[index];break;//端口号
            default: value = 0;break;
        }
    }
    else if(addr >= 0x2000 && addr < 0x3000)
    {
    	switch(addr)
    	{
			case 0x2000:value = comParam[0].baud;break;//串口1波特率
			case 0x2001:value = comParam[0].protocol;break;//串口1设备类型
			case 0x2002:value = comParam[0].sId;break;//串口1起始ID
			case 0x2003:value = comParam[0].eId;break;//串口1结束ID
			case 0x2004:value = comParam[1].baud;break;//串口2波特率
			case 0x2005:value = comParam[1].protocol;break;//串口2设备类型
			case 0x2006:value = comParam[1].sId;break;//串口2起始ID
			case 0x2007:value = comParam[1].eId;break;//串口2结束ID
			case 0x2008:value = comParam[2].baud;break;//串口3波特率
			case 0x2009:value = comParam[2].protocol;break;//串口3设备类型
			case 0x200A:value = comParam[2].sId;break;//串口3起始ID
			case 0x200B:value = comParam[2].eId;break;//串口3结束ID
			case 0x200C:value = comParam[3].baud;break;//串口4波特率
			case 0x200D:value = comParam[3].protocol;break;//串口4设备类型
			case 0x200E:value = comParam[3].sId;break;//串口4起始ID
			case 0x200F:value = comParam[3].eId;break;//串口4结束ID
			case 0x2010:value = comParam[4].baud;break;//串口5波特率
			case 0x2011:value = comParam[4].protocol;break;//串口5设备类型
			case 0x2012:value = comParam[4].sId;break;//串口5起始ID
			case 0x2013:value = comParam[4].eId;break;//串口5结束ID
			case 0x2014:value = comParam[5].baud;break;//串口6波特率
			case 0x2015:value = comParam[5].protocol;break;//串口6设备类型
			case 0x2016:value = comParam[5].sId;break;//串口6起始ID
			case 0x2017:value = comParam[5].eId;break;//串口6结束ID
    		default: value = 0;break;
    	}
    }
    else if(addr >= 0x3000 && addr < 0x4000)
    {
    	uint8_t index = 0, sel = 0;
    	index = (addr-0x3000)/5;
    	sel = (addr-0x3000)%5;
        switch(sel)
        {
            //目标服务器参数
            case 0x00:value = System_Info.PVServer[index].ip[0];break;//IP地址0
            case 0x01:value = System_Info.PVServer[index].ip[1];break;//IP地址1
            case 0x02:value = System_Info.PVServer[index].ip[2];break;//IP地址2
            case 0x03:value = System_Info.PVServer[index].ip[3];break;//IP地址3
            case 0x04:value = System_Info.PVServer_Modbus_Port[index];break;//端口号
            default: value = 0;break;
        }
    }
    else if(addr >= 0x8000 && addr <= 0x8010)
    {
    	switch(addr)
    	{
    		case 0x8000: value = ENV_Info[0].Sunshine_Radiation_Intensity1;break;
    		case 0x8001: value = ENV_Info[0].Day_Radiation1;break;
    		case 0x8002: value = ENV_Info[0].All_Radiation1;break;
    		case 0x8003: value = ENV_Info[0].Sunshine_Radiation_Intensity2;break;
    		case 0x8004: value = ENV_Info[0].Day_Radiation2;break;
    		case 0x8005: value = ENV_Info[0].All_Radiation2;break;
    		case 0x8006: value = ENV_Info[0].Wind_Speed;break;
    		case 0x8007: value = ENV_Info[0].Wind_Level;break;
    		case 0x8008: value = ENV_Info[0].Wind_Direction_Degree;break;
    		case 0x8009: value = ENV_Info[0].Wind_Position;break;
    		case 0x800A: value = ENV_Info[0].Atmospheric_Pressure;break;
    		case 0x800B: value = (int16_t)ENV_Info[0].Battery_Board_Temperature;break;
    		case 0x800C: value = (int16_t)ENV_Info[0].Env_Temperature;break;
    		case 0x800D: value = ENV_Info[0].Env_Humidity;break;
    		default: value = 0;break;
    	}
    }
    return value;
}
float get_realtimedata_by_id(uint32_t addr)
{
    float value = 0;

    switch(addr)
    {
        case 0x02A0: value = (float)EMS_Info.EMS3x.AGC_Info.RMeasure.PCS_Active_Power/10.0;break;
        case 0x02A1: value = (float)System_Run_Info.PCS_DC_Voltage/10.0;break;
        case 0x02A2: value = (float)System_Run_Info.PCS_AC_Voltage/10.0;break;
        case 0x02A3: value = (float)EMS_Info.EMS3x.AGC_Info.RMeasure.MG_Frequency/100.0;break;
        case 0x02A4: value = (float)EMS_Info.EMS3x.AGC_Info.RMeasure.PV_Active_Power/10.0;break;
        case 0x02A5: value = (float)System_Run_Info.PV_DC_Voltage/10.0;break;
        case 0x02A6: value = (float)System_Run_Info.PV_AC_Voltage/10.0;break;
        case 0x02A7: value = (float)EMS_Info.EMS3x.AGC_Info.RMeasure.Load_Active_Power/10.0;break;
        default: value = 0;break;
    }
    return value;
}
void webserver_do_save_int(void)                                //异步保存参数值(数值型)
{
    Touch_Write_Time[0]=showTime.tm_year;
    Touch_Write_Time[1]=showTime.tm_mon;
    Touch_Write_Time[2]=showTime.tm_day;
    Touch_Write_Time[3]=showTime.tm_hour;
    Touch_Write_Time[4]=showTime.tm_min;
    Touch_Write_Time[5]=showTime.tm_sec;

    for(int i=0;i<PARAMETER_NUM;i++)
    {
        if(Param_flag[i]!=0)
        {
        	mutex_for_timer.lock();
            do_save_data_int(Param_flag[i],Param_value[i]);
            mutex_for_timer.unlock();
            Param_flag[i]=0;
        }
    }
    for(int i=0;i<6;i++)
    {
    	if(Serial_Param_Change[i])
    	{
    		Serial_Param_Change[i] = false;
    		mutex_for_timer.lock();
    		ctrl.serialRev_thread[i].SetComParam(comParam[i]);
    		ctrl.serialRev_thread[i].Set_Baud(comParam[i].baud);
    		mutex_for_timer.unlock();
    	}
    }
    if(Touch_Write_Time_Flag==true)
    {
        Touch_Write_Time_Flag=false;
        mutex_for_timer.lock();
        set_sys_time();
        get_sys_time();
        for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;i++)
        {
            Send_Data_To_PCS_0x10_01[i]=true;
        }
        for(uint8_t i=0;i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number;i++)
        {
            Send_Data_To_PV_0x10_01[i]=true;
        }
        mutex_for_timer.unlock();
        delay10s=true;
        Clear_Data = true;                              //判断是否是新的一天，是否要清除前一天的数据
    }
}
void s_handler(int id,char* data,char *ip)                               //处理初始化参数值
{
    char *ptemp=NULL;//切割出的内容
    char delim[] = "?=&+";//切割字符串
    uint16_t i=0,j=0;
    uint32_t id_No=~0;//参数序号
    uint32_t part=0;

    switch(id)
    {
        case 0: //登录
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "\"ID_0x0301\":\"%d\","   //页面语言
            		"\"ID_0x0202\":\"%s\"," //页面标题
            		"\"ID_0x0282\":\"%d\""   //用户名称
                "}",
                get_param_by_id(0x0301,0),
                Device_Info[DEVICE_NAME],
                Get_User_Authority_For_Web(ip)
            );
            s_Rsc[0].size = strlen(s_000);
        break;
        case 1: //框架
        case 2://概要信息
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
            		"ID_0x0202:'%s'," //页面标题
            		"ID_0x0282:%d,"   //用户名称
                    "ID_0x014F:%d,"   //EMS工作状态
                    "ID_0x0182:%d,"   //工作模式    PCS运行模式 0-并网 1-离网 2-并离网切换
                    "ID_0x018B:%d,"   //并网工作模式 0-固定功率, 1：跟踪负载功率, 2：PCC点功率控制, 3：上位机调度
                    "ID_0x0150:%d,"   //启动模式    0-电网优先；1-微网优先
                    "ID_0x0151:%d,"   //启动类型    0-主动模式；1-被动模式
            		"ID_0x0152:%d,"   //故障状态字1
            		"ID_0x0153:%d,"	  //故障状态字2
            		"ID_0x0154:%d,"   //告警状态
                    "ID_0x0016:%d,"   //微网电压
                    "ID_0x0017:%d,"   //微网频率
                    "ID_0x0004:%d,"   //电网状态
                    "ID_0x0005:%d,"   //负载总有功功率
                    "ID_0x0008:%d,"   //光伏总有功功率
                    "ID_0x000B:%d,"   //负载功率
            		"ID_0x0389:%d,"   //分时段使能
            		"ID_0x050B:%d"    //时段1控制模式
                "}",
                	get_param_by_id(0x0301,0),
                    Device_Info[DEVICE_NAME],
                    Get_User_Authority_For_Web(ip),
                    get_param_by_id(0x014F,0),
                    get_param_by_id(0x0182,0),
                    get_param_by_id(0x018B,0),
                    get_param_by_id(0x0150,0),
                    get_param_by_id(0x0151,0),
                    get_param_by_id(0x0152,0),
                    get_param_by_id(0x0153,0),
                    get_param_by_id(0x0154,0),
                    get_param_by_id(0x0016,0),
                    get_param_by_id(0x0017,0),
                    get_param_by_id(0x0004,0),
                    get_param_by_id(0x0005,0),
                    get_param_by_id(0x0008,0),
                    get_param_by_id(0x000B,0),
                    get_param_by_id(0x0389,0),
                    get_param_by_id(0x050B,0)
            );
            s_Rsc[0].size = strlen(s_000);
        break;
        case 3: //PCS信息
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);  //PCS设备编号
            if(id_No < 1 || id_No > PCS_NUM) id_No = 1;
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
                    "ID_0x0350:%d,"   //PCS总数
            		"ID_0x02B0:%d,"   //PCS通讯状态
                ,
                    get_param_by_id(0x0301,0),
                    get_param_by_id(0x0350,0),
                    get_param_by_id(0x02B0,id_No-1)
            );
            //PCS信息
            for(i=0x001F; i<=0x0031; i++)
            {
                //sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,Cur_PCS_Num-1));
            	if(i == 0x002D || i == 0x002E) sprintf(jsCharts_temp,"ID_0x%04X:%u,",i,get_param_by_id(i,id_No-1));
            	else sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
                strcat(s_000, jsCharts_temp);
            }
            strcat(s_000, "}");
            s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
            s_000[strlen(s_000)-1]='\0';
            s_Rsc[0].size = strlen(s_000);
        break;
        case 4: //系统参数
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No==0)
            {
                sprintf(s_000,
                    "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                    "{"
                        "ID_0x0301:%d,"   //页面语言
                        "ID_0x0282:%d,"   //用户名称
                        "ID_0x017C:%d,"   //负载总用电量校正
                        "ID_0x0170:%d,"   //系统时钟(年)
                        "ID_0x0171:%d,"   //系统时钟(月)
                        "ID_0x0172:%d,"   //系统时钟(日)
                        "ID_0x0173:%d,"   //系统时钟(时)
                        "ID_0x0174:%d,"   //系统时钟(分)
                		"ID_0x0220:%d,"
                		"ID_0x0221:%d,"
                		"ID_0x0222:%d,"
                		"ID_0x0223:%d,"
                		"ID_0x0224:%d,"
                		"ID_0x0225:%d,"
                		"ID_0x020C:%d,"
                		"ID_0x0341:%u"
                    "}",
                        get_param_by_id(0x0301,0),
                        Get_User_Authority_For_Web(ip),
                        get_param_by_id(0x017C,0),
                        get_param_by_id(0x0170,0),
                        get_param_by_id(0x0171,0),
                        get_param_by_id(0x0172,0),
                        get_param_by_id(0x0173,0),
                        get_param_by_id(0x0174,0),
                        get_param_by_id(0x0220,0),
                        get_param_by_id(0x0221,0),
                        get_param_by_id(0x0222,0),
                        get_param_by_id(0x0223,0),
                        get_param_by_id(0x0224,0),
                        get_param_by_id(0x0225,0),
                        get_param_by_id(0x020C,0),
                        get_param_by_id(0x0341,0)
                );
            }
            else
            {
                sprintf(s_000,
                    "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                    "{"
                );
                while(strcmp(ptemp,"time"))
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",id_No,get_param_by_id(id_No,0));
                    strcat(s_000, jsCharts_temp);
                    ptemp = strtok(NULL,delim);
                    id_No=(uint16_t)strtoul(ptemp,0,16);
                }
                strcat(s_000, "}");
                s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
                s_000[strlen(s_000)-1]='\0';
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 5: //运行参数
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No==0)
            {
                sprintf(s_000,
                    "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                    "{"
                        "ID_0x0301:%d,"   //页面语言
                        "ID_0x0282:%d,"   //用户权限
                		"ID_0x0386:%d,"   //优先级
                		"ID_0x038E:%d,"   //配电使能
                		"ID_0x038F:%d,"   //SOC使能
						"ID_0x03B2:%d,"   //峰值
						"ID_0x03B3:%d,"   //谷值
						"ID_0x03B6:%d,"   //限发功率
                		"ID_0x03B7:%d,"   //自充电功率
                		"ID_0x03B9:%d,"   //额定光伏功率
                		"ID_0x03BA:%d,"	  //斜率
                		"ID_0x03BB:%d,"   //斜率时间
                		"ID_0x03BC:%d,"   //soc上限
                		"ID_0x03BD:%d,"   //soc下限
                		"ID_0x03BE:%d,"   //Vbat上限
                		"ID_0x03BF:%d,"   //Vbat下限
                		"ID_0x03C1:%d,"   //数字/模拟调频选择
                		"ID_0x03C2:%d,"   //模拟上限
                		"ID_0x03C3:%d,"   //模拟下限
                		"ID_0x03C4:%d,"   //更新周期
                		"ID_0x045A:%d,"   //Vbat下限1下限
                		"ID_0x045B:%d,"   //Vbat下限1上限
                    ,
					get_param_by_id(0x0301,0),
					Get_User_Authority_For_Web(ip),
					get_param_by_id(0x0386,0),
					get_param_by_id(0x038E,0),
					get_param_by_id(0x038F,0),
					get_param_by_id(0x03B2,0),
					get_param_by_id(0x03B3,0),
					get_param_by_id(0x03B6,0),
					get_param_by_id(0x03B7,0),
					get_param_by_id(0x03B9,0),
					get_param_by_id(0x03BA,0),
					get_param_by_id(0x03BB,0),
					get_param_by_id(0x03BC,0),
					get_param_by_id(0x03BD,0),
					get_param_by_id(0x03BE,0),
					get_param_by_id(0x03BF,0),
					get_param_by_id(0x03C1,0),
					get_param_by_id(0x03C2,0),
					get_param_by_id(0x03C3,0),
					get_param_by_id(0x03C4,0),
					get_param_by_id(0x045A,0),
					get_param_by_id(0x045B,0)
                );
                //无功比例,功率因数,无功功率
                for(i=0x184; i<=0x0186; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,0));
                    strcat(s_000, jsCharts_temp);
                }
                memset(jsCharts_temp,0,1024);
                //主被动模式,起停控制,珺MS控制
                for(i=0x188; i<=0x018A; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,0));
                    strcat(s_000, jsCharts_temp);
                }
                memset(jsCharts_temp,0,1024);
                //VSG参数 VSG输出电压给定值,VSG输出频率给定值,有功下垂控制系数,无功下垂控制系数,二次调频给定值,无功下发给定值
                //for(i=0x18C; i<=0x0191; i++)
                //{
                //  sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,0));
                //  strcat(s_000, jsCharts_temp);
                //}
                memset(jsCharts_temp,0,1024);
                //系统功率
                for(i=0x0384; i<=0x0384; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,0));
                    strcat(s_000, jsCharts_temp);
                }
                memset(jsCharts_temp,0,1024);
                //VSG使能标志，分时控制使能标志, BMS类型
                for(i=0x0388; i<=0x038A; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,0));
                    strcat(s_000, jsCharts_temp);
                }
                memset(jsCharts_temp,0,1024);
                //功率因数上下限、无功功率上下限
                for(i=0x0414; i<=0x0417; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,0));
                    strcat(s_000, jsCharts_temp);
                }
                memset(jsCharts_temp,0,1024);
                //时段1~12参数
                for(j=0; j<12; j++)
                {
                    for(i=0x0503; i<=0x050D; i++)
                    {
                        sprintf(jsCharts_temp,"ID_0x%04X:%d,",i+j*16,get_param_by_id(i+j*16,0));
                        strcat(s_000, jsCharts_temp);
                    }
                }
                strcat(s_000, "}");
                s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
                s_000[strlen(s_000)-1]='\0';
            }
            else
            {
                sprintf(s_000,
                    "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                    "{"
                );
                while(strcmp(ptemp,"time"))
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",id_No,get_param_by_id(id_No,0));
                    strcat(s_000, jsCharts_temp);
                    ptemp = strtok(NULL,delim);
                    id_No=(uint16_t)strtoul(ptemp,0,16);
                }
                strcat(s_000, "}");
                s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
                s_000[strlen(s_000)-1]='\0';
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 6: //通讯参数
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No==0)  //读所有设备当前modbus地址
            {
                sprintf(s_000,
                    "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                    "{"
                        "ID_0x0301:%d,"   //页面语言
                        "ID_0x0282:%d,"   //用户权限
                		"ID_0x020B:%d,"
                		"ID_0x0230:%d,"
                		"ID_0x0231:%d,"
                		"ID_0x0232:%d,"
                		"ID_0x0233:%d,"
                		"ID_0x0234:%d,"
                		"ID_0x0235:%d,"
                		"ID_0x0240:%d,"
                		"ID_0x0241:%d,"
                		"ID_0x0242:%d,"
                		"ID_0x0243:%d,"
                		"ID_0x0244:%d,"
                		"ID_0x0245:%d,"
                    ,
                        get_param_by_id(0x0301,0),
                        Get_User_Authority_For_Web(ip),
                        get_param_by_id(0x020B,0),
                        get_param_by_id(0x0230,0),
                        get_param_by_id(0x0231,0),
                        get_param_by_id(0x0232,0),
                        get_param_by_id(0x0233,0),
                        get_param_by_id(0x0234,0),
                        get_param_by_id(0x0235,0),
                        get_param_by_id(0x0240,0),
                        get_param_by_id(0x0241,0),
                        get_param_by_id(0x0242,0),
                        get_param_by_id(0x0243,0),
                        get_param_by_id(0x0244,0),
                        get_param_by_id(0x0245,0)
                );
                for(i=0x157; i<=0x016E; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,0));
                    strcat(s_000, jsCharts_temp);
                }
                memset(jsCharts_temp,0,1024);
                for(i=0x210; i<=0x021B; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,0));
                    strcat(s_000, jsCharts_temp);
                }
                memset(jsCharts_temp,0,1024);
                for(i=0x350; i<=0x0355; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,0));
                    strcat(s_000, jsCharts_temp);
                }
                memset(jsCharts_temp,0,1024);
                for(i=0x390; i<=0x0395; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,0));
                    strcat(s_000, jsCharts_temp);
                }
                memset(jsCharts_temp,0,1024);
                for(i=0; i<24; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x2000+i,get_param_by_id(0x2000+i,0));
                    strcat(s_000, jsCharts_temp);
                }
                strcat(s_000, "}");
                s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
                s_000[strlen(s_000)-1]='\0';
            }
            else
            {
                sprintf(s_000,
                    "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                    "{"
                );
                while(strcmp(ptemp,"time"))
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",id_No,get_param_by_id(id_No,0));
                    strcat(s_000, jsCharts_temp);
                    ptemp = strtok(NULL,delim);
                    id_No=(uint16_t)strtoul(ptemp,0,16);
                }
                strcat(s_000, "}");
                s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
                s_000[strlen(s_000)-1]='\0';
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 10://系统信息
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"    //页面语言
                    "ID_0x0203:'%s',"  //LCD版本
                    "ID_0x0204:'%s',"  //Web版本
                    "ID_0x0205:'%s',"  //TOUCH版本
                    "ID_0x0282:%d"     //权限
                "}",
                    get_param_by_id(0x0301,0),
                    Device_Info[FIRMWARE_VEB1],  //LCD
                    Device_Info[WEB_VER], //WEB
                    Device_Info[FIRMWARE_VEB2], //TOUCH
                    Get_User_Authority(ip)
            );
            s_Rsc[0].size = strlen(s_000);
        break;
        case 12://logo
        case 47://logo
        sprintf(s_000,
            "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
            "{"
                 "ID_0x0201:'%s',"  //序列号
                "ID_0x0202:'%s',"  //设备名称
                "ID_0x0301:%d,"    //页面语言
                 "ID_0x0282:%d"     //用户名称
            "}",
            IAT_Info.Set_Info.SN_Number,
            Device_Info[DEVICE_NAME],
            get_param_by_id(0x0301,0),
            Get_User_Authority(ip)
        );
        s_Rsc[0].size = strlen(s_000);
        break;
        case 13://菜单
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d," //页面语言
                    "ID_0x0390:%d," //PCS界面显示状态
                    "ID_0x0391:%d," //PV界面显示状态
                    "ID_0x0392:%d," //DCDC界面显示状态
                    "ID_0x0393:%d," //PVS界面显示状态
                    "ID_0x0394:%d," //BMS界面显示状态
                    "ID_0x0395:%d," //电表界面显示状态
                    "ID_0x0396:%d,"  //实时监控界面显示状态
            		"ID_0x0397:%d,"
            		"ID_0x0398:%d,"
            		"ID_0x0399:%d,"
            		"ID_0x039A:%d"  //环境仪界面显示状态
                "}",
                    get_param_by_id(0x0301,0),
                    get_param_by_id(0x0390,0),
                    get_param_by_id(0x0391,0),
                    get_param_by_id(0x0392,0),
                    get_param_by_id(0x0393,0),
                    get_param_by_id(0x0394,0),
                    get_param_by_id(0x0395,0),
                    get_param_by_id(0x0396,0),
                    get_param_by_id(0x0397,0),
                    get_param_by_id(0x0398,0),
                    get_param_by_id(0x0399,0),
                    get_param_by_id(0x039A,0)
            );
            s_Rsc[0].size = strlen(s_000);
        break;
        case 15://PV信息
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);  //PV设备编号
            if(id_No < 1 || id_No > PV_NUM) id_No = 1;
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
                    "ID_0x0351:%d,"   //PV总数
            		"ID_0x02B1:%d,"   //PV通讯状态
                ,
                    get_param_by_id(0x0301,0),
                    get_param_by_id(0x0351,0),
                    get_param_by_id(0x02B1,id_No-1)
            );
            //PV信息
            for(i=0x0130; i<=0x0140; i++)
            {
                //sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,Cur_PV_Num-1));
            	if(i == 0x0139 || i == 0x013C) sprintf(jsCharts_temp,"ID_0x%04X:%u,",i,get_param_by_id(i,id_No-1));
            	else sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
                strcat(s_000, jsCharts_temp);
            }
            strcat(s_000, "}");
            s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
            s_000[strlen(s_000)-1]='\0';
            s_Rsc[0].size = strlen(s_000);
        break;
        case 16://PVS信息
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);  //PVS设备编号
            if(id_No < 1 || id_No > PVS_NUM) id_No = 1;
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
                    "ID_0x0353:%d,"   //PVS总数
            		"ID_0x02B5:%d,"   //PVS通讯状态
                ,
                    get_param_by_id(0x0301,0),
                    get_param_by_id(0x0353,0),
                    get_param_by_id(0x02B5,id_No-1)
            );
            //PVS信息
            for(i=0x0040; i<=0x0047; i++)
            {
                //sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,Cur_PVS_Num-1));
            	if(i == 0x0046) sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
            	else sprintf(jsCharts_temp,"ID_0x%04X:%u,",i,get_param_by_id(i,id_No-1));
                strcat(s_000, jsCharts_temp);
            }
            strcat(s_000, "}");
            s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
            s_000[strlen(s_000)-1]='\0';
            s_Rsc[0].size = strlen(s_000);
        break;
        case 17://DCDC信息
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);  //DCDC设备编号
            if(id_No < 1 || id_No > DCDC_NUM) id_No = 1;
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
                    "ID_0x0352:%d,"   //DCDC总数
            		"ID_0x038B:%d,"   //DCDC类型
            		"ID_0x02B2:%d,"   //DCDC通讯状态
                ,
                    get_param_by_id(0x0301,0),
                    get_param_by_id(0x0352,0),
                    get_param_by_id(0x038B,0),
                    get_param_by_id(0x02B2,id_No-1)
            );
            //DCDC信息
            if(get_param_by_id(0x038B,0) == 1)    //单向
            {
				for(i=0x0070; i<=0x0078; i++)
				{
					//sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,Cur_DCDC_Num-1));
					if(i == 0x0075) sprintf(jsCharts_temp,"ID_0x%04X:%u,",i,get_param_by_id(i,id_No-1));
					else sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
					strcat(s_000, jsCharts_temp);
				}
				memset(jsCharts_temp,0,1024);
				for(i=0x0085; i<=0x008C; i++)
				{
					//sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,Cur_DCDC_Num-1));
					if(i == 0x0089) sprintf(jsCharts_temp,"ID_0x%04X:%u,",i,get_param_by_id(i,id_No-1));
					else sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
					strcat(s_000, jsCharts_temp);
				}
				memset(jsCharts_temp,0,1024);
				for(i=0x0099; i<=0x00A0; i++)
				{
					//sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,Cur_DCDC_Num-1));
					if(i == 0x009D) sprintf(jsCharts_temp,"ID_0x%04X:%u,",i,get_param_by_id(i,id_No-1));
					else sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
					strcat(s_000, jsCharts_temp);
				}
				memset(jsCharts_temp,0,1024);
				for(i=0x00AD; i<=0x00B4; i++)
				{
					//sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,Cur_DCDC_Num-1));
					if(i == 0x00B1) sprintf(jsCharts_temp,"ID_0x%04X:%u,",i,get_param_by_id(i,id_No-1));
					else sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
					strcat(s_000, jsCharts_temp);
				}
            }
            else if(get_param_by_id(0x038B,0) == 2)		//双向
            {
				for(i=0x00F1; i<=0x00F8; i++)
				{
					sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
					strcat(s_000, jsCharts_temp);
				}
            	memset(jsCharts_temp,0,1024);
				for(i=0x0101; i<=0x0108; i++)
				{
					sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
					strcat(s_000, jsCharts_temp);
				}
            	memset(jsCharts_temp,0,1024);
				for(i=0x0111; i<=0x0118; i++)
				{
					sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
					strcat(s_000, jsCharts_temp);
				}
            	memset(jsCharts_temp,0,1024);
				for(i=0x0121; i<=0x0128; i++)
				{
					sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
					strcat(s_000, jsCharts_temp);
				}
            }
            strcat(s_000, "}");
            s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
            s_000[strlen(s_000)-1]='\0';
            s_Rsc[0].size = strlen(s_000);
        break;
        case 18://BMS信息
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);  //BMS设备编号
            if(id_No < 1 || id_No > BMS_NUM) id_No = 1;
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
            		"ID_0x02B3:%d,"   //BMS通讯状态
            		"ID_0x038A:%d,"
                    "ID_0x0354:%d,"   //BMS总数
                ,
                	get_param_by_id(0x0301,0),
                	get_param_by_id(0x02B3,id_No-1),
                    get_param_by_id(0x038A,0),
                    get_param_by_id(0x0354,id_No-1)
            );
			for(i=0x0050; i<=0x006A; i++)
			{
				if(i == 0x0057 || i == 0x0058){
					if(System_Info.Config.BMS_Type == 1){
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,(int8_t)(get_param_by_id(i,id_No-1)&0x00ff));
					}else if(System_Info.Config.BMS_Type == 2){
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,(int16_t)get_param_by_id(i,id_No-1));
					}else{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,0);
					}
				}else{
					sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,id_No-1));
				}
				strcat(s_000, jsCharts_temp);
			}
            strcat(s_000, "}");
            s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
            s_000[strlen(s_000)-1]='\0';
            s_Rsc[0].size = strlen(s_000);
        break;
        case 20://7777框架
        case 21://7777菜单
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0202:'%s',"  //设备名称
                    "ID_0x0301:%d"     //页面语言
                "}",
                    Device_Info[DEVICE_NAME],
                    get_param_by_id(0x0301,0)
            );
            s_Rsc[0].size = strlen(s_000);
        break;
        case 22://系统参数
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
                    "ID_0x0380:%d,"   //协议选择
                    "ID_0x0381:%d,"   //母线类型
                    "ID_0x0382:%d,"   //是否并网
                    "ID_0x0383:%d,"   //直流母线充电使能
                    "ID_0x0385:%d,"   //储能逆变器型号
                    "ID_0x0386:%d,"   //优先级选择
            		"ID_0x020B:%d"    //MAC锁定
                "}",
                    get_param_by_id(0x0301,0),
                    get_param_by_id(0x0380,0),
                    get_param_by_id(0x0381,0),
                    get_param_by_id(0x0382,0),
                    get_param_by_id(0x0383,0),
                    get_param_by_id(0x0385,0),
                    get_param_by_id(0x0386,0),
                    get_param_by_id(0x020B,0)
            );
            s_Rsc[0].size = strlen(s_000);
        break;
        case 23://系统参数
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No==0)  //读所有设备当前modbus地址
            {
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x0390:%d,"   //PCS从设备web界面是否显示
						"ID_0x0391:%d,"   //PV从设备web界面是否显示
						"ID_0x0392:%d,"   //DCDC从设备web界面是否显示
						"ID_0x0393:%d,"   //PVS从设备web界面是否显示
						"ID_0x0394:%d,"   //BMS从设备web界面是否显示
						"ID_0x0395:%d,"   //电表从设备web界面是否显示
						"ID_0x0396:%d,"   //实时监控界面是否显示
						"ID_0x0397:%d,"   //电池控制界面是否显示
						"ID_0x0398:%d,"
						"ID_0x0399:%d,"
						"ID_0x039A:%d,"   //环境仪界面是否显示
						"ID_0x0387:%d,"   //PV通讯模式
						"ID_0x0388:%d,"   //VSG模式
						"ID_0x0389:%d,"   //分时段控制使能
						"ID_0x038A:%d,"   //电池类型
						"ID_0x038B:%d,"   //DCDC类型
						"ID_0x038C:%d,"   //电表通讯使能
						"ID_0x038D:%d,"   //PCS通讯模式
						"ID_0x038E:%d,"   //配电使能
						"ID_0x0370:%d,"   //均衡使能
						"ID_0x0371:%d,"   //PCS离网启动策略
						"ID_0x0372:%d,"   //PCS波特率
						"ID_0x0373:%d,"   //PV波特率
						"ID_0x036F:%d"
					"}",
						get_param_by_id(0x0301,0),
						get_param_by_id(0x0390,0),
						get_param_by_id(0x0391,0),
						get_param_by_id(0x0392,0),
						get_param_by_id(0x0393,0),
						get_param_by_id(0x0394,0),
						get_param_by_id(0x0395,0),
						get_param_by_id(0x0396,0),
						get_param_by_id(0x0397,0),
						get_param_by_id(0x0398,0),
						get_param_by_id(0x0399,0),
						get_param_by_id(0x039A,0),
						get_param_by_id(0x0387,0),
						get_param_by_id(0x0388,0),
						get_param_by_id(0x0389,0),
						get_param_by_id(0x038A,0),
						get_param_by_id(0x038B,0),
						get_param_by_id(0x038C,0),
						get_param_by_id(0x038D,0),
						get_param_by_id(0x038E,0),
						get_param_by_id(0x0370,0),
						get_param_by_id(0x0371,0),
						get_param_by_id(0x0372,0),
						get_param_by_id(0x0373,0),
						get_param_by_id(0x036F,0)
				);
            }
            else
			{
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
				);
				while(strcmp(ptemp,"time"))
				{
					sprintf(jsCharts_temp,"ID_0x%04X:%d,",id_No,get_param_by_id(id_No,0));
					strcat(s_000, jsCharts_temp);
					ptemp = strtok(NULL,delim);
					id_No=(uint16_t)strtoul(ptemp,0,16);
				}
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
			}
            s_Rsc[0].size = strlen(s_000);
        break;
        case 24://系统参数
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No==0)  //读所有设备当前modbus地址
            {
                sprintf(s_000,
                    "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                    "{"
                        "ID_0x0301:%d,"   //页面语言
                		"ID_0x0360:%d,"   //最大逆流功率
                		"ID_0x0361:%d,"   //网口下发周期
                		"ID_0x0362:%d,"   //网口设置周期
                        "ID_0x0384:%d,"   //系统功率
                        "ID_0x0183:%d,"   //控制模式
                        "ID_0x0187:%d,"   //并网模式
                        "ID_0x0180:%d,"   //独立逆变电压
                        "ID_0x0181:%d,"   //独立逆变频率
                        "ID_0x0410:%d,"   //独立逆变电压下限
                        "ID_0x0411:%d,"   //独立逆变电压上限
                        "ID_0x0412:%d,"   //独立逆变频率下限
                        "ID_0x0413:%d,"   //独立逆变频率上限
                        "ID_0x019A:%d,"   //并网恒压模式放电目标电压
                        "ID_0x019B:%d,"   //并网恒压模式充电目标电压
                        "ID_0x0426:%d,"   //并网恒压模式放电目标电压下限
                        "ID_0x0427:%d,"   //并网恒压模式放电目标电压上限
                        "ID_0x0428:%d,"   //并网恒压模式充电目标电压下限
                        "ID_0x0429:%d,"    //并网恒压模式充电目标电压上限
                    ,
                        get_param_by_id(0x0301,0),
                        get_param_by_id(0x0360,0),
                        get_param_by_id(0x0361,0),
                        get_param_by_id(0x0362,0),
                        get_param_by_id(0x0384,0),
                        get_param_by_id(0x0183,0),
                        get_param_by_id(0x0187,0),
                        get_param_by_id(0x0180,0),
                        get_param_by_id(0x0181,0),
                        get_param_by_id(0x0410,0),
                        get_param_by_id(0x0411,0),
                        get_param_by_id(0x0412,0),
                        get_param_by_id(0x0413,0),
                        get_param_by_id(0x019A,0),
                        get_param_by_id(0x019B,0),
                        get_param_by_id(0x0426,0),
                        get_param_by_id(0x0427,0),
                        get_param_by_id(0x0428,0),
                        get_param_by_id(0x0429,0)
                );

                for(i=0; i<24; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x2000+i,get_param_by_id(0x2000+i,0));
                    strcat(s_000, jsCharts_temp);
                }
                strcat(s_000, "}");
                s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
                s_000[strlen(s_000)-1]='\0';
            }
            else
            {
                sprintf(s_000,
                    "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                    "{"
                );
                while(strcmp(ptemp,"time"))
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",id_No,get_param_by_id(id_No,0));
                    strcat(s_000, jsCharts_temp);
                    ptemp = strtok(NULL,delim);
                    id_No=(uint16_t)strtoul(ptemp,0,16);
                }
                strcat(s_000, "}");
                s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
                s_000[strlen(s_000)-1]='\0';
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 25://实时监控界面
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
                    "ID_0x02A0:%.2f,"
                    "ID_0x02A1:%.2f,"
                    "ID_0x02A2:%.2f,"
            		"ID_0x02A3:%.2f,"
            		"ID_0x02A4:%.2f,"
            		"ID_0x02A5:%.2f,"
            		"ID_0x02A6:%.2f,"
            		"ID_0x02A7:%.2f"
                "}",
                    get_param_by_id(0x0301,0),
                    get_realtimedata_by_id(0x02A0),
                    get_realtimedata_by_id(0x02A1),
                    get_realtimedata_by_id(0x02A2),
                    get_realtimedata_by_id(0x02A3),
                    get_realtimedata_by_id(0x02A4),
                    get_realtimedata_by_id(0x02A5),
                    get_realtimedata_by_id(0x02A6),
                    get_realtimedata_by_id(0x02A7)
            );
            s_Rsc[0].size = strlen(s_000);
        break;
        case 26://电表信息
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);  //电表设备编号
            if(id_No < 1 || id_No > Meter_NUM) id_No = 1;
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
                    "ID_0x0355:%d,"   //电表总数
                    "ID_0x00C1:%d,"   //当前电表序号
                    "ID_0x00C2:%d,"   //电表Vab
                    "ID_0x00C3:%d,"   //电表Vbc
                    "ID_0x00C4:%d,"   //电表Vac
                    "ID_0x00C5:%d,"   //电表有功功率
                    "ID_0x00C6:%d,"   //电表无功功率
                    "ID_0x00C7:%d,"   //电表功率因数
                    "ID_0x00C8:%d,"   //电表状态
            		"ID_0x02B4:%d"    //电表通讯状态
                "}",
                	get_param_by_id(0x0301,0),
                    get_param_by_id(0x0355,id_No-1),
                    get_param_by_id(0x00C1,id_No-1),
                    get_param_by_id(0x00C2,id_No-1),
                    get_param_by_id(0x00C3,id_No-1),
                    get_param_by_id(0x00C4,id_No-1),
                    get_param_by_id(0x00C5,id_No-1),
                    get_param_by_id(0x00C6,id_No-1),
                    get_param_by_id(0x00C7,id_No-1),
                    get_param_by_id(0x00C8,id_No-1),
                    get_param_by_id(0x02B4,id_No-1)
            );
            s_Rsc[0].size = strlen(s_000);
        break;
        case 27://设备地址配置界面
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No!=0xFFFF)  //读所有设备当前modbus地址
            {
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x0282:%d,"   //用户权限
						"ID_0x0387:%d,"   //PV通信方式 0--串口 1--网口
						"ID_0x038D:%d,"   //PCS通信方式 0--串口 1--网口
						"ID_0x0390:%d,"   //PCS从设备web界面是否显示
						"ID_0x0391:%d,"   //PV从设备web界面是否显示
						"ID_0x0392:%d,"   //DCDC从设备web界面是否显示
						"ID_0x0393:%d,"   //PVS从设备web界面是否显示
						"ID_0x0394:%d,"   //BMS从设备web界面是否显示
						"ID_0x0395:%d,"   //电表从设备web界面是否显示
						"ID_0x039A:%d,"   //环境仪设备web界面是否显示
						"ID_0x0350:%d,"   //PCS最大台数
						"ID_0x0351:%d,"   //PV最大台数
						"ID_0x0352:%d,"   //DCDC最大台数
						"ID_0x0353:%d,"   //PVS最大台数
						"ID_0x0354:%d,"   //BMS最大台数
						"ID_0x0355:%d,"   //电表最大台数
					,
						get_param_by_id(0x0301,0),
						Get_User_Authority_For_Web(ip),
						get_param_by_id(0x0387,0),
						get_param_by_id(0x038D,0),
						get_param_by_id(0x0390,0),
						get_param_by_id(0x0391,0),
						get_param_by_id(0x0392,0),
						get_param_by_id(0x0393,0),
						get_param_by_id(0x0394,0),
						get_param_by_id(0x0395,0),
						get_param_by_id(0x039A,0),
						get_param_by_id(0x0350,0),
						get_param_by_id(0x0351,0),
						get_param_by_id(0x0352,0),
						get_param_by_id(0x0353,0),
						get_param_by_id(0x0354,0),
						get_param_by_id(0x0355,0)
				);
				if(id_No == 0)
				{
					if(System_Info.Config.PCS_Comm_Type==0) //串口通信
					{
						for(i=0; i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
						{
							sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x640+i,get_param_by_id(0x640+i,0));
							strcat(s_000, jsCharts_temp);
						}
					}
					else    //网口通信
					{
						uint8_t num=0;
						if(EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number>10) num=10;
						else num=EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number;
						for(i=0; i<num; i++)
						{
							sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x640+i,get_param_by_id(0x640+i,0));
							strcat(s_000, jsCharts_temp);
						}
						memset(jsCharts_temp,0,1024);
						for(i=0; i<num; i++)
						{
							sprintf(jsCharts_temp,"ID_0x%04X:%d,ID_0x%04X:%d,ID_0x%04X:%d,ID_0x%04X:%d,ID_0x%04X:%d,",
							0x1000+i*5,get_param_by_id(0x1000+i*5,0),
							0x1000+i*5+1,get_param_by_id(0x1000+i*5+1,0),
							0x1000+i*5+2,get_param_by_id(0x1000+i*5+2,0),
							0x1000+i*5+3,get_param_by_id(0x1000+i*5+3,0),
							0x1000+i*5+4,get_param_by_id(0x1000+i*5+4,0));
							strcat(s_000, jsCharts_temp);
						}
					}
				}
				else if(id_No == 0x640)  //请求所有PCS 串口modbus地址
				{
					for(i=0; i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PCS_Number; i++)
					{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x640+i,get_param_by_id(0x640+i,0));
						strcat(s_000, jsCharts_temp);
					}
				}
				else if(id_No == 0x900)
				{
					//for(i=0; i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_ENV_Number; i++)
					for(i=0; i<1; i++)
					{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x900+i,get_param_by_id(0x900+i,0));
						strcat(s_000, jsCharts_temp);
					}
				}
				else if(id_No >= 0x1000 && id_No < 0x2000)  //请求PCS单页net参数
				{
					uint8_t index=0;
					index=(id_No-0x1000)/5;  //请求PCS起始编号
					for(i=0; i<10; i++)
					{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x640+index+i,get_param_by_id(0x640+index+i,0));
						strcat(s_000, jsCharts_temp);
					}
					memset(jsCharts_temp,0,1024);
					for(i=0; i<10; i++)
					{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,ID_0x%04X:%d,ID_0x%04X:%d,ID_0x%04X:%d,ID_0x%04X:%d,",
						id_No+i*5,get_param_by_id(id_No+i*5,0),
						id_No+i*5+1,get_param_by_id(id_No+i*5+1,0),
						id_No+i*5+2,get_param_by_id(id_No+i*5+2,0),
						id_No+i*5+3,get_param_by_id(id_No+i*5+3,0),
						id_No+i*5+4,get_param_by_id(id_No+i*5+4,0));
						strcat(s_000, jsCharts_temp);
					}
				}
				else if(id_No >= 0x3000 && id_No < 0x4000)  //请求PV单页net参数
				{
					uint8_t index=0;
					index=(id_No-0x3000)/5;  //请求PCS起始编号
					for(i=0; i<10; i++)
					{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x6B0+index+i,get_param_by_id(0x6B0+index+i,0));
						strcat(s_000, jsCharts_temp);
					}
					memset(jsCharts_temp,0,1024);
					for(i=0; i<10; i++)
					{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,ID_0x%04X:%d,ID_0x%04X:%d,ID_0x%04X:%d,ID_0x%04X:%d,",
						id_No+i*5,get_param_by_id(id_No+i*5,0),
						id_No+i*5+1,get_param_by_id(id_No+i*5+1,0),
						id_No+i*5+2,get_param_by_id(id_No+i*5+2,0),
						id_No+i*5+3,get_param_by_id(id_No+i*5+3,0),
						id_No+i*5+4,get_param_by_id(id_No+i*5+4,0));
						strcat(s_000, jsCharts_temp);
					}
				}
				else if(id_No == 0x6B0)  //请求所有PV串口modbus地址
				{
					for(i=0; i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PV_Number; i++)
					{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x6B0+i,get_param_by_id(0x6B0+i,0));
						strcat(s_000, jsCharts_temp);
					}
				}
				else if(id_No == 0x6F0)  //请求所有DCDC串口modbus地址
				{
					for(i=0; i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_DCDC_Number; i++)
					{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x6F0+i,get_param_by_id(0x6F0+i,0));
						strcat(s_000, jsCharts_temp);
					}
				}
				else if(id_No == 0x840)  //请求所有BMS串口modbus地址
				{
					for(i=0; i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_BMS_Number; i++)
					{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x840+i,get_param_by_id(0x840+i,0));
						strcat(s_000, jsCharts_temp);
					}
				}
				else if(id_No == 0x750)  //请求所有电表串口modbus地址
				{
					for(i=0; i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_Meter_Number; i++)
					{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x750+i,get_param_by_id(0x750+i,0));
						strcat(s_000, jsCharts_temp);
					}
				}
				else if(id_No == 0x760)  //请求所有电表串口modbus地址
				{
					for(i=0; i<EMS_Info.EMS4x.AGC_Info.SysParameter.Total_PVS_Number; i++)
					{
						sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x760+i,get_param_by_id(0x760+i,0));
						strcat(s_000, jsCharts_temp);
					}
				}
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
            }
            else
            {
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x0282:%d,"   //用户权限
						"ID_0x0387:%d,"   //PV通信方式 0--串口 1--网口
						"ID_0x038D:%d,"   //PCS通信方式 0--串口 1--网口
						"ID_0x0390:%d,"   //PCS从设备web界面是否显示
						"ID_0x0391:%d,"   //PV从设备web界面是否显示
						"ID_0x0392:%d,"   //DCDC从设备web界面是否显示
						"ID_0x0393:%d,"   //PVS从设备web界面是否显示
						"ID_0x0394:%d,"   //BMS从设备web界面是否显示
						"ID_0x0395:%d,"   //电表从设备web界面是否显示
						"ID_0x0350:%d,"   //PCS最大台数
						"ID_0x0351:%d,"   //PV最大台数
						"ID_0x0352:%d,"   //DCDC最大台数
						"ID_0x0353:%d,"   //PVS最大台数
						"ID_0x0354:%d,"   //BMS最大台数
						"ID_0x0355:%d,"   //电表最大台数
					,
						get_param_by_id(0x0301,0),
						Get_User_Authority_For_Web(ip),
						get_param_by_id(0x0387,0),
						get_param_by_id(0x038D,0),
						get_param_by_id(0x0390,0),
						get_param_by_id(0x0391,0),
						get_param_by_id(0x0392,0),
						get_param_by_id(0x0393,0),
						get_param_by_id(0x0394,0),
						get_param_by_id(0x0395,0),
						get_param_by_id(0x0350,0),
						get_param_by_id(0x0351,0),
						get_param_by_id(0x0352,0),
						get_param_by_id(0x0353,0),
						get_param_by_id(0x0354,0),
						get_param_by_id(0x0355,0)
				);
                while(strcmp(ptemp,"time"))
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",id_No,get_param_by_id(id_No,0));
                    strcat(s_000, jsCharts_temp);
                    ptemp = strtok(NULL,delim);
                    id_No=(uint16_t)strtoul(ptemp,0,16);
                }
                strcat(s_000, "}");
                s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
                s_000[strlen(s_000)-1]='\0';
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 28://保护参数配置界面
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No==0)  //读所有设备当前modbus地址
            {
                sprintf(s_000,
                    "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                    "{"
                        "ID_0x0301:%d,"   //页面语言
                        "ID_0x0282:%d,"   //用户权限
                    ,
                        get_param_by_id(0x0301,0),
                        Get_User_Authority_For_Web(ip)
                );

                for(i=0; i<12; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x600+i,get_param_by_id(0x600+i,0));
                    strcat(s_000, jsCharts_temp);
                }
                memset(jsCharts_temp,0,1024);
                for(i=0; i<12; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x610+i,get_param_by_id(0x610+i,0));
                    strcat(s_000, jsCharts_temp);
                }
                memset(jsCharts_temp,0,1024);
                for(i=0; i<44; i++)
                {
                    sprintf(jsCharts_temp,"ID_0x%04X:%d,",0x440+i,get_param_by_id(0x440+i,0));
                    strcat(s_000, jsCharts_temp);
                }
                strcat(s_000, "}");
                s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
                s_000[strlen(s_000)-1]='\0';
            }
            else
            {
                sprintf(s_000,
                    "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                    "{"
                        "ID_0x%04X:%d"   //页面语言
                    "}",
                        id_No,
                        get_param_by_id(id_No,0)
                );

            }

            s_Rsc[0].size = strlen(s_000);
        break;
        case 7://运行记录
        case 8://事件记录
        case 9://故障记录
        case 29://告警记录
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No==0)  //读所有设备当前modbus地址
            {
	            sprintf(s_000,
	                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
	                "{"
	                    "ID_0x0301:%d," //页面语言
	                    "ID_0x0390:%d," //PCS界面显示状态
	                    "ID_0x0391:%d," //PV界面显示状态
	                    "ID_0x0392:%d," //DCDC界面显示状态
	                    "ID_0x0393:%d," //PVS界面显示状态
	                    "ID_0x0394:%d," //BMS界面显示状态
	                    "ID_0x0395:%d," //电表界面显示状态
	                    "ID_0x0396:%d,"  //实时监控界面显示状态
	            		"ID_0x0397:%d,"
	            		"ID_0x0398:%d,"
	            		"ID_0x0399:%d,"
	            		"ID_0x039A:%d"  //环境仪界面显示状态
	                "}",
	                    get_param_by_id(0x0301,0),
	                    get_param_by_id(0x0390,0),
	                    get_param_by_id(0x0391,0),
	                    get_param_by_id(0x0392,0),
	                    get_param_by_id(0x0393,0),
	                    get_param_by_id(0x0394,0),
	                    get_param_by_id(0x0395,0),
	                    get_param_by_id(0x0396,0),
	                    get_param_by_id(0x0397,0),
	                    get_param_by_id(0x0398,0),
	                    get_param_by_id(0x0399,0),
	                    get_param_by_id(0x039A,0)
	            );
            }
            else
            {
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0300:%d"
					"}",
					get_param_by_id(0x0300,0)   //当前导出状态
				);
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 30://系统参数
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No==0)  //读所有设备当前modbus地址
            {
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03A0:%d,"
						"ID_0x03A1:%d,"
						"ID_0x03A2:%d,"
						"ID_0x03A3:%d,"
						"ID_0x03A4:%d,"
						"ID_0x03A5:%d,"
						"ID_0x03A6:%d,"
						"ID_0x03A7:%d,"
						"ID_0x03B0:%d,"
						"ID_0x0282:%d"
					"}",
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03A0,0),
						get_param_by_id(0x03A1,0),
						get_param_by_id(0x03A2,0),
						get_param_by_id(0x03A3,0),
						get_param_by_id(0x03A4,0),
						get_param_by_id(0x03A5,0),
						get_param_by_id(0x03A6,0),
						get_param_by_id(0x03A7,0),
						get_param_by_id(0x03B0,0),
						Get_User_Authority_For_Web(ip)
				);
            }
            else
			{
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
				);
				while(strcmp(ptemp,"time"))
				{
					sprintf(jsCharts_temp,"ID_0x%04X:%d,",id_No,get_param_by_id(id_No,0));
					strcat(s_000, jsCharts_temp);
					ptemp = strtok(NULL,delim);
					id_No=(uint16_t)strtoul(ptemp,0,16);
				}
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
			}
            s_Rsc[0].size = strlen(s_000);
        break;
        case 31://概要信息
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
            		"ID_0x0309:%d,"   //PCS图片滚动方向
                    "ID_0x0310:%d,"   //电网侧接触器状态
                    "ID_0x0311:%d,"   //微网侧接触器状态
                    "ID_0x0312:%d,"   //电网图片滚动方向
                    "ID_0x0313:%d,"   //微网图片滚动方向
                    "ID_0x0314:%d,"   //光伏逆变器图片滚动方向
                    "ID_0x0315:%d,"   //负载供电滚动方向
                    "ID_0x0316:%d,"   //是否交流母线系统
                    "ID_0x0317:%d,"   //电网电压
                    "ID_0x0318:%d,"   //负载电压
                    "ID_0x0319:%d,"   //负载有功功率
                    "ID_0x031A:%d,"   //储能交流电压
                    "ID_0x031B:%d,"   //储能有功功率
                    "ID_0x031C:%d,"   //光伏直流电压
                    "ID_0x031D:%d,"   //光伏有功功率
                    "ID_0x031E:%d,"   //SOC
                    "ID_0x031F:%d,"   //SOH
                    "ID_0x0320:%d,"   //储能直流侧电压
            		"ID_0x0322:%d,"   //电网功率
                    "ID_0x0381:%d,"   //母线类型
            		"ID_0x038F:%d"    //SOC使能状态
                "}",
                    get_param_by_id(0x0301,0),
                    get_param_by_id(0x0309,0),
                    get_param_by_id(0x0310,0),
                    get_param_by_id(0x0311,0),
                    get_param_by_id(0x0312,0),
                    get_param_by_id(0x0313,0),
                    get_param_by_id(0x0314,0),
                    get_param_by_id(0x0315,0),
                    get_param_by_id(0x0316,0),
                    get_param_by_id(0x0317,0),
                    get_param_by_id(0x0318,0),
                    get_param_by_id(0x0319,0),
                    get_param_by_id(0x031A,0),
                    get_param_by_id(0x031B,0),
                    get_param_by_id(0x031C,0),
                    get_param_by_id(0x031D,0),
                    get_param_by_id(0x031E,0),
                    get_param_by_id(0x031F,0),
                    get_param_by_id(0x0320,0),
                    get_param_by_id(0x0322,0),
                    get_param_by_id(0x0381,0),
                    get_param_by_id(0x038F,0)
            );
            s_Rsc[0].size = strlen(s_000);
        break;
        case 32: //分时功率
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No == 0)
            {
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
						"ID_0x0282:%d"   //用户
					"}",
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0),
						Get_User_Authority_For_Web(ip)
				);
            }
            else
            {
            	//id_No == 1 实时
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
					 ,
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0)
				);
				sprintf(jsCharts_temp,"D00:%d,", showTime.tm_year);
				strcat(s_000, jsCharts_temp);
				sprintf(jsCharts_temp,"D01:%d,", showTime.tm_mon);
				strcat(s_000, jsCharts_temp);
				sprintf(jsCharts_temp,"D02:%d,", showTime.tm_day);
				strcat(s_000, jsCharts_temp);
				memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	                sprintf(jsCharts_temp,"A%02d:%.1f,",i, (float)PV_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"B%02d:%.1f,",i, (float)Grid_Power_Curve_Buffer[i]/-10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"C%02d:%.1f,",i, (float)PCS_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 33: //削峰填谷
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No == 0)
            {
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
						"ID_0x0282:%d"   //用户
					"}",
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0),
						Get_User_Authority_For_Web(ip)
				);
            }
            else
            {
            	//id_No == 1 实时
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
					 ,
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0)
				);
				sprintf(jsCharts_temp,"D00:%d,", showTime.tm_year);
				strcat(s_000, jsCharts_temp);
				sprintf(jsCharts_temp,"D01:%d,", showTime.tm_mon);
				strcat(s_000, jsCharts_temp);
				sprintf(jsCharts_temp,"D02:%d,", showTime.tm_day);
				strcat(s_000, jsCharts_temp);
				memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	                sprintf(jsCharts_temp,"A%02d:%.1f,",i, (float)Load_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"B%02d:%.1f,",i, (float)Grid_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"C%02d:%.1f,",i, (float)PCS_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 34: // 交流限发
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No == 0)
            {
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
						"ID_0x0282:%d"   //用户
					"}",
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0),
						Get_User_Authority_For_Web(ip)
				);
            }
            else
            {
            	//id_No == 1 实时
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
					 ,
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0)
				);
				sprintf(jsCharts_temp,"D00:%d,", showTime.tm_year);
				strcat(s_000, jsCharts_temp);
				sprintf(jsCharts_temp,"D01:%d,", showTime.tm_mon);
				strcat(s_000, jsCharts_temp);
				sprintf(jsCharts_temp,"D02:%d,", showTime.tm_day);
				strcat(s_000, jsCharts_temp);
				memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	                sprintf(jsCharts_temp,"A%02d:%.1f,",i, (float)PV_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"B%02d:%.1f,",i, (float)Grid_Power_Curve_Buffer[i]/-10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"C%02d:%.1f,",i, (float)PCS_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 35: //平滑
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No == 0)
            {
            	uint32_t i=0, j=0, t=0;
            	i = showTime.tm_hour*3600+showTime.tm_min*60+showTime.tm_sec;
            	j = i / 60;    //1min记一次
            	if(j >= 1440) j = 0;
            	//总共需要传送的分段数
            	if(j <= 96){
            		t = 1;
            	}else{
            		if((j%96)!=0) t=j/96+1;
            		else t = j/96;
            	}
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
						"ID_0x0282:%d,"   //用户
						"D00:%d,"
						"D01:%d,"
						"D02:%d,"
						"T:%d"			  //请求的总分段数
					"}",
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0),
						Get_User_Authority_For_Web(ip),
						showTime.tm_year,
						showTime.tm_mon,
						showTime.tm_day,
						t
				);
            }
            else if(id_No == 2)    //获取实时点数据
            {
            	uint32_t i=0, j=0;
            	i = showTime.tm_hour*3600+showTime.tm_min*60+showTime.tm_sec;
            	j = i / 60;    //1min记一次
            	if(j>=1440) j=0;
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"Index:%d,"   //点的位置
						"Pa:%.1f,"
						"Pb:%.1f,"
						"Pc:%.1f"
					"}"
					 ,
						j,
						(float)PV_Power_Curve_Buffer[j]/10.0,
						(float)Grid_Power_Curve_Buffer[j]/-10.0,
						(float)PCS_Power_Curve_Buffer[j]/10.0
				);
            }
            else if(id_No == 1)    //获取所有实时数据, 分段 96 x 15
            {
                ptemp = strtok(NULL,delim);
                ptemp = strtok(NULL,delim);
                part=(uint16_t)strtoul(ptemp,0,16);
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
						"P:%d,"
					 ,
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0),
						part
				);
	            for(i=0; i<96; i++)
	            {
	                sprintf(jsCharts_temp,"A%d:%.1f,",i, (float)PV_Power_Curve_Buffer[96*part+i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"B%d:%.1f,",i, (float)Grid_Power_Curve_Buffer[96*part+i]/-10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"C%d:%.1f,",i, (float)PCS_Power_Curve_Buffer[96*part+i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 36: //调频
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No == 0)
            {
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
						"ID_0x0282:%d"   //用户
					"}",
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0),
						Get_User_Authority_For_Web(ip)
				);
            }
            else
            {
            	//id_No == 1 实时
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
					 ,
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0)
				);
				sprintf(jsCharts_temp,"D00:%d,", showTime.tm_year);
				strcat(s_000, jsCharts_temp);
				sprintf(jsCharts_temp,"D01:%d,", showTime.tm_mon);
				strcat(s_000, jsCharts_temp);
				sprintf(jsCharts_temp,"D02:%d,", showTime.tm_day);
				strcat(s_000, jsCharts_temp);
				memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	                sprintf(jsCharts_temp,"A%02d:%.1f,",i, (float)PV_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"B%02d:%.1f,",i, (float)Grid_Power_Curve_Buffer[i]/-10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"C%02d:%.1f,",i, (float)PCS_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 37://高级功能
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No==0)  //读所有设备当前modbus地址
            {
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x0282:%d,"   //用户
						"ID_0x03B1:%d,"   //消峰填谷使能
						"ID_0x03B5:%d,"   //交流限发
						"ID_0x03B8:%d,"	  //输出平滑
						"ID_0x03C0:%d"    //调频
					"}",
						get_param_by_id(0x0301,0),
						Get_User_Authority_For_Web(ip),
						get_param_by_id(0x03B1,0),
						get_param_by_id(0x03B5,0),
						get_param_by_id(0x03B8,0),
						get_param_by_id(0x03C0,0)
				);
            }
            else
			{
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
				);
				while(strcmp(ptemp,"time"))
				{
					sprintf(jsCharts_temp,"ID_0x%04X:%d,",id_No,get_param_by_id(id_No,0));
					strcat(s_000, jsCharts_temp);
					ptemp = strtok(NULL,delim);
					id_No=(uint16_t)strtoul(ptemp,0,16);
				}
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
			}
            s_Rsc[0].size = strlen(s_000);
        break;
        case 38:
			sprintf(s_000,
				"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
				"{"
					"\"ID_0x0301\":\"%d\","   //页面语言
					"\"ID_0x0152\":\"%d\","   //故障状态字1
					"\"ID_0x0153\":\"%d\""    //故障状态字2
				"}",
					get_param_by_id(0x0301,0),
					get_param_by_id(0x0152,0),
					get_param_by_id(0x0153,0)
			);
			s_Rsc[0].size = strlen(s_000);
		break;
        case 39:
			sprintf(s_000,
				"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
				"{"
					"\"ID_0x0301\":\"%d\","   //页面语言
					"\"ID_0x0154\":\"%d\""    //告警状态
				"}",
					get_param_by_id(0x0301,0),
					get_param_by_id(0x0154,0)
			);
			s_Rsc[0].size = strlen(s_000);
		break;
        case 41:	//状态栏
			sprintf(s_000,
				"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
				"{"
					"ID_0x0301:%d,"   //页面语言
					"ID_0x0285:%.1f,"
					"ID_0x0286:%d,"
					"ID_0x0287:'%d / %d',"
					"ID_0x0288:'%04d/%02d/%02d %02d:%02d'"
				"}",
					get_param_by_id(0x0301,0),
					ctrl.netServer.Get_Used_CPU(),
					ctrl.netServer.Get_Free_MEM(),
					Get_Logined_User_Num(),
					MAX_SRV_USER,
					showTime.tm_year+2000,
					showTime.tm_mon,
					showTime.tm_day,
					showTime.tm_hour,
					showTime.tm_min
			);
			s_Rsc[0].size = strlen(s_000);
        break;
        case 42: // 微网曲线
            ptemp = strtok(data,delim);
            ptemp = strtok(NULL,delim);
            id_No=(uint16_t)strtoul(ptemp,0,16);
            if(id_No == 0)
            {
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
						"ID_0x0282:%d"   //用户
					"}",
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0),
						Get_User_Authority_For_Web(ip)
				);
            }
            else
            {
            	//id_No == 1 实时
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x03B4:%d,"
					 ,
						get_param_by_id(0x0301,0),
						get_param_by_id(0x03B4,0)
				);
				sprintf(jsCharts_temp,"D00:%d,", showTime.tm_year);
				strcat(s_000, jsCharts_temp);
				sprintf(jsCharts_temp,"D01:%d,", showTime.tm_mon);
				strcat(s_000, jsCharts_temp);
				sprintf(jsCharts_temp,"D02:%d,", showTime.tm_day);
				strcat(s_000, jsCharts_temp);
				memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	                sprintf(jsCharts_temp,"A%02d:%.1f,",i, (float)PV_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"B%02d:%.1f,",i, (float)Grid_Power_Curve_Buffer[i]/-10.0);
	            	//sprintf(jsCharts_temp,"B%02d:%.1f,",i, (float)Load_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<96; i++)
	            {
	            	sprintf(jsCharts_temp,"C%02d:%.1f,",i, (float)PCS_Power_Curve_Buffer[i]/10.0);
	                strcat(s_000, jsCharts_temp);
	            }
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
            }
            s_Rsc[0].size = strlen(s_000);
        break;
        case 43:  //详细信息
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
            		"ID_0x0381:%d,"   //母线类型
            		"ID_0x0188:%d,"   //启动类型    0-主动模式；1-被动模式
                    "ID_0x0319:%d,"   //负载有功功率
            		"ID_0x0386:%d,"   //启动模式    0-电网优先；1-微网优先
            		"ID_0x0321:%d,"   //负载无功功率
            		"ID_0x014F:%d,"   //系统状态
            		"ID_0x0005:%d,"   //PCS总有功功率
            		"ID_0x0004:%d,"   //电网状态
            		"ID_0x0006:%d,"   //PCS总无功功率
            		"ID_0x018B:%d,"   //并网工作模式 0-固定功率, 1：跟踪负载功率, 2：PCC点功率控制, 3：上位机调度  ...
            		"ID_0x0008:%d,"   //光伏总有功功率
            		"ID_0x0016:%d,"   //微网电压
            		"ID_0x0009:%d,"   //光伏总无功功率
            		"ID_0x0017:%d,"   //微网频率
            		"ID_0x0012:%d,"   //PCS在线台数
            		"ID_0x0013:%d,"   //PV在线台数
                ,
                    get_param_by_id(0x0301,0),
                    get_param_by_id(0x0381,0),
                    get_param_by_id(0x0188,0),
                    get_param_by_id(0x0319,0),
                    get_param_by_id(0x0386,0),
                    get_param_by_id(0x0321,0),
                    get_param_by_id(0x014F,0),
                    (int32_t)get_param_by_id(0x0005,0),
                    get_param_by_id(0x0004,0),
                    (int32_t)get_param_by_id(0x0006,0),
                    get_param_by_id(0x018B,0),
                    (int32_t)get_param_by_id(0x0008,0),
                    get_param_by_id(0x0016,0),
                    (int32_t)get_param_by_id(0x0009,0),
                    get_param_by_id(0x0017,0),
                    get_param_by_id(0x0012,0),
                    get_param_by_id(0x0013,0)
            );
            for(i=0x0330; i<=0x00345; i++)
            {
            	sprintf(jsCharts_temp,"ID_0x%04X:%u,",i,get_param_by_id(i,0));
                strcat(s_000, jsCharts_temp);
            }
            strcat(s_000, "}");
            s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
            s_000[strlen(s_000)-1]='\0';
            s_Rsc[0].size = strlen(s_000);
        break;
        case 44:
        	ptemp = strtok(data,delim);
			ptemp = strtok(NULL,delim);
			id_No=(uint16_t)strtoul(ptemp,0,16);
			if(id_No==0)  //读所有设备当前modbus地址
			{
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
						"ID_0x0301:%d,"   //页面语言
						"ID_0x0470:%d,"
						"ID_0x0471:%d,"
						"ID_0x0472:%d,"
						"ID_0x0750:%d,"
						"ID_0x0751:%d,"
						"ID_0x0752:%d,"
					,
						get_param_by_id(0x0301,0),
						get_param_by_id(0x0470,0),
						get_param_by_id(0x0471,0),
						get_param_by_id(0x0472,0),
						get_param_by_id(0x0750,0),
						get_param_by_id(0x0751,0),
						get_param_by_id(0x0752,0)
				);
	            memset(jsCharts_temp,0,1024);
	            for(i=0; i<16; i++)
	            {
	            	sprintf(jsCharts_temp,"ID_0x%04X:%d,", 0x0490+i, get_param_by_id(0x0490+i,0));
	                strcat(s_000, jsCharts_temp);
	            }
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
			}
			else
			{
				sprintf(s_000,
					"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
					"{"
				);
				while(strcmp(ptemp,"time"))
				{
					sprintf(jsCharts_temp,"ID_0x%04X:%d,",id_No,get_param_by_id(id_No,0));
					strcat(s_000, jsCharts_temp);
					ptemp = strtok(NULL,delim);
					id_No=(uint16_t)strtoul(ptemp,0,16);
				}
				strcat(s_000, "}");
				s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
				s_000[strlen(s_000)-1]='\0';
			}
			s_Rsc[0].size = strlen(s_000);
		break;
        case 45://ENV信息
            sprintf(s_000,
                "HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
                "{"
                    "ID_0x0301:%d,"   //页面语言
            		"ID_0x02B6:%d,"   //环境仪通讯状态
                ,
                    get_param_by_id(0x0301,0),
                    get_param_by_id(0x02B6,0)
            );
            //ENV信息
            for(i=0x8000; i<=0x800D; i++)
            {
            	if(i == 0x8001 || i == 0x8002 || i == 0x8004 || i == 0x8005) sprintf(jsCharts_temp,"ID_0x%04X:%u,",i,get_param_by_id(i,0));
            	else sprintf(jsCharts_temp,"ID_0x%04X:%d,",i,get_param_by_id(i,0));
                strcat(s_000, jsCharts_temp);
            }
            strcat(s_000, "}");
            s_000[strlen(s_000)-2]='}'; //去掉最后一个','号
            s_000[strlen(s_000)-1]='\0';
            s_Rsc[0].size = strlen(s_000);
        break;
        default:
        break;
    }
}
void a_handler(int id,char* data,char *ip)                 				//处理返回的参数，并回应
{
    char *ptemp;//切割出的内容
    char delim[] = "?=&+";//切割字符串
    uint32_t id_No=~0;//参数序号
    int32_t value=~0;//参数值
    uint8_t set_num=0;

    ptemp = strtok(data,delim);
    ptemp = strtok(NULL,delim);
    while(strcmp(ptemp,"time"))
    {
		id_No=(uint16_t)strtoul(ptemp,0,16);
		ptemp = strtok(NULL,delim);
		value=(int32_t)atoi(ptemp);
		if(id_No == 0x0281 && value == 1){
			Logout_User(ip);
		}else{
			set_param(set_num++,id_No,value);
		}
        ptemp = strtok(NULL,delim);
    }
    if(id == 1){   //index.html 需要返回0
        sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",0);
    }else{
    	sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",1);
    }
    a_Rsc[0].size = strlen(a_000);
}
bool Leap_Year(int year)                                        //是否为闰年
{
    bool rtn = false;
    if ((year % 4 == 0 && year % 100 != 0 )|| year % 400 == 0)
    {
        rtn = true;
    }
    else
    {
        rtn = false;
    }
    return rtn;
}
uint8_t How_Many_Day(int year,int month)                        //计算月发电量柱状图应该显示的天数
{
    uint8_t day=28;
    if(month==2)
    {
        if(Leap_Year(year)) day=29;
        else day=28;
    }
    else if(month==1 || month==3 || month==5 || month==7 || month==8 || month==10 || month==12)
        day=31;
    else if(month==4 || month==6 || month==9 || month==11)
        day=30;
    return day;
}

static uint8_t run_buf[EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN] = {0}; //超过linux默认堆栈大小8196,改成全局变量
void r_handler(int id,char* data)                               //导出历史记录（运行记录、故障记录）
{
    //uint8_t run_buf[EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN] = {0}; //超过linux默认堆栈大小8196,改成全局变量
    int32_t records_num=0;//记录的条数
    LogTime time;
    char *ptemp;
    char delim[] = "?=&+";
    int32_t *p=NULL;
    uint16_t page = 0;
    uint8_t array[16];
    ptemp = strtok(data,delim);
    ptemp = strtok(NULL,delim);
    ptemp = strtok(NULL,delim);
    time.Year = ((uint8_t)(atoi(ptemp)-2000));
    ptemp = strtok(NULL,delim);
    ptemp = strtok(NULL,delim);
    time.Month = ((uint8_t)atoi(ptemp));
    ptemp = strtok(NULL,delim);
    ptemp = strtok(NULL,delim);
    time.Date = ((uint8_t)atoi(ptemp));
    ptemp = strtok(NULL,delim);
    ptemp = strtok(NULL,delim);
    page = atoi(ptemp);                 //对于高级曲线，输出平滑对应0~14个数据段，其他曲线不判断
    memset(jsCharts_temp,0,sizeof(jsCharts_temp));
    memset(p_000,0,sizeof(p_000));
    switch(id)
    {
        case 7://运行记录
            sprintf(p_000,"HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\n\r\n");
            records_num = start_query(E_RUNRECORD_LOG, &time, 1, run_buf, EVERY_RUNRECORD_BUF_LEN);
            if(records_num<=0)records_num=0;
            sprintf(jsCharts_temp,"{'R':[");
            strcat(p_000,jsCharts_temp);
            for(int i = (page-1)*10+1;page >= 1 && i<=(((page * 10) > records_num) ? records_num : (page * 10));i++)
            {
                query_rcd(i, run_buf, EVERY_RUNRECORD_BUF_LEN);
                sprintf(jsCharts_temp,"{");
                strcat(p_000,jsCharts_temp);
                //读失败就放前一条
                sprintf(jsCharts_temp,
                        "'C1':%d,"
                        "'C2':'%4d.%02d.%02d/%02d:%02d:00',"
                        "'C3':%d,"					//储能型号
                        "'C4':'%.1fkW',"			//系统额定功率
                        "'C5':%d," 					//并网使能
                        "'C6':%d,"					//总线类型
                        "'C7':%d,"					//分时使能
                        "'C8':%d,"                  //工作状态
                        "'C9':%d,"             		//电网状态
                        "'C10':'%.1fV',"            //电网电压
                        "'C11':'%.2fHz',"           //电网频率
                        "'C12':'%.1fkW',"			//电网功率
                        "'C13':%d,"             	//电网接触器状态
                        "'C14':%d,"             	//电网接触器故障状态
                        "'C15':%d,"             	//微网接触器状态
                        "'C16':%d,"            		//微网接触器故障状态
                        "'C17':%d,"      			//运行模式
                        "'C18':%d,"					//功率控制模式
                        "'C19':'%.1fkW',"			//运行功率
                        "'C20':%d,"					//储能并网工作模式
                        "'C21':'%.1fkW',"           //储能逆变器有功功率
                        "'C22':'%.1fV',"            //微网电压
                        "'C23':'%.2fHz',"           //微网频率
                        "'C24':'%.1fkW',"      		//光伏逆变器功率
                        "'C25':'%.1fV',"            //光伏逆变器电压
                        "'C26':'%.1fkW',"          	//负载功率
                        "'C27':'%.1fV',"          	//负载电压
                        "'C28':'%.1fV',"            //电池电压
                		"'C29':'%.1fV',"            //电池电压
                		"'C30':'%.1f%%',"          	//电池SOC
                        "'C31':'%.1f%%'",          	//电池SOC
                        i,
                        (run_buf[2])+2000,(run_buf[3]),(run_buf[4]),(run_buf[5]),(run_buf[6]),
                        GetU16(run_buf+8), 			//储能型号	  2B
                        (float)GetU16(run_buf+10),  //系统额定功率  2B
                        GetU16(run_buf+12), 		//并网使能	  2B
                        GetU16(run_buf+14),			//总线类型     2B
                        GetU16(run_buf+16),			//分时使能     2B
                        GetU16(run_buf+18),         //工作状态      2B
                        GetU16(run_buf+20),    		//电网状态      2B
                        GetU16(run_buf+22)/10.0,    //电网电压      2B
                        GetU16(run_buf+24)/100.0,  	//电网频率      2B
                        (int16_t)GetU16(run_buf+26)/10.0,    //电网功率	  2B
                        GetU16(run_buf+28),    		//电网接触器状态      2B
                        GetU16(run_buf+30),    		//电网接触器故障状态   2B
                        GetU16(run_buf+32),    		//微网接触器状态      2B
                        GetU16(run_buf+34),    		//微网接触器故障状态   2B
                        GetU16(run_buf+36),			//运行模式		2B
                        GetU16(run_buf+38),			//功率控制模式		2B
                        (int16_t)GetU16(run_buf+40)/10.0,	//运行功率		2B
                        GetU16(run_buf+42),			//储能并网功率模式	2B
                        (int32_t)GetU32(run_buf+44)/10.0,	//储能逆变器有功功率	4B
                        GetU16(run_buf+48)/10.0,	//微网电压	2B
                        GetU16(run_buf+50)/100.0,   //微网频率	2B
                        GetU32(run_buf+52)/10.0,   	//光伏逆变器功率     4B
                        GetU16(run_buf+56)/10.0,    //光伏逆变器电压     2B
                        (int32_t)GetU32(run_buf+58)/10.0,    //负载功率   		  4B
                        GetU16(run_buf+62)/10.0,    //负载电压    	  2B
                        GetU16(run_buf+64)/10.0,    //电池电压      2B
                        GetU16(run_buf+66)/10.0,  	//电池SOC      2B
                        GetU16(run_buf+68)/10.0,  	//电池SOC      2B
                        GetU16(run_buf+70)/10.0  	//电池SOC      2B
                );
                strcat(p_000,jsCharts_temp);
                sprintf(jsCharts_temp,"},");
                strcat(p_000,jsCharts_temp);
            }
            sprintf(jsCharts_temp,"{'end':0}]};Records=%d;",records_num);
            strcat(p_000,jsCharts_temp);
            p_Rsc[0].size = strlen(p_000);
            end_query();
            break;
        case 8://事件记录
            sprintf(p_000,"HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\n\r\n");
            sprintf(jsCharts_temp,"{'r':[");
            strcat(p_000,jsCharts_temp);

            for(int i=(page-1)*10+1; page >= 1 && i<=(((page*10)>read_event_record.total_nub)?read_event_record.total_nub:(page*10));i++)
            {
                Read_OnConditionNB_Record(read_event_record.ptr, read_event_record.total_nub,i,HISEVENTRECORD_START_ADDRESS,array) ;
                sprintf(jsCharts_temp,"{");
                strcat(p_000,jsCharts_temp);
                sprintf(jsCharts_temp,
                "'C1':%d,"
                "'C2':'%4d/%02d/%02d&nbsp;&nbsp;%02d:%02d:%02d',"
                "'C3':'%d'",
                i,
                (array[0])+2000,
                (array[1]),
                (array[2]),
                (array[3]),
                (array[4]),
                (array[5]),
                array[6]);
                strcat(p_000,jsCharts_temp);
                sprintf(jsCharts_temp,"},");
                strcat(p_000,jsCharts_temp);
            }
            sprintf(jsCharts_temp,"{'end':0}]};Records=%d;",read_event_record.total_nub);
            strcat(p_000,jsCharts_temp);
            p_Rsc[0].size = strlen(p_000);
        break;
        case 9://故障记录
            sprintf(p_000,"HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\n\r\n");
            sprintf(jsCharts_temp,"{'r':[");
            strcat(p_000,jsCharts_temp);

            for(int i=(page-1)*10+1; page >= 1 && i<=(((page*10)>read_fault_record.total_nub)?read_fault_record.total_nub:(page*10));i++)
            {
                Read_OnConditionNB_Record(read_fault_record.ptr, read_fault_record.total_nub,i,HISFAULTRECORD_START_ADDRESS,array) ;
                sprintf(jsCharts_temp,"{");
                strcat(p_000,jsCharts_temp);
                sprintf(jsCharts_temp,
                "'C1':%d,"
                "'C2':'%4d/%02d/%02d&nbsp;&nbsp;%02d:%02d:%02d',"
                "'C3':'%d'",
                i,
                (array[0])+2000,
                (array[1]),
                (array[2]),
                (array[3]),
                (array[4]),
                (array[5]),
                array[6]);
                strcat(p_000,jsCharts_temp);
                sprintf(jsCharts_temp,"},");
                strcat(p_000,jsCharts_temp);
            }
            sprintf(jsCharts_temp,"{'end':0}]};Records=%d;",read_fault_record.total_nub);
            strcat(p_000,jsCharts_temp);
            p_Rsc[0].size = strlen(p_000);
        break;
        case 46://复位记录
			sprintf(p_000,"HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\n\r\n");
			sprintf(jsCharts_temp,"{'r':[");
			strcat(p_000,jsCharts_temp);

			for(int i=(page-1)*10+1; page >= 1 && i<=(((page*10)>read_reset_record.total_nub)?read_reset_record.total_nub:(page*10));i++)
			{
				Read_OnConditionNB_Record(read_reset_record.ptr, read_reset_record.total_nub,i,HISRESETRECORD_START_ADDRESS,array) ;
				sprintf(jsCharts_temp,"{");
				strcat(p_000,jsCharts_temp);
				sprintf(jsCharts_temp,
				"'C1':%d,"
				"'C2':'%4d/%02d/%02d&nbsp;&nbsp;%02d:%02d:%02d',"
				"'C3':'%d'",
				i,
				(array[0])+2000,
				(array[1]),
				(array[2]),
				(array[3]),
				(array[4]),
				(array[5]),
				array[6]);
				strcat(p_000,jsCharts_temp);
				sprintf(jsCharts_temp,"},");
				strcat(p_000,jsCharts_temp);
			}
			sprintf(jsCharts_temp,"{'end':0}]};Records=%d;",read_reset_record.total_nub);
			strcat(p_000,jsCharts_temp);
			p_Rsc[0].size = strlen(p_000);
		break;
        case 29://告警记录
            sprintf(p_000,"HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\n\r\n");
			sprintf(jsCharts_temp,"{'r':[");
			strcat(p_000,jsCharts_temp);

			for(int i=(page-1)*10+1; page >= 1 && i<=(((page*10)>read_alarm_record.total_nub)?read_alarm_record.total_nub:(page*10));i++)
			{
				Read_OnConditionNB_Record(read_alarm_record.ptr, read_alarm_record.total_nub,i,HISALARMRECORD_START_ADDRESS,array) ;
				sprintf(jsCharts_temp,"{");
				strcat(p_000,jsCharts_temp);
				sprintf(jsCharts_temp,
				"'C1':%d,"
				"'C2':'%4d/%02d/%02d&nbsp;&nbsp;%02d:%02d:%02d',"
				"'C3':'%d'",
				i,
				(array[0])+2000,
				(array[1]),
				(array[2]),
				(array[3]),
				(array[4]),
				(array[5]),
				array[6]);
				strcat(p_000,jsCharts_temp);
				sprintf(jsCharts_temp,"},");
				strcat(p_000,jsCharts_temp);
			}
			sprintf(jsCharts_temp,"{'end':0}]};Records=%d;",read_alarm_record.total_nub);
			strcat(p_000,jsCharts_temp);
			p_Rsc[0].size = strlen(p_000);
		break;
        case 32: //分时电价
			sprintf(p_000,
				"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
				"{"
					"ID_0x0301:%d,"   //页面语言
					"ID_0x03B4:%d,"
				 ,
				 get_param_by_id(0x0301,0),
				 get_param_by_id(0x03B4,0)
			);
            records_num = start_query(E_TIME_OF_USE_CURVE_LOG, &time, 1, run_buf, EVERY_DAY_TIME_OF_USE_CURVE_BUF_LEN);
            //printf("time of use record num %d\n", records_num);
            memset(run_buf, 0x00, EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN);
            if(records_num==1)
            {
            	query_rcd(1, run_buf, EVERY_DAY_TIME_OF_USE_CURVE_BUF_LEN);
            }
			for(int i=0; i<3; i++)
			{
				sprintf(jsCharts_temp,"D%02d:%d,",i, run_buf[2+i]);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"A%02d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[96*4+8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"B%02d:%.1f,",i, (float)p[i]/-10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[96*8+8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"C%02d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			p = NULL;
			strcat(p_000, "}");
			p_000[strlen(p_000)-2]='}'; //去掉最后一个','号
			p_000[strlen(p_000)-1]='\0';

			p_Rsc[0].size = strlen(p_000);
            end_query();
        break;
        case 33: //消峰填谷
        	sprintf(p_000,
				"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
				"{"
					"ID_0x0301:%d,"   //页面语言
					"ID_0x03B4:%d,"
				 ,
				 get_param_by_id(0x0301,0),
				 get_param_by_id(0x03B4,0)
			);
			records_num = start_query(E_PEAKVALLEY_CURVE_LOG, &time, 1, run_buf, EVERY_DAY_PEAKVALLEY_CURVE_BUF_LEN);
			//printf("peak record num %d\n", records_num);
			memset(run_buf, 0x00, EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN);
			if(records_num==1)
			{
				query_rcd(1, run_buf, EVERY_DAY_PEAKVALLEY_CURVE_BUF_LEN);
			}
			for(int i=0; i<3; i++)
			{
				sprintf(jsCharts_temp,"D%02d:%d,",i, run_buf[2+i]);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"A%02d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[96*4+8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"B%02d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[96*8+8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"C%02d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			strcat(p_000, "}");
			p_000[strlen(p_000)-2]='}'; //去掉最后一个','号
			p_000[strlen(p_000)-1]='\0';

			p_Rsc[0].size = strlen(p_000);
			end_query();
        break;
        case 34: //交流限发
        	sprintf(p_000,
				"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
				"{"
					"ID_0x0301:%d,"   //页面语言
					"ID_0x03B4:%d,"
				 ,
				 get_param_by_id(0x0301,0),
				 get_param_by_id(0x03B4,0)
			);
			records_num = start_query(E_AC_LIMITED_PRODUCE_CURVE_LOG, &time, 1, run_buf, EVERY_DAY_ACLIMIT_CURVE_BUF_LEN);
			//printf("ac record num %d\n", records_num);
			memset(run_buf, 0x00, EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN);
			if(records_num==1)
			{
				query_rcd(1, run_buf, EVERY_DAY_ACLIMIT_CURVE_BUF_LEN);
			}
			for(int i=0; i<3; i++)
			{
				sprintf(jsCharts_temp,"D%02d:%d,",i, run_buf[2+i]);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"A%02d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[96*4+8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"B%02d:%.1f,",i, (float)p[i]/-10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[96*8+8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"C%02d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			strcat(p_000, "}");
			p_000[strlen(p_000)-2]='}'; //去掉最后一个','号
			p_000[strlen(p_000)-1]='\0';

			p_Rsc[0].size = strlen(p_000);
			end_query();
        break;
        case 35: //输出平滑
			sprintf(p_000,
				"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
				"{"
					"ID_0x0301:%d,"   //页面语言
					"ID_0x03B4:%d,"
					"P:%d,"
				 ,
				 get_param_by_id(0x0301,0),
				 get_param_by_id(0x03B4,0),
					page
			);
			if(page==0)  //第一个分段读历史数据，后面分段直接发送
			{
				records_num = start_query(E_OUTPUT_SMOOTH_CURVE_LOG, &time, 1, run_buf, EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN);
				//printf("smooth record num %d\n", records_num);
				memset(run_buf, 0x00, EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN);
				if(records_num==1)
				{
					query_rcd(1, run_buf, EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN);
				}
			}
			for(int i=0; i<3; i++)
			{
				sprintf(jsCharts_temp,"D%02d:%d,",i, run_buf[2+i]);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[8+96*4*page];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"A%d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[1440*4+8+96*4*page];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"B%d:%.1f,",i, (float)p[i]/-10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[1440*8+8+96*4*page];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"C%d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			strcat(p_000, "}");
			p_000[strlen(p_000)-2]='}'; //去掉最后一个','号
			p_000[strlen(p_000)-1]='\0';

			p_Rsc[0].size = strlen(p_000);
			end_query();
        break;
        case 36: //调频
        	sprintf(p_000,
				"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
				"{"
					"ID_0x0301:%d,"   //页面语言
					"ID_0x03B4:%d,"
				 ,
				 get_param_by_id(0x0301,0),
				 get_param_by_id(0x03B4,0)
			);
			records_num = start_query(E_FM_CURVE_LOG, &time, 1, run_buf, EVERY_FM_BUF_LEN);
			//printf("fm record num %d\n", records_num);
			memset(run_buf, 0x00, EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN);
			if(records_num==1)
			{
				query_rcd(1, run_buf, EVERY_FM_BUF_LEN);
			}
			for(int i=0; i<3; i++)
			{
				sprintf(jsCharts_temp,"D%02d:%d,",i, run_buf[2+i]);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"A%02d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[96*4+8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"B%02d:%.1f,",i, (float)p[i]/-10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[96*8+8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"C%02d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			strcat(p_000, "}");
			p_000[strlen(p_000)-2]='}'; //去掉最后一个','号
			p_000[strlen(p_000)-1]='\0';

			p_Rsc[0].size = strlen(p_000);
			end_query();
        break;
        case 42: //微网曲线
        	sprintf(p_000,
				"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n"
				"{"
					"ID_0x0301:%d,"   //页面语言
					"ID_0x03B4:%d,"
				 ,
				 get_param_by_id(0x0301,0),
				 get_param_by_id(0x03B4,0)
			);
			records_num = start_query(E_MGRID_CURVE_LOG, &time, 1, run_buf, EVERY_DAY_MGRID_CURVE_BUF_LEN);
			//printf("mgrid record num %d\n", records_num);
			memset(run_buf, 0x00, EVERY_DAY_OUTPUT_SMOOTH_BUF_LEN);
			if(records_num==1)
			{
				query_rcd(1, run_buf, EVERY_DAY_MGRID_CURVE_BUF_LEN);
			}
			for(int i=0; i<3; i++)
			{
				sprintf(jsCharts_temp,"D%02d:%d,",i, run_buf[2+i]);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"A%02d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[96*4+8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"B%02d:%.1f,",i, (float)p[i]/-10.0);
				strcat(p_000, jsCharts_temp);
			}
			memset(jsCharts_temp,0,4096);
			p = (int32_t*)&run_buf[96*8+8];
			for(int i=0; i<96; i++)
			{
				sprintf(jsCharts_temp,"C%02d:%.1f,",i, (float)p[i]/10.0);
				strcat(p_000, jsCharts_temp);
			}
			strcat(p_000, "}");
			p_000[strlen(p_000)-2]='}'; //去掉最后一个','号
			p_000[strlen(p_000)-1]='\0';

			p_Rsc[0].size = strlen(p_000);
			end_query();
        break;
        default:
            p_000[0]=0;
            p_Rsc[0].size = 1;
        break;
    }
}
void his_handler(uint8_t type,char* data)                       //导出历史记录
{
    uint8_t run_buf[EVERY_RUNRECORD_BUF_LEN] = {0};
    int32_t records_num=0;//运行记录的条数
    //const uint8_t minute[3]={0,20,40};
    uint8_t array[16]={0};
    //uint8_t month_day = 0;
    static char stemp[5];
    memset(stemp,0,sizeof(stemp));
    memcpy(stemp,data+21,4);
    R_time.Year = ((uint8_t)(atoi(stemp)-2000));
    memset(stemp,0,sizeof(stemp));
    memcpy(stemp,data+26,2);
    R_time.Month = ((uint8_t)atoi(stemp));
    memset(stemp,0,sizeof(stemp));
    memcpy(stemp,data+29,2);
    R_time.Date = ((uint8_t)atoi(stemp));

    uint8_t temp=0;
    if(Webserver_Language!=0x55)
    {
        temp=1;
    }
    memset(p_000,0,sizeof(p_000));
    sprintf(p_000,"HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\n\r\n");
    switch(type)
    {
        case 6://运行信息
            sprintf(jsCharts_temp,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\r\n",
                    Export_Name[0][temp],Export_Name[1][temp],Export_Name[2][temp],Export_Name[3][temp],Export_Name[4][temp],
                    Export_Name[5][temp],Export_Name[6][temp],Export_Name[7][temp],Export_Name[8][temp],Export_Name[9][temp],
                    Export_Name[10][temp],Export_Name[11][temp],Export_Name[12][temp],Export_Name[13][temp],Export_Name[14][temp],
                    Export_Name[15][temp],Export_Name[16][temp],Export_Name[17][temp],Export_Name[18][temp],Export_Name[19][temp],
                    Export_Name[20][temp],Export_Name[21][temp],Export_Name[22][temp],Export_Name[23][temp],Export_Name[24][temp],
                    Export_Name[25][temp],Export_Name[26][temp],Export_Name[27][temp],Export_Name[28][temp],Export_Name[29][temp],
                    Export_Name[30][temp]);
            strcat(p_000,jsCharts_temp);

            records_num = start_query(E_RUNRECORD_LOG, &R_time, 1, run_buf, EVERY_RUNRECORD_BUF_LEN);
            if((records_num > 0) && (records_num < RUNRECORD_DAY_MAX_NUM))
            {
                for(int i=1;i<=records_num;i++)
                {
                    query_rcd(i, run_buf, EVERY_RUNRECORD_BUF_LEN);
                    sprintf(jsCharts_temp,
                        "%d,"
                        "%4d.%02d.%02d/%02d:%02d:00,"
                    	"%s,"						//储能型号
                    	"%.1fkW,"					//系统额定功率
                    	"%s," 						//并网使能
                    	"%s," 						//总线类型
                    	"%s," 						//分时使能
                        "%s,"                       //工作状态
                        "%s,"                    	//电网状态
                        "%.1fV,"                    //电网电压
                        "%.2fHz,"                   //电网频率
                    	"%.1fkW,"					//电网功率
                        "%s,"                    	//电网接触器状态
                        "%s,"                    	//电网接触器故障状态
                        "%s,"                    	//微网接触器状态
                        "%s,"                    	//微网接触器故障状态
                    	"%s,"						//运行模式
                    	"%s,"						//功率控制模式
                    	"%.1fkW,"					//运行功率
                    	"%s,"						//储能并网工作模式
                    	"%.1fkW,"                   //储能逆变器有功功率
                    	"%.1fV,"                   	//微网电压
                    	"%.2fHz,"					//微网频率
						"%.1fkW,"                   //光伏逆变器功率
						"%.1fV,"                   	//光伏逆变器电压
						"%.1fkW,"                  	//负载功率
						"%.1fV,"                  	//负载电压
                        "%.1fV,"                    //电池最大电压
                    	"%.1fV,"                    //电池最小电压
                        "%.1f%%,"                   //电池最大SOC
                    	"%.1f%%"                    //电池最小SOC
                        "\r\n",
                        i,
                        (run_buf[2])+2000,(run_buf[3]),(run_buf[4]),(run_buf[5]),(run_buf[6]),
                        PcsType_Name[GetU16(run_buf+8)][temp],			//储能型号
                        (float)GetU16(run_buf+10),						//系统额定功率
                        Enable_Name[GetU16(run_buf+12)][temp],			//并网使能
                        BusType_Name[GetU16(run_buf+14)][temp],			//总线类型
                        Enable_Name[GetU16(run_buf+16)][temp],			//分时使能
                        Event_Name[GetU16(run_buf+18)][temp],			//运行状态
                        GridStatus_Name[GetU16(run_buf+20)][temp],    	//电网状态
                        GetU16(run_buf+22)/10.0,    					//电网电压
                        GetU16(run_buf+24)/100.0,   					//电网频率
                        (int16_t)GetU16(run_buf+26)/10.0,				//电网功率
                        ContactorStatus_Name[GetU16(run_buf+28)][temp], //电网接触器状态
                        FaultStatus_Name[GetU16(run_buf+30)][temp],    	//电网接触器故障状态
                        ContactorStatus_Name[GetU16(run_buf+32)][temp], //微网接触器状态
                        FaultStatus_Name[GetU16(run_buf+34)][temp],    	//微网接触器故障状态
                        RunMode_Name[GetU16(run_buf+36)][temp],			//运行模式
                        CtrlMode_Name[GetU16(run_buf+38)][temp],		//控制模式
                        (int16_t)GetU16(run_buf+40)/10.0,  				//运行功率
                        GridMode_Name[GetU16(run_buf+42)][temp],		//PCS并网工作模式
                        (int32_t)GetU32(run_buf+44)/10.0,  				//储能逆变器有功功率
                        GetU16(run_buf+48)/10.0,						//微网电压
                        GetU16(run_buf+50)/100.0,						//微网频率
                        GetU32(run_buf+52)/10.0,    					//光伏逆变器功率
                        GetU16(run_buf+56)/10.0,    					//光伏逆变器电压
                        (int32_t)GetU32(run_buf+58)/10.0,    			//负载功率
                        GetU16(run_buf+62)/10.0,     					//负载电压
                        GetU16(run_buf+64)/10.0,    					//电池最大电压
                        GetU16(run_buf+66)/10.0,    					//电池最小电压
                        GetU16(run_buf+68)/10.0, 						//电池最大SOC
                        GetU16(run_buf+70)/10.0							//电池最小SOC
                    );
                    strcat(p_000,jsCharts_temp);
                }
            }
            end_query();
            p_Rsc[0].size =strlen(p_000);
        break;
        case 7://事件信息
            sprintf(jsCharts_temp,"%s,%s,%s\r\n",Event_Name[33][temp],Event_Name[34][temp],Event_Name[35][temp]);
            strcat(p_000,jsCharts_temp);
            for(int i=1;i <= read_event_record.total_nub;i++)
            {
                Read_OnConditionNB_Record(read_event_record.ptr, read_event_record.total_nub,i,HISEVENTRECORD_START_ADDRESS,array);
                sprintf(jsCharts_temp,"%02d,%04d.%02d.%02d/%02d:%02d:%02d,%s\r\n",
                i,
                (array[0])+2000,
                (array[1]),
                (array[2]),
                (array[3]),
                (array[4]),
                (array[5]),
                Event_Name[(uint8_t)array[6]][temp]);
                strcat(p_000,jsCharts_temp);
                if(i%100==0) usleep(20);
            }
            p_Rsc[0].size =strlen(p_000);
        break;
        case 8://故障信息
            sprintf(jsCharts_temp,"%s,%s,%s\r\n",Fault_Name[65][temp],Fault_Name[66][temp],Fault_Name[67][temp]);
            strcat(p_000,jsCharts_temp);
            for(int i=1;i <= read_fault_record.total_nub;i++)
            {
                Read_OnConditionNB_Record(read_fault_record.ptr, read_fault_record.total_nub,i,HISFAULTRECORD_START_ADDRESS,array);
                sprintf(jsCharts_temp,"%02d,%04d.%02d.%02d/%02d:%02d:%02d,%s\r\n",
                i,
                (array[0])+2000,
                (array[1]),
                (array[2]),
                (array[3]),
                (array[4]),
                (array[5]),
                Fault_Name[(uint8_t)array[6]][temp]);
                strcat(p_000,jsCharts_temp);
            }
            p_Rsc[0].size =strlen(p_000);
        break;
        case 29://告警信息
            sprintf(jsCharts_temp,"%s,%s,%s\r\n",Alarm_Name[33][temp],Alarm_Name[34][temp],Alarm_Name[35][temp]);
            strcat(p_000,jsCharts_temp);
            for(int i=1;i <= read_alarm_record.total_nub;i++)
            {
                Read_OnConditionNB_Record(read_alarm_record.ptr, read_alarm_record.total_nub,i,HISALARMRECORD_START_ADDRESS,array);
                sprintf(jsCharts_temp,"%02d,%04d.%02d.%02d/%02d:%02d:%02d,%s\r\n",
                i,
                (array[0])+2000,
                (array[1]),
                (array[2]),
                (array[3]),
                (array[4]),
                (array[5]),
                Alarm_Name[(uint8_t)array[6]][temp]);
                strcat(p_000,jsCharts_temp);
            }
            p_Rsc[0].size =strlen(p_000);
        break;
        case 32: //分时电价
        break;
        case 33: //消峰填谷
        break;
        case 34: //交流限发
        break;
        case 35: //输出平滑
        break;
        case 36: //调频
        break;
        default:
            p_000[0]=0;
            p_Rsc[0].size = 1;
        break;
    }
}

