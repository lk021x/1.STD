/*
 * net_util.h
 *
 *  Created on: 2017-1-19
 *      Author: root
 */

#ifndef NET_UTIL_H_
#define NET_UTIL_H_

int GetLocalIp(const char* if_name, char *ipaddr);
int SetLocalIp(const char* if_name, const char *ipaddr);
int GetLocalMac(const char* if_name, char* mac_addr);
int GetLocalNetMask(const char* if_name, char* netmask_addr);
int SetLocalNetMask(const char* if_name, const char *szNetMask);
int GetGateWay(char* gateway);
int SetGateWay(const char *szGateWay);

#endif /* NET_UTIL_H_ */
