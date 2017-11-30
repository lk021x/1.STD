#include "includes.h"
#include "http_server.h"

#define USER_LOGOUT_TIMES		60

/*****************************************************************************
 libevent 与 libev 库不能同时使用, 也不能都加载，使用libev需要把libevent库去掉
 *****************************************************************************/

Srv_User_Info  User[MAX_SRV_USER];

uint8_t operator_passwd[8]="1111";  //操作员密码
uint8_t administrator_passwd[8]="2222";  //管理员密码
uint8_t maintainer_passwd[8]="3333";   //维护员密码
uint8_t config_passwd[8]="7777"; //配置界面密码
uint8_t debug_passwd[8]="5555"; //调试界面密码

//初始化用户信息
void Init_User()
{
	for(int i=0; i<MAX_SRV_USER; i++)
	{
		User[i].login_st = LOG_OFF;
		User[i].user_type = OPERATOR;
		User[i].host_page_access = 0;
		User[i].heartbeat = USER_LOGOUT_TIMES;
		memset((char*)&User[i].ip, 0x00, 64);
	}
}

//获取当前登陆用户数
int Get_Logined_User_Num()
{
	int count = 0;

	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			count++;
		}
	}
	return count;
}

//检查用户登陆状态
int Get_Register_Status(char* ip)
{
	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			if(strcmp(ip, (char*)User[i].ip) == 0)
			{
				return LOGGED_IN;
			}
		}
	}
	return LOG_OFF;
}

//获取用户ID
/*
int Get_User_ID(char *ip)
{
	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			if(strcmp(ip, (char*)User[i].ip) == 0){
				return i;
			}
		}
	}
	return -1;
}
*/

//获取当前用户数
/*
int Get_Users_MaxID()
{
	int num = 0;
	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			num++;
		}
	}
	return num;
}
*/

//获取用户权限
int Get_User_Authority(char *ip)
{
	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			if(strcmp(ip, (char*)User[i].ip) == 0){
				return User[i].user_type;
			}
		}
	}
	return -1;
}

//获取用户权限2
uint8_t Get_User_Authority_For_Web(char *ip)
{
	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			if(strcmp(ip, (char*)User[i].ip) == 0){
				if(User[i].user_type == OPERATOR) return 0x11;
				else if(User[i].user_type == ADMINISTRATOR) return 0x22;
				else if(User[i].user_type == MAINTAINER) return 0x22;
				else return 0x11;
			}
		}
	}
	return 0x11;
}

//注册用户
//返回值： 2 -- 已注册, 1 -- 注册成功， 4 -- 用户满
int Register_User(char* ip, USER_TYPE type)
{
	int i = 0;
	if(LOGGED_IN == Get_Register_Status(ip)) return 2; //已经注册

	for(i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOG_OFF)  //可以使用
		{
			mutex_for_web.lock();
			User[i].login_st = LOGGED_IN;
			User[i].user_type = type;
			User[i].host_page_access = 1;
			User[i].config_page_access = 1;
			User[i].heartbeat = USER_LOGOUT_TIMES;
			sprintf((char*)User[i].ip, "%s", ip);
			mutex_for_web.unlock();
			//printf("register usr ip %s success!\n", ip);
			return 1;
		}
	}
	return 4;
}

//注销用户
bool Logout_User(char *ip)
{
	int i = 0;
	if(LOG_OFF == Get_Register_Status(ip)) return true; //已经注销

	for(i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			if(strcmp(ip, (char*)User[i].ip) == 0)
			{
				mutex_for_web.lock();
				User[i].login_st = LOG_OFF;
				User[i].user_type = OPERATOR;
				User[i].host_page_access = 0;
				User[i].config_page_access = 0;
				User[i].heartbeat = 0;
				//printf("user %s is logoff\n", User[i].ip);
				memset((char*)&User[i].ip, 0x00, 64);
				mutex_for_web.unlock();
				return true;
			}
		}
	}
	return false;
}

//检查密码匹配
//OPERATOR -- 密码： 1111 -- 登陆index
//ADMINISTRATOR -- 密码: 2222 -- 登陆index，7777 -- 登陆config
//MAINTAINER -- 密码：3333 -- 登陆index，7777 -- 登陆config
//返回值 -- 0: 密码错误, 1：index， 3：config, 5: debug
int Check_User_Authority(USER_TYPE type, char* passwd)
{
	if(passwd[0] == 0x00) return 0;
	if(type == OPERATOR){
		if(0 == strcmp(passwd,(char*)operator_passwd)) return 1;
	}else if(type == ADMINISTRATOR){
		if(0 == strcmp(passwd,(char*)administrator_passwd)) return 1;
		else if(0 == strcmp(passwd,(char*)config_passwd)) return 3;
		else if(0 == strcmp(passwd,(char*)debug_passwd)) return 5;
	}else if(type == MAINTAINER){
		if(0 == strcmp(passwd,(char*)maintainer_passwd)) return 1;
		else if(0 == strcmp(passwd,(char*)config_passwd)) return 3;
	}else{
		return 0;
	}
	return 0;
}

bool Check_User_Password(USER_TYPE type, char* passwd)
{
	if(passwd[0] == 0x00) return false;
	if(type == OPERATOR){
		if(0 == strcmp(passwd,(char*)operator_passwd)) return true;
	}else if(type == ADMINISTRATOR){
		if(0 == strcmp(passwd,(char*)administrator_passwd)) return true;
	}else if(type == MAINTAINER){
		if(0 == strcmp(passwd,(char*)maintainer_passwd)) return true;
	}else{
		return false;
	}
	return false;
}

//判断用户注销状态
void Update_User_Login_Status()
{
	static bool stLogin[MAX_SRV_USER]={false};

	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN){
			stLogin[i] = true;
			if(ExportST) continue;
			mutex_for_web.lock();
			User[i].heartbeat -= 1;
			if(User[i].heartbeat <= 0){
				User[i].login_st = LOG_OFF;
				User[i].user_type = OPERATOR;
				User[i].host_page_access = 0;
				User[i].config_page_access = 0;
				User[i].heartbeat = 0;
				//printf("user %s is timeoff\n", User[i].ip);
				memset((char*)&User[i].ip, 0x00, 64);
			}
			mutex_for_web.unlock();
		}else{
			if(stLogin[i] && User[i].heartbeat != 0){  //当用户异常断开时需要注销用户
				stLogin[i] = false;
				mutex_for_web.lock();
				User[i].login_st = LOG_OFF;
				User[i].user_type = OPERATOR;
				User[i].host_page_access = 0;
				User[i].config_page_access = 0;
				User[i].heartbeat = 0;
				//printf("user %s is logoff\n", User[i].ip);
				memset((char*)&User[i].ip, 0x00, 64);
				mutex_for_web.unlock();
			}
		}
	}
}

//更新心跳
void Update_User_HeartBeat(char *ip)
{
	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			if(strcmp(ip, (char*)User[i].ip) == 0){
				mutex_for_web.lock();
				User[i].heartbeat = USER_LOGOUT_TIMES;
				mutex_for_web.unlock();
			}
		}
	}
}

//获取主页访问权限
int Get_Host_Page_Access(char *ip)
{
	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			if(strcmp(ip, (char*)User[i].ip) == 0){
				return User[i].host_page_access;
			}
		}
	}
	return 0;
}

//重置主页访问权限
void Clear_Host_Page_Access(char *ip)
{
	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			if(strcmp(ip, (char*)User[i].ip) == 0){
				User[i].host_page_access = 0;
			}
		}
	}
}

//获取配置页访问权限
int Get_Config_Page_Access(char *ip)
{
	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			if(strcmp(ip, (char*)User[i].ip) == 0){
				return User[i].config_page_access;
			}
		}
	}
	return 0;
}

//重置配置页访问权限
void Clear_Config_Page_Access(char *ip)
{
	for(int i=0; i<MAX_SRV_USER; i++)
	{
		if(User[i].login_st == LOGGED_IN)
		{
			if(strcmp(ip, (char*)User[i].ip) == 0){
				User[i].config_page_access = 0;
			}
		}
	}
}
int authority = 0;
void Data_Parse(char *data_buf, char *user_ip, http_state *hs)
{
	char user_name[8]={0};
	char user_passwd[8]={0};
	char user_newpasswd[8]={0};
	char *p = (char*)(data_buf+5);

	if(p[0] == 'F' && p[1] == 'e' && p[2] == 'e' && p[3] == 'd')  //"GET /Feed.IExxHTTP/1.1"
	{
		Update_User_HeartBeat(user_ip);
		if(Get_Register_Status(user_ip)==LOGGED_IN){
			sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",0);  //返回0
		}else{
			sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",1);  //用户已退出，页面返回到登陆界面
		}
		a_Rsc[0].size = strlen(a_000);
		hs->file = (char*)a_Rsc[0].data;
		hs->left = (int)a_Rsc[0].size;
	}
	else if(p[0] == 'L' && p[1] == 'A')	//登陆认证  "GET /LA.html?0x0282=1&0x0283=2222&time=Mon%20May%2015%202017%2014:25:28%20GMT+0800 HTTP/1.1
	{
		char delim[] = "?=&+";//切割字符串
		char *ptemp;	//切割出的内容
		ptemp = strtok(data_buf,delim);
		ptemp = strtok(NULL,delim);
		if(!strcmp(ptemp,"0x0282")) {
			ptemp = strtok(NULL,delim);
			sprintf(user_name, "%s", ptemp);
		}
		ptemp = strtok(NULL,delim);
		if(!strcmp(ptemp,"0x0283")) {
			ptemp = strtok(NULL,delim);
			sprintf(user_passwd, "%s", ptemp);
		}
		//printf("user name %d, passwd %s ip %s\n", atoi(user_name), user_passwd, user_ip);
		authority = Check_User_Authority((USER_TYPE)atoi(user_name), user_passwd);
		if(authority > 0){
			if(LOG_OFF == Get_Register_Status(user_ip)){
				if(Register_User(user_ip, (USER_TYPE)atoi(user_name)) != 4){
					//printf("user login success!\n");
					sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",authority);
				}else{
					//printf("user full!\n");
					sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",4); //登陆用户满
				}
			}else{
				if(Get_User_Authority(user_ip) != (USER_TYPE)atoi(user_name)){
					sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",2); //同权限用户已经登陆
					//printf("user login yet!\n");
				}else{
					Update_User_HeartBeat(user_ip);
					//printf("user login again!\n");
					sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",authority); //已经登陆的情况下，当关闭网页在超时时间到之前可以登陆。
				}
			}
		}else{
			sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",0);
		}
		a_Rsc[0].size = strlen(a_000);
		hs->file = (char*)a_Rsc[0].data;
		hs->left = (int)a_Rsc[0].size;
	}
	else if(p[0] == 'I' && p[1] == 'n' && p[2] == 'd' && p[3] == 'e' && p[4] == 'x')   //请求主页面 "GET /Index.html HTTP/1.1"
	{
		if(LOGGED_IN == Get_Register_Status(user_ip))
		{
			//if(Get_Host_Page_Access(user_ip)>0 && authority == 1)
			{
				//Clear_Host_Page_Access(user_ip);
				int type = Get_User_Authority(user_ip);
				switch(type)
				{
					case OPERATOR:
					case ADMINISTRATOR:
					case MAINTAINER:
						hs->file = (char*)h_Rsc[1].data;
						hs->left = (int)h_Rsc[1].size;
						authority = 0;
					break;
					default:
						hs->file = (char*)h_Rsc[14].data;
						hs->left = (int)h_Rsc[14].size;
					break;
				}
			}
			//else
			//{
			//	hs->file = (char*)h_Rsc[14].data;
			//	hs->left = (int)h_Rsc[14].size;
			//}
		}
		else
		{
			hs->file = (char*)h_Rsc[14].data;
			hs->left = (int)h_Rsc[14].size;
		}
	}
	else if(p[0] == 'C' && p[1] == 'o' && p[2] == 'n' && p[3] == 'f' && p[4] == 'i' && p[5] == 'g') //请求配置页面 "GET /Config.html HTTP/1.1"
	{
		if(authority == 3)
		{
			hs->file = (char*)h_Rsc[20].data;
			hs->left = (int)h_Rsc[20].size;
			authority = 0;
		}
		else
		{
			hs->file = (char*)h_Rsc[14].data;
			hs->left = (int)h_Rsc[14].size;
		}
	}
	else if(p[0] == 'D' && p[1] == 'e' && p[2] == 'b' && p[3] == 'u' && p[4] == 'g') //请求配置页面 "GET /Debug.html HTTP/1.1"
	{
		if(authority == 5)
		{
			hs->file = (char*)h_Rsc[46].data;
			hs->left = (int)h_Rsc[46].size;
			authority = 0;
		}
		else
		{
			hs->file = (char*)h_Rsc[14].data;
			hs->left = (int)h_Rsc[14].size;
		}
	}
	else if(p[0] == 'P' && p[1] == 'D' && p[2] == 'M') //密码修改 "GET /PDM.IExxHTTP/1.1",  原来发出的是 "GET /a_010.IExxHTTP/1.1"
	{
		if(LOGGED_IN == Get_Register_Status(user_ip))
		{
			char delim[] = "?=&+";//切割字符串
			char *ptemp;	//切割出的内容
			ptemp = strtok(data_buf,delim);
			ptemp = strtok(NULL,delim);
			if(!strcmp(ptemp,"0x0282")) {
				ptemp = strtok(NULL,delim);
				sprintf(user_name, "%s", ptemp);
			}
			ptemp = strtok(NULL,delim);
			if(!strcmp(ptemp,"0x0283")) {
				ptemp = strtok(NULL,delim);
				sprintf(user_passwd, "%s", ptemp);
			}
			ptemp = strtok(NULL,delim);
			if(!strcmp(ptemp,"0x0284")) {
				ptemp = strtok(NULL,delim);
				sprintf(user_newpasswd, "%s", ptemp);
			}
			int type = Get_User_Authority(user_ip);
			switch(type)
			{
				case ADMINISTRATOR:
					if(Check_User_Password((USER_TYPE)atoi(user_name), user_passwd)){
						if(atoi(user_name) == OPERATOR){
							for(int i=0; i<8; i++) operator_passwd[i] = user_newpasswd[i];
							(void)write_eeprom(OPERATOR_PASSWORD_ADDRESS, user_newpasswd, 8);
						}else if(atoi(user_name) == ADMINISTRATOR){
							for(int i=0; i<8; i++) administrator_passwd[i] = user_newpasswd[i];
							(void)write_eeprom(ADMINISTRATOR_PASSWORD_ADDRESS, user_newpasswd, 8);
						}else if(atoi(user_name) == MAINTAINER){
							for(int i=0; i<8; i++) maintainer_passwd[i] = user_newpasswd[i];
							(void)write_eeprom(MAINTRAINER_PASSWORD_ADDRESS, user_newpasswd, 8);
						}
						sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",1);
					}else{
						sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",0);
					}
				break;
				default:
					sprintf(a_000,"HTTP/1.1 200 OK\r\nContent-Type:text/plain\r\nAccept-Ranges:bytes\r\n\r\n%d",0);
				break;
			}
			a_Rsc[0].size = strlen(a_000);
			hs->file = (char*)a_Rsc[0].data;
			hs->left = (int)a_Rsc[0].size;
		}
	}
	else
	{
		char delim[] = " ";			//切割字符串
		char *ptemp;				//切割出的内容
		char temp[4]={0};
		memcpy(temp,data_buf+7,3);
		int num=atoi(temp);			//获取序号
		switch(p[0])//资源类型
		{
			case ' '://Login.html
				hs->file = (char*)h_Rsc[0].data;
				hs->left = (int)h_Rsc[0].size;
			break;
			case 'L'://Login.html
				if(p[1] == 'o' && p[2] == 'g' && p[3] == 'i' && p[4] == 'n'){
					hs->file = (char*)h_Rsc[0].data;
					hs->left = (int)h_Rsc[0].size;
				}
			break;
			case 'h'://h_xxx.html
				if(num <= MAX_PAGES){
					hs->file = (char*)h_Rsc[num].data;
					hs->left = (int)h_Rsc[num].size-1;
				}else{//否则返回登录页面

					hs->file = (char*)h_Rsc[14].data;
					hs->left = (int)h_Rsc[14].size;
				}
			break;
			case 'j'://javascript
				ptemp = strtok(data_buf,delim);
				ptemp = strtok(NULL,delim);
				if(!strcmp(ptemp,"/jquery.flot.min.js")) { hs->file = (char*)j_Rsc[3].data; hs->left = (int)j_Rsc[3].size-1; }
				else if(!strcmp(ptemp,"/jquery.min.js")) { hs->file = (char*)j_Rsc[4].data; hs->left = (int)j_Rsc[4].size-1; }
				else if(!strcmp(ptemp,"/jquery.flot.time.js")) { hs->file = (char*)j_Rsc[5].data; hs->left = (int)j_Rsc[5].size-1; }
				else if(!strcmp(ptemp,"/j_highcharts.js")) { hs->file = (char*)j_Rsc[7].data; hs->left = (int)j_Rsc[7].size-1; }
				else if(!strcmp(ptemp,"/j_exporting.js")) { hs->file = (char*)j_Rsc[8].data; hs->left = (int)j_Rsc[8].size-1; }
				else if(!strcmp(ptemp,"/j_canvas-tools.js")) { hs->file = (char*)j_Rsc[9].data; hs->left = (int)j_Rsc[9].size-1; }
				else if(!strcmp(ptemp,"/j_export-csv.js")) { hs->file = (char*)j_Rsc[10].data; hs->left = (int)j_Rsc[10].size-1; }
				else if(!strcmp(ptemp,"/j_highcharts-export-clientside.js")) { hs->file = (char*)j_Rsc[11].data; hs->left = (int)j_Rsc[11].size-1; }
				else if(!strcmp(ptemp,"/j_zDialog.js")) { hs->file = (char*)j_Rsc[12].data; hs->left = (int)j_Rsc[12].size-1; }
				else if(!strcmp(ptemp,"/j_encoding.js")) { hs->file = (char*)j_Rsc[13].data; hs->left = (int)j_Rsc[13].size-1; }
				else if(!strcmp(ptemp,"/j_encoding-indexes.js")) { hs->file = (char*)j_Rsc[14].data; hs->left = (unsigned long)j_Rsc[14].size-1; }
				else if(!strcmp(ptemp,"/j_filesaver.js")) { hs->file = (char*)j_Rsc[15].data; hs->left = (int)j_Rsc[15].size-1; }
				else if(!strcmp(ptemp,"/j_excanvas.min.js")) { hs->file = (char*)j_Rsc[6].data; hs->left = (int)j_Rsc[6].size-1; }
				else{
					if(num < 3){
						hs->file = (char*)j_Rsc[num].data;
						hs->left = (int)j_Rsc[num].size-1;
					}else{
						hs->file = (char*)j_Rsc[0].data;
						hs->left = 0;
					}
				}
			break;
			case 'c'://c_000.css  / css  样式表
				hs->file = (char*)c_Rsc[num].data;
				hs->left = (int)c_Rsc[num].size-1;
			break;
			case 'i'://i_xxx.png, i_xxx.gif  / image
				hs->file = (char*)i_Rsc[num].data;
				hs->left = (int)i_Rsc[num].size;
			break;
			case 's'://s_xxx.html / 请求获取数据
				s_handler(num,data_buf,user_ip);
				hs->file = (char*)s_Rsc[0].data;
				hs->left = (int)s_Rsc[0].size;
			break;
			case 'a'://a_xxx.html / 请求设置数据
				a_handler(num,data_buf,user_ip);
				hs->file = (char*)a_Rsc[0].data;
				hs->left = (int)a_Rsc[0].size;
			break;
			case 'r'://r_xxx.html / 查询历史记录
				if(ExportST == 0){
					ExportST = 1;
					r_handler(num,data_buf);
					ExportST = 0;
					hs->file = (char*)p_Rsc[0].data;
					hs->left = (int)p_Rsc[0].size;
					Update_User_HeartBeat(user_ip);
				}else{
					hs->file = (char*)p_Rsc[0].data;
					hs->left = 0;
				}
			break;
			case 'R'://运行记录csv
				if(ExportST == 0){
					ExportST = 1;
					his_handler(6,data_buf);
					ExportST = 0;
					hs->file = (char*)p_Rsc[0].data;
					hs->left = (int)p_Rsc[0].size;
					Update_User_HeartBeat(user_ip);
				}else{
					hs->file = (char*)p_Rsc[0].data;
					hs->left = 0;
				}
			break;
			case 'E'://事件记录csv
				if(ExportST == 0){
					ExportST = 1;
					his_handler(7,data_buf);
					ExportST = 0;
					hs->file = (char*)p_Rsc[0].data;
					hs->left = (int)p_Rsc[0].size;
					Update_User_HeartBeat(user_ip);
				}else{
					hs->file = (char*)p_Rsc[0].data;
					hs->left = 0;
				}
			break;
			case 'F'://故障记录csv
				if(ExportST == 0){
					ExportST = 1;
					his_handler(8,data_buf);
					ExportST = 0;
					hs->file = (char*)p_Rsc[0].data;
					hs->left = (int)p_Rsc[0].size;
					Update_User_HeartBeat(user_ip);
				}else{
					hs->file = (char*)p_Rsc[0].data;
					hs->left = 0;
				}
			break;
			case 'A'://告警记录csv
				if(ExportST == 0){
					ExportST = 1;
					his_handler(29,data_buf);
					ExportST = 0;
					hs->file = (char*)p_Rsc[0].data;
					hs->left = (int)p_Rsc[0].size;
					Update_User_HeartBeat(user_ip);
				}else{
					hs->file = (char*)p_Rsc[0].data;
					hs->left = 0;
				}
			break;
			default://默认替换成登陆界面
				//Logout_User(user_ip);
				hs->file = (char*)h_Rsc[14].data;
				hs->left = (int)h_Rsc[14].size;
			break;
		}
	}

	System_Info.EMS_MONITOR_Web_Http_Com_Count=SIGNAL_FULL;
}

