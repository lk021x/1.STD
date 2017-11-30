#ifndef  _CONNECTION_H
#define _CONNECTION_H

#include "TcpUtil.h"

class Connection
{
public:
	Connection();
	Connection(char* IP,int Port);
	~Connection();
	int 	Connect();          			//连接服务器
	bool 	DisConnect();       			//与服务器断开连接
	int 	ReConnect();
	int     GetSocket();

	void 	SetServerIP(char* IP);      	//设置服务器IP地址
	void 	SetServerPort(int Port);    	//设置服务器端口号
	void 	SetSendTimeout();   			//设置发送内容超时时间
	void 	SetReceiveTimeout();			//设置接收内容超时时间

	int 	Send(const char* buf,int len);
	int 	Receive(char* buf,int len);

	bool 	CheckConnectStatus();  			//检查连接状态
public:
	char   	ServerIP[32];     				//服务器端IP地址
	int     ServerPort; 					//服务器连接端口

private:
	TcpUtil ConnectSock;     				//TcpUtil实例
};
#endif
