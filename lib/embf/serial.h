#ifndef _SERIAL_H
#define _SERIAL_H

/*
 * Decription for TIMEOUT_SEC(buflen,baud);
 * baud bits per second, buflen bytes to send.
 * buflen*20 (20 means sending an octect-bit data by use of the maxim bits 20)
 * eg. 9600bps baudrate, buflen=1024B, then TIMEOUT_SEC = 1024*20/9600+1 = 3
 * don't change the two lines below unless you do know what you are doing.
*/
#define TIMEOUT_SEC(buflen,baud) (buflen*20/baud+2)
#define TIMEOUT_USEC 0

#define CH_TO_WAIT 5
#define CH_BITS 11

int32_t	get_baudrate(int32_t fd);
void set_baudrate (int32_t);
int32_t	set_port_attr (int32_t fd,int32_t baudrate,int32_t databit,const char *stopbit,char parity,uint16_t vmin);
int32_t set_uart_stopbit(int32_t fd,int32_t stopbit);
int32_t set_uart_data_bit(int32_t fd,int32_t databit);
int32_t set_uart_parity(int32_t fd,int32_t parity);

extern struct termios termios_old, termios_new;
#endif

