/*
 * Program:     serial.c
 * Author:      lk
 * Version:     1.0.0
 * Date:        2017-01-04
 * Description: To provide underlying serial port function, for high level applications.
 *
*/

#include "emfcntl.h"
#include <termios.h>            /* tcgetattr, tcsetattr */
#include <stdio.h>              /* perror, printf, puts, fprintf, fputs */
#include <unistd.h>             /* read, write, close */
#include <fcntl.h>              /* open */
#include "serial.h"

struct termios termios_old, termios_new;

static int32_t	baudrate2Bxx (int32_t baudrate);
static int32_t	Bxx2baudrate (int32_t _baudrate);
static void	set_parity (char parity);
static void	set_data_bit (int32_t databit);
static void	set_stopbit (const char *stopbit);

/* get serial port baudrate */
int32_t get_baudrate (int32_t fd)
{
	int32_t baud=0;

    bzero(&termios_old, sizeof (termios_old));
	tcgetattr(fd, &termios_old);
	baud=Bxx2baudrate (cfgetospeed (&termios_old));
	//printf("get baud %d\n", baud);

    return baud;
}

/* set serial port baudrate by use of file descriptor fd */
void set_baudrate (int32_t baudrate)
{
	//printf("set baud %d\n", baudrate);
    termios_new.c_cflag = baudrate2Bxx(baudrate);  /* set baudrate */
}

static void set_data_bit (int32_t databit)
{
    termios_new.c_cflag &= ~CSIZE;
    switch (databit) {
    case 8:
        termios_new.c_cflag |= CS8;
        break;
    case 7:
        termios_new.c_cflag |= CS7;
        break;
    case 6:
        termios_new.c_cflag |= CS6;
        break;
    case 5:
        termios_new.c_cflag |= CS5;
        break;
    default:
        termios_new.c_cflag |= CS8;
        break;
    }
}

static void set_stopbit (const char *stopbit)
{
    if (0 == strcmp (stopbit, "1")) {
        termios_new.c_cflag &= ~CSTOPB; /* 1 stop bit */
    }
    else if (0 == strcmp (stopbit, "1.5")) {
        termios_new.c_cflag &= ~CSTOPB; /* 1.5 stop bits */
    }
    else if (0 == strcmp (stopbit, "2")) {
        termios_new.c_cflag |= CSTOPB;  /* 2 stop bits */
    }
    else {
        termios_new.c_cflag &= ~CSTOPB; /* 1 stop bit */
    }
}

static void set_parity (char parity)
{
    switch (parity) {
    case 'N':                  /* no parity check */
        termios_new.c_cflag &= ~PARENB;
        break;
    case 'E':                  /* even */
        termios_new.c_cflag |= PARENB;
        termios_new.c_cflag &= ~PARODD;
        break;
    case 'O':                  /* odd */
        termios_new.c_cflag |= PARENB;
        termios_new.c_cflag |= PARODD;
        break;
    default:                   /* no parity check */
        termios_new.c_cflag &= ~PARENB;
        break;
    }
}

//设置串口参赛数
int32_t set_port_attr (
						int32_t fd,
						int32_t baudrate,        	// 1200 2400 4800 9600 .. 115200
						int32_t databit,           	// 5, 6, 7, 8
                        const char *stopbit,  		//  "1", "1.5", "2"
                        char parity,              	// N(o), O(dd), E(ven)
                        uint16_t vmin)
{
    bzero(&termios_new, sizeof (termios_new));
	cfmakeraw (&termios_new);

	set_baudrate (baudrate);
    set_data_bit (databit);
    set_parity (parity);
    set_stopbit (stopbit);

    termios_new.c_cflag |= CLOCAL | CREAD;  //启动接收同时忽略串口信号线的状态
    //termios_new.c_oflag 			= 0;  //关闭输出处理
    termios_new.c_oflag 			&= ~(OPOST|ONLCR|OCRNL);
    //termios_new.c_lflag 			= 0;
    termios_new.c_lflag 			&= ~(ICANON|ECHO|ECHOE|ISIG); //设置串口工作在原始模式（非终端模式）
    //termios_new.c_iflag			= 0; //关闭输入处理
    termios_new.c_iflag	  			&= ~(ICRNL|INLCR|IGNCR|IXON|IXOFF|IXANY);

    //MIN==0,TIME==0时，如果数据可用，read最多返回要求的字节数，如果无数据可用，立即返回。
    termios_new.c_cc[VTIME] = 1;           //单位: 10ms， 收到最小长度的超时时间 (修改后的内核)
    termios_new.c_cc[VMIN]  = vmin;        //255;  //返回的最大字节数
    tcflush (fd, TCIFLUSH);

	return (tcsetattr (fd, TCSANOW, &termios_new)); //参数即刻生效
}

int32_t set_uart_stopbit(
						int32_t fd,
						int32_t stopbit  		// 1 --  "1",  2 -- "1.5", 3 --  "2"
					)
{
    bzero(&termios_new, sizeof (termios_new));
	tcgetattr(fd, &termios_new);
	if(stopbit==1)set_stopbit ("1");
	else if(stopbit==2)set_stopbit ("1.5");
	else if(stopbit==3)set_stopbit ("2");
    tcflush (fd, TCIFLUSH);
	return (tcsetattr (fd, TCSANOW, &termios_new)); //参数即刻生效
}

int32_t set_uart_data_bit(
						int32_t fd,
						int32_t databit           	// 5, 6, 7, 8
					)
{
    bzero(&termios_new, sizeof (termios_new));
	tcgetattr(fd, &termios_new);
	set_data_bit (databit);
    tcflush (fd, TCIFLUSH);
	return (tcsetattr (fd, TCSANOW, &termios_new)); //参数即刻生效
}

int32_t set_uart_parity(
						int32_t fd,
						int32_t parity              	// 1 -- N(o), 2 -- O(dd),  3 -- E(ven)
					)
{
    bzero(&termios_new, sizeof (termios_new));
	tcgetattr(fd, &termios_new);
	if(parity==1)set_parity ('N');
	else if(parity==2)set_parity ('O');
	else if(parity==3)set_parity ('E');
    tcflush (fd, TCIFLUSH);
	return (tcsetattr (fd, TCSANOW, &termios_new)); //参数即刻生效
}


/**
 * baudrate xxx to Bxxx
 * 
 * @@param baudrate xxx
 * 
 * @@return 
 */
static int32_t baudrate2Bxx (int32_t baudrate)
{
    switch (baudrate) {
    case 0:
        return (B0);
    case 50:
        return (B50);
    case 75:
        return (B75);
    case 110:
        return (B110);
    case 134:
        return (B134);
    case 150:
        return (B150);
    case 200:
        return (B200);
    case 300:
        return (B300);
    case 600:
        return (B600);
    case 1200:
        return (B1200);
    case 2400:
        return (B2400);
    case 4800:
        return (B4800);
    case 9600:
        return (B9600);
    case 19200:
        return (B19200);
    case 38400:
        return (B38400);
    case 57600:
        return (B57600);
    case 115200:
        return (B115200);
    default:
        return (B9600);
    }
}

/**
 * get boundrate from Bxxx
 * 
 * @@param baudrate Bxxx refers to bound rate
 * 
 * @@return 
 */
static int32_t Bxx2baudrate (int32_t _baudrate)
{
/* reverse baudrate */
    switch (_baudrate) {
    case B0:
        return (0);
    case B50:
        return (50);
    case B75:
        return (75);
    case B110:
        return (110);
    case B134:
        return (134);
    case B150:
        return (150);
    case B200:
        return (200);
    case B300:
        return (300);
    case B600:
        return (600);
    case B1200:
        return (1200);
    case B2400:
        return (2400);
    case B4800:
        return (4800);
    case B9600:
        return (9600);
    case B19200:
        return (19200);
    case B38400:
        return (38400);
    case B57600:
        return (57600);
    case B115200:
        return (115200);
    default:
        return (9600);
    }
}


