/*
 * UdpUtil.h
 *
 *  Created on: 2017-1-21
 *      Author: root
 */

#ifndef UDPUTIL_H_
#define UDPUTIL_H_

int udp_init(void);
int send_udp(char *buf, int len);

extern struct sockaddr_in broadcast_addr; //广播地址
extern int udp_sock;


#endif /* UDPUTIL_H_ */
