/*
 * fcntl.cpp
 *
 *  Created on: Jan 4, 2017
 *      Author: lk
 */

#include <termios.h>            /* tcgetattr, tcsetattr */
#include <stdio.h>              /* perror, printf, puts, fprintf, fputs */
#include <unistd.h>             /* read, write, close */
#include <sys/ioctl.h>
#include <linux/watchdog.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "emfcntl.h"
#include "serial.h"

static int32_t super_system(const char * cmd, char *retmsg, int32_t msg_len, int flag);

/*
 *	pathname -- 驱动路径 (例如:/dev/ttyO0)
 *  flags -- 对于串口: 打开模式 (sample:O_RDWR | O_NOCTTY)
 *           	   对于CAN: 打开摸索(SOCK_RAW or SOCK_DGRAM(低字高S8) | CAN_RAW or CAN_BCM(低字低8）)
 *  arg -- 对于串口:  参数
 *
 **/
int32_t drv_open(const char* pathname, int32_t flags, int32_t mode)
{
	int32_t ret=-1, device_id=-1;
	int32_t fd=-1;

	if (0 == strcmp (pathname, "/dev/ttyO0")) {device_id=0;}
	else if (0 == strcmp (pathname, "/dev/ttyO1")) {device_id=1;}
	else if (0 == strcmp (pathname, "/dev/ttyO2")) {device_id=2;}
	else if (0 == strcmp (pathname, "/dev/ttyO3")) {device_id=3;}
	else if (0 == strcmp (pathname, "/dev/ttyO4")) {device_id=4;}
	else if (0 == strcmp (pathname, "/dev/ttyO5")) {device_id=5;}
	else if (0 == strcmp (pathname, "/dev/ttyS0")) {device_id=6;}
	else if (0 == strcmp (pathname, "/dev/ttyS1")) {device_id=7;}
	else if (0 == strcmp (pathname, "/dev/ttyS2")) {device_id=8;}
	else if (0 == strcmp (pathname, "/dev/ttyS3")) {device_id=9;}
	else if (0 == strcmp (pathname, "/dev/ttyS4")) {device_id=10;}
	else if (0 == strcmp (pathname, "/dev/ttyS5")) {device_id=11;}
	else if (0 == strcmp (pathname, "/dev/ttyXRUSB0")) {device_id=12;}
	else if (0 == strcmp (pathname, "/dev/ttyXRUSB1")) {device_id=13;}
	else if (0 == strcmp (pathname, "/dev/ttyXRUSB2")) {device_id=14;}
	else if (0 == strcmp (pathname, "/dev/ttyXRUSB3")) {device_id=15;}
	else if (0 == strcmp (pathname, "can0")) {device_id=21;}
	else if (0 == strcmp (pathname, "can1")) {device_id=22;}
	else if (0 == strcmp (pathname,  "/dev/watchdog")) {device_id=23;}
	else{
		//fprintf (stderr, "device %s is not exist!\n", pathname);
		return (-1);
	}

	fd = -1;
	if(device_id<=15){		//串口
		fd = open (pathname, flags);
		if (-1 == fd) {
			//fprintf (stderr, "cannot open port %s\n", pathname);
			return (-1);
		}
		//printf("com port fd = %d\n", fd);
		serial_param param;
		param.baudrate=9600;
		param.databit=8;
		param.stopbit=(char*)"1";
		param.parity='N';
		if(device_id==5) param.vmin=8;
		else param.vmin=255;
		//tcgetattr(fd, &termios_old);       //保存老的串口参数
		ret = set_port_attr (fd, param.baudrate, param.databit, param.stopbit, param.parity, param.vmin); //0成功，-1失败
		if (-1 == ret) {
			fprintf (stderr, "\nport %s cannot set baudrate at %d\n",pathname,param.baudrate);
			return (ret);
		}
	}else if(device_id==21||device_id==22){  //CAN
		struct sockaddr_can addr;
		struct ifreq ifr;

		srand(time(NULL));
		/* 建立套接字，设置为原始套接字，原始CAN协议 */
		fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
		if (fd < 0) {
			fprintf(stderr, "open %s PF_CAN err", pathname);
			return (-1);
		}

		/* 以下是对CAN接口进行初始化，如设置CAN接口名，即当我们用ifconfig命令时显示的名字 */
		//为了套接字和所有的CAN接口帮定，接口索引必须是0.这样套接字可以从所有使用的CAN接口接收CAN帧
		//revfrom(2)可以制定从哪个接口接收， sendto(2)可以指定从哪个接口发送
		//strcpy(ifr.ifr_name, "can0");  //获取第0个CAN接口的ID
		strcpy(ifr.ifr_name, pathname);  //获取指定CAN接口的ID   "can0" "can1"
		ret = ioctl(fd, SIOCGIFINDEX, &ifr);    //获取网卡序号
		if (ret < 0) {
			fprintf(stderr, "ioctl %s err\n", pathname);
			return (-1);
		}

		//设置CAN协议
		addr.can_family = PF_CAN;
		addr.can_ifindex = ifr.ifr_ifindex;  //  ifr.ifr_ifindex绑定指定的CAN接口ID到SOCK， 如果是0则套接字绑定所有CAN接口
		//套接字绑定到CAN接口
		ret = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
		if (ret < 0) {
			fprintf(stderr, "bind %s err\n", pathname);
			return -1;
		}
	}else if(device_id==23){			//看门狗
		fd = open (pathname, flags);
		if (-1 == fd) {
			fprintf (stderr, "cannot open WDT \n");
			return (-1);
		}
		printf("WDT is opened\n");
		int timeout;
		timeout=10;  //10s超时
		ioctl(fd, WDIOC_SETTIMEOUT,  &timeout);
		ioctl(fd, WDIOC_GETTIMEOUT,  &timeout);
		printf("The timeout was is %d seconds\n", timeout);
	}

	return (fd);
}

/*
 *  设置CAN接口过滤字
 *  fd -- can socket套接字
 *  type -- 0 -- 接收规则， 1 -- 发送规则
 */
int set_can_filter(int fd, uint8_t type)
{
	/*
	* struct can_filter - CAN ID based filter in can_register().
	* @can_id:   relevant bits of CAN ID which are not masked out.
	* @can_mask: CAN mask (see description)
	*
	* Description:
	* A filter matches, when
	*
	* <received_can_id> & mask == can_id & mask
	*
	*/
	if(type == 0)
	{
		// 过滤规则：当<received_can_id> & mask == can_id & mask时，接收报文，否则过滤掉.
		// 可以同时添加多条过滤规则
	    const int n = 1;
		struct can_filter rfilter[n];
		rfilter[0].can_id = 0x00F; //  如果是扩展帧 id | CAN_EFF_FLAG;
		rfilter[0].can_mask = CAN_SFF_MASK;  //标准帧屏蔽字
		(void)setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, rfilter, n*sizeof(struct can_filter));
		//过滤错误帧
		//can_err_mask_t err_mask = ( CAN_ERR_TX_TIMEOUT | CAN_ERR_BUSOFF );
		//(void)setsockopt(fd, SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &err_mask, sizeof(err_mask));
	}
	else
	{
		const int n = 1;
		struct can_filter sfilter[n];
		// 在用来发送CAN帧的CAN_RAW套接字上不接收任何CAN帧
		sfilter[0].can_id   = 0x00000000;
		sfilter[0].can_mask = CAN_EFF_MASK;
		(void)setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, sfilter, n*sizeof(struct can_filter));
	    //禁用接收过滤规则, 忽略所有帧
	    //(void)setsockopt(recv_socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
	}

    return  0;
}

int set_can_loopback(const int sockfd, const bool lp)
{
    // 在默认情况下，本地回环功能是开启的，可以使用下面的方法关闭回环/开启功能：
    int loopback = lp;  // 0表示关闭, 1表示开启(默认)
    (void)setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));

#if 0
    // 在本地回环功能开启的情况下，所有的发送帧都会被回环到与CAN总线接口对应的套接字上。
    // 默认情况下，发送CAN报文的套接字不想接收自己发送的报文，因此发送套接字上的回环功能是关闭的。
    // 可以在需要的时候改变这一默认行为：
    int ro = 1; // 0表示关闭(默认), 1表示开启
    (void)setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &ro, sizeof(ro));
#endif

    return  0;
}

int32_t set_can_baud(int32_t baud)
{
	char cmd[128]={0};
	sprintf(cmd, "canconfig can0 stop");
	super_system(cmd, 0, 0, 0);
	memset(cmd,0x00,128);
	sprintf(cmd, "canconfig can0 bitrate %d ctrlmode tripple-sampling on", baud);
	super_system(cmd, 0, 0, 0);
	memset(cmd,0x00,128);
	sprintf(cmd, "canconfig can0 start");
	super_system(cmd, 0, 0, 0);
	return 0;
}

int32_t drv_ioctl(int32_t fildes, uint32_t request, void* arg)
{
	uint8_t function=0;
	uint32_t device_type=0;
	uint32_t gpio=0,value=0;
	uint32_t temp=0;

	device_type=request&0x00000f00;
	if(device_type==CTRL_GPIO){
		gpio=(uint8_t)(request>>24);
		value=(uint8_t)(request>>16);
		//printf("type 0x%04x 0x%04x 0x%04x\n", device_type, gpio, value);
		drv_write_gpio(GPIO_OUTPUT_PATH, gpio, &value);
	}else if(device_type==CTRL_SERIAL){
		function=request&0xff;
		switch(function)
		{
		case USART_SET_BAUD:
			temp=*((uint32_t*)arg);
			//printf("baud %d vim %d\n", temp&0xffff,(temp>>16)&0xffff);
			set_port_attr(fildes, temp&0xffff, 8, (char*)"1", 'N', (temp>>16)&0xffff);
			break;
		case USART_SET_STOPBIT:
			temp=*((uint32_t*)arg);
			//printf("stopbit %d\n", temp&0xff);
			set_uart_stopbit(fildes, (temp&0xff));
			break;
		case USART_SET_PARITY:
			temp=*((uint32_t*)arg);
			//printf("parity %d\n", temp&0xff);
			set_uart_parity(fildes, temp&0xff);
			break;
		case USART_SET_DATA:
			temp=*((uint32_t*)arg);
			//printf("databit %d", temp&0xff);
			set_uart_data_bit(fildes, temp&0xff);
			break;
		case USART_GET_BAUD:
			return (get_baudrate(fildes));
			break;
		default:
			break;
		}
	}else if(device_type==CTRL_CAN){
		function=request&0xff;
		switch(function)
		{
		case CAN_SET_BAUD:
			temp=*((uint32_t*)arg);

			if(temp == CAN_BAUD_1Mbps) value = 1000000;
			else if(temp == CAN_BAUD_500Kbps) value = 500000;
			else if(temp == CAN_BAUD_250Kbps) value = 250000;
			else if(temp == CAN_BAUD_200Kbps) value = 200000;
			else if(temp == CAN_BAUD_100Kbps) value = 100000;
			else if(temp == CAN_BAUD_80Kbps) value = 80000;
			else if(temp == CAN_BAUD_30Kbps) value = 30000;
			else if(temp == CAN_BAUD_20Kbps) value = 20000;
			else value = 500000;
			printf("can baud set %d\n", value);
			set_can_baud(value);
			break;
		default:
			break;
		}
	}else if(device_type==CTRL_NET){
		switch(request)
		{
		case NET_SET_IP:
			break;
		case NET_SET_SUBMASK:
			break;
		case NET_SET_GATEWAY:
			break;
		case NET_SET_DHCP_ON:
			break;
		case NET_SET_DHCP_OFF:
			break;
		default:
			break;
		}
	}

	return 0;
}

//关闭驱动
int32_t drv_close(int32_t fd)
{
	//在关闭前刷新输出并恢复原来的属性
	if(fd < 0) return -1;
	//tcsetattr (fd, TCSADRAIN, &termios_old);
	close (fd);
	fd = -1;
	return 0;
}

//buf -- 数据缓存区
//count -- GPIO数量
/*测试
 * while(1)
{
	drv_read_gpio(dig_input,16);
	printf("in -- ");for(int i=0;i<16;i++)printf(" %d ", dig_input[i]);printf("\r\n");
	sleep(1);
}
*/
int32_t drv_read_gpio(const char* pathname, uint8_t* buf,uint32_t count)
{
	uint8_t i=0;
	uint8_t *data=(uint8_t*)buf;
	uint16_t port[16]={496,511,497,510,498,509,499,508,500,507,501,506,502,505,503,504};  //input port
	char str[128]={0};
	int32_t fd = -1;
	uint8_t ch;

	if (0 != strcmp (pathname, GPIO_INPUT_PATH)) return -1;

	for(i = 0; i<16; i++)
	{
		//memset(str,0x00,128);
		sprintf(str, "%s/gpio%d/value", pathname, port[i]);
		fd = -1;
		fd = open (str, O_RDONLY);
		if (fd < 0) {
			//fprintf (stderr, "cannot open %s\n", str);
		} else {
			int ret = read (fd, &ch, 1);
			if(ret == 1) data[i]=ch-0x30;
			close(fd);
		}
	}

	//printf("in -- ");for(i=0;i<16;i++)printf(" %d ", data[i]);printf("\r\n");
	return 0;
}

int32_t drv_read_adc(const char* pathname, void* buf, uint32_t count)
{
	int32_t fd=-1;
    struct timeval tv_timeout;
    uint32_t timeout = 50;
    fd_set fs_read;
    int ret = -1;

	fd = open (pathname, O_RDONLY|O_NOCTTY|O_NONBLOCK);
	if (-1 == fd) {
		//fprintf (stderr, "cannot open ADC \n");
		return (-1);
	}

	tv_timeout.tv_sec  = timeout  / 1000;
	tv_timeout.tv_usec = (timeout % 1000) * 1000;
	FD_ZERO(&fs_read);
	FD_SET(fd, &fs_read);	//if fd == -1, FD_SET will block here
	ret = select((int)fd + 1, &fs_read, NULL, NULL, &tv_timeout);

    if(ret == 0) {return 0;} //rev overtime
    if(ret < 0) {return ret;} // select error
    ret = read(fd, (char*)buf, count);  //count = 40
    if(ret <= 0) {return  -1;}
    close(fd);
    return  ret;
}

/*测试
 * while(1)
{
	(void)drv_ioctl(gt_glp.fdido,OP_LED|DO_STATE_HIGH,0);
	sleep(1);
	(void)drv_ioctl(gt_glp.fdido,FA_LED|DO_STATE_HIGH,0);
	sleep(1);
	(void)drv_ioctl(gt_glp.fdido,KM1|DO_STATE_HIGH,0);
	sleep(1);
	(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_HIGH,0);
	sleep(1);
	(void)drv_ioctl(gt_glp.fdido,OP_LED|DO_STATE_LOW,0);
	sleep(1);
	(void)drv_ioctl(gt_glp.fdido,FA_LED|DO_STATE_LOW,0);
	sleep(1);
	(void)drv_ioctl(gt_glp.fdido,KM1|DO_STATE_LOW,0);
	sleep(1);
	(void)drv_ioctl(gt_glp.fdido,KM2|DO_STATE_LOW,0);
	sleep(1);
}*/

int32_t drv_write_gpio(const char* pathname, int32_t flags, void* arg)
{
	int32_t fd=-1, i=0, value=0;
	uint8_t port[4]={45,44,27,26};  //output port
	char str[64]={0};

	if (0 != strcmp (pathname, GPIO_OUTPUT_PATH)) return -1;

	if((flags&0xff)==0x00){
		i=0;
	}else if((flags&0xff)==0x01){
		i=1;
	}else if((flags&0xff)==0x02){
		i=2;
	}else if((flags&0xff)==0x03){
		i=3;
	}else{
		return -1;
	}

	value=*((uint32_t*)arg);
	if(value !=0 && value != 1) return -1;
	value += 0x30;

	//echo 1 > /sys/devices/virtual/gpio/gpio64/value
	//memset(str,0x00,128);
	sprintf(str, "%s/gpio%d/value", pathname, port[i]);
	fd = open (str, O_WRONLY);
	if (fd < 0) {
		fprintf (stderr, "cannot open %s\n", str);
	} else {
		write (fd, &value, 1);
		close (fd);
	}

	return 0;
}

int32_t drv_write_light_gpio(const char* path, uint8_t value)
{
	int32_t fd = -1;
	uint8_t val = value+0x30;

	if(value !=0 && value != 1) return -1;

	fd = open (path, O_WRONLY);
	if (fd < 0) {
		//fprintf (stderr, "cannot open %s\n", path);
	} else {
		write (fd, &val, 1);
		close (fd);
	}

	return 0;
}

int32_t drv_read(int32_t fd, void* buf,uint32_t count)
{
	fd_set fs_read;
	struct timeval tv_timeout;
	int32_t retval = 0;

	if(fd < 0) return -1;
    FD_ZERO (&fs_read);
    FD_SET (fd, &fs_read);

    tv_timeout.tv_sec = TIMEOUT_SEC (count, get_baudrate (fd));
    tv_timeout.tv_usec = TIMEOUT_USEC;
    retval = select (fd+1, &fs_read, NULL, NULL, &tv_timeout);
    if (retval > 0) {
        retval = read (fd, buf, count);
        return (retval);
    } else {
        if (0 == retval ) {
            return (0);
        } else {
            return (-1);
        }
    }
}
/*
 * Write datalength bytes in buffer given by UINT8 *data,
 * return value: bytes written
 * Nonblock mode
*/
int32_t drv_write(int32_t fd, const void* buf, uint32_t count)
{
	fd_set fs_write;
	struct timeval tv_timeout;
	int32_t retval;
	uint32_t len = 0, total_len = 0;
	int8_t *data = (int8_t*)buf;

	if(fd < 0) return -1;
    FD_ZERO (&fs_write);
    FD_SET (fd, &fs_write);

    tv_timeout.tv_sec = TIMEOUT_SEC (count, get_baudrate (fd));
    tv_timeout.tv_usec = TIMEOUT_USEC;

    for (total_len = 0, len = 0; total_len < count;) {
        retval = select (fd + 1, NULL, &fs_write, NULL, &tv_timeout);
        if (retval) {
            len = write (fd, &data[total_len], count - total_len);
            if (len > 0) {
                total_len += len;
            }
        } else {
            tcflush (fd, TCOFLUSH);     // flush all output data
            break;
        }
    }

    return (total_len);
}

int Create_Process_ID()
{
	char cmd[128]={0};
	char retmsg[128]={0};
	uint16_t id = 0;
	sprintf(cmd, "pgrep \"LCD_EMS\"");
	super_system(cmd, retmsg, 4, 1);
	id = (uint16_t)strtoul(retmsg,0,10);
	//printf("process id %d\n", id);
	return 0;
}

void System_Reboot()
{
	FILE * fp;
	char cmd[128]={0};
	sprintf(cmd, "reboot");
	if ((fp = popen(cmd, "w") ) == NULL){
		//printf("POPEN error: %s\n", strerror(errno));
		return ;
	}else{
		pclose(fp);
	}
}

/*
 * 得到system命令执行的结果.
 * 注意：此接口只能够获取命令输出的最后一行的信息，若有多行输出信息将不能够全部获取到，
 * 此封装接口只适用于得到命令执行结果的最后一行的信息。
 * flag : 1 -- 读 0 -- 写
 */
static int32_t super_system(const char * cmd, char *retmsg, int32_t msg_len, int flag)
{
	FILE * fp;

	if(flag){
		if (cmd == NULL || retmsg == NULL || msg_len < 0) return -1;
		if ((fp = popen(cmd, "r") ) == NULL){
			//printf("POPEN error: %s\n", strerror(errno));
			return -1;
		}else{
			 //读执行结果
			fread(retmsg, sizeof(char), msg_len, fp);
			//printf("POPEN: %s\n", retmsg);
			pclose(fp);
		}
	}else{
		if (cmd == NULL) return -1;
		if ((fp = popen(cmd, "w") ) == NULL){
			//printf("POPEN error: %s\n", strerror(errno));
			return -1;
		}else{
			pclose(fp);
		}
	}
	return 0;
}

