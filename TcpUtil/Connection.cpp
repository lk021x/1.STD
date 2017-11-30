#include "Connection.h"

Connection::Connection()
{
	memset(ServerIP, 0x00, 32);
	ServerPort = 0;
}

Connection::Connection(char* IP,int Port)
{
	memset(ServerIP, 0x00, 32);
	ServerPort = 0;
	SetServerIP(IP);
	SetServerPort(Port);
}

Connection::~Connection()
{
	ConnectSock.CloseSocket();
}

int Connection::Connect()
{
	int ret = ConnectSock.CreateSocket(ServerIP,ServerPort,0);
	if(ret == 0)
	{
		ret = ConnectSock.Connect();
		//ret = ConnectSock.Connect2();
		return ret;
	}
	else
	{
		return -1;
	}
}

bool Connection::DisConnect()
{
	ConnectSock.CloseSocket();
	return 0;
}

int Connection::ReConnect()
{
	int ret = ConnectSock.ReConnect();
	return ret;
}

int Connection::GetSocket()
{
	return (ConnectSock.GetSocket());
}

void Connection::SetServerIP(char* IP)
{
	memcpy(ServerIP, IP, strlen(IP));
}

void Connection::SetServerPort(int Port)
{
	ServerPort = Port;
}

void Connection::SetSendTimeout()
{

}

void Connection::SetReceiveTimeout()
{

}

int Connection::Send(const char* buf,int len)
{
	int ret = 0;
	ret = ConnectSock.SendSocket(buf,len,0);  //0ms, 不设置超时发送
	return ret;
}

int  Connection::Receive(char* buf,int len)
{
	int ret = 0;
	ret = ConnectSock.ReceiveSocket(buf,len, 10); //10ms, 过长会影响多线程中的定时
	return ret;
}

bool Connection::CheckConnectStatus()
{
	int ret = 0;
	ret = ConnectSock.GetConnectStatus();
	return ret;
}
