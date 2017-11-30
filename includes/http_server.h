#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H

#define MAX_SRV_USER			5				//http同时支持的最大在线用户数

enum LOGIN_STATUS
{
    LOGGED_IN  = 1,   //已登陆
    LOG_OFF = 2    //注销
};

enum USER_TYPE
{
	OPERATOR = 0,  //操作员, 只具有察看权限
	ADMINISTRATOR = 1, //管理员， 具有全部功能操作权限
	MAINTAINER = 2  //维护员， 具有设置察看权限，不具有密码修改权限
};

typedef struct
{
	LOGIN_STATUS login_st;              //登陆状态
	USER_TYPE user_type;				//用户类型
	int host_page_access;				//主页访问权限    > 1 -- 可以访问， 0 -- 禁止访问
	int config_page_access;				//配置页面访问权限    > 1 -- 可以访问， 0 -- 禁止访问
	int heartbeat;						//心跳
	uint8_t ip[64];						//登陆用户IP
} Srv_User_Info;

extern uint8_t operator_passwd[8];  	//操作员密码
extern uint8_t administrator_passwd[8]; //管理员密码
extern uint8_t maintainer_passwd[8];   	//维护员密码

extern Srv_User_Info  User[MAX_SRV_USER];

extern void Init_User();
extern int Get_Logined_User_Num();
extern bool Logout_User(char *ip);
extern int Get_User_ID(char *ip);
extern int Get_Users_MaxID();
extern int Get_User_Authority(char *ip);
extern uint8_t Get_User_Authority_For_Web(char *ip);
extern void Update_User_Login_Status();

#endif
