/*
 * serial.cpp
 *
 *  Created on: 2018年3月30日
 *      Author: lhw
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<errno.h>
#include<string.h>
#include "serial.h"

#define TRUE  (0)
#define FALSE (-1)

int UART0_Open(const char* port){
	int fd = open( port, O_RDWR|O_NOCTTY|O_NDELAY);
	if (FALSE == fd)
	{
		perror("Can't Open Serial Port");
		return(FALSE);
	}

	if(fcntl(fd, F_SETFL, 0) < 0)
	{
		printf("fcntl failed!\n");
		return(FALSE);
	}
	else
	{
		printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
	}

	if(0 == isatty(STDIN_FILENO))
	{
		printf("standard input is not a terminal device\n");
		return(FALSE);
	}
	else
	{
		printf("isatty success!\n");
	}
	printf("fd->open=%d\n",fd);
	return fd;
}

void UART0_Close(int fd){
	close(fd);
}

int UART0_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)
{

    int   i;
    int   status;
    int   speed_arr[] = {B230400,B115200, B19200, B9600, B4800, B2400, B1200, B300};
    int   name_arr[] = {230400,115200,  19200,  9600,  4800,  2400,  1200,  300};

    struct termios options;
    if( tcgetattr( fd,&options)  !=  0)
	{
		perror("SetupSerial 1");
		return(FALSE);
	}


	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
	{
		if  (speed == name_arr[i])
		{
			cfsetispeed(&options, speed_arr[i]);
			cfsetospeed(&options, speed_arr[i]);
			printf("set  speed=%d,speed-arr=%d\r\n",speed,speed_arr[i]);
		}
	}
	options.c_cflag |= CLOCAL;
	options.c_cflag |= CREAD;
	switch(flow_ctrl)
	{

		case 0 :
			  options.c_cflag &= ~CRTSCTS;
			  break;
		case 1 :
			  options.c_cflag |= CRTSCTS;
			  break;
		case 2 :
			  options.c_cflag |= IXON | IXOFF | IXANY;
			  break;
	}

	    options.c_cflag &= ~CSIZE;
	    switch (databits)
	    {
	        case 5    :
	                     options.c_cflag |= CS5;
	                     break;
	        case 6    :
	                     options.c_cflag |= CS6;
	                     break;
	        case 7    :
	                 options.c_cflag |= CS7;
	                 break;
	        case 8:
	                 options.c_cflag |= CS8;
	                 break;
	        default:
	                 fprintf(stderr,"Unsupported data size\n");
	                 return (FALSE);
	    }

	    switch (parity)
	    {
	        case 'n':
	        case 'N':
	                 options.c_cflag &= ~PARENB;
	                 options.c_iflag &= ~INPCK;
	                 break;
	        case 'o':
	        case 'O':
	                 options.c_cflag |= (PARODD | PARENB);
	                 options.c_iflag |= INPCK;
	                 break;
	        case 'e':
	        case 'E':
	                 options.c_cflag |= PARENB;
	                 options.c_cflag &= ~PARODD;
	                 options.c_iflag |= INPCK;
	                 break;
	        case 's':
	        case 'S':
	                 options.c_cflag &= ~PARENB;
	                 options.c_cflag &= ~CSTOPB;
	                 break;
	        default:
	                 fprintf(stderr,"Unsupported parity\n");
	                 return (FALSE);
	    }
	    switch (stopbits)
	    {
	        case 1:
	                 options.c_cflag &= ~CSTOPB; break;
	        case 2:
	                 options.c_cflag |= CSTOPB; break;
	        default:
	                       fprintf(stderr,"Unsupported stop bits\n");
	                       return (FALSE);
	    }

	    // 参考于IBM上的文章，下面的两个flag是设置原始模式进行通讯，(Raw Mode)
	    options.c_oflag &= ~OPOST;

	    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	    //options.c_lflag &= ~(ISIG | ICANON);

		/*设置等待时间和最小接收字符*/
	    options.c_cc[VTIME] = 1;
	    //最小等待字符
	    options.c_cc[VMIN] = 1;

	    tcflush(fd,TCIFLUSH);


	    if (tcsetattr(fd,TCSANOW,&options) != 0)
	    {
	        perror("com set error!\n");
	        return (FALSE);
	    }
	    return (TRUE);
	}

//int UART0_Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity)
int UART0_Init(int fd,int speed)
{
    if (UART0_Set(fd,speed,0,8,1,'N') == FALSE)
	{
		return FALSE;
	}
	else
	{
		return  TRUE;
	}
}

int UART0_Recv(int fd, unsigned char *rcv_buf,int data_len)
{
    int len,fs_sel;
    fd_set fs_read;

    struct timeval time;

    FD_ZERO(&fs_read);
    FD_SET(fd,&fs_read);

//    time.tv_sec = 10;
    time.tv_sec = 1;
    time.tv_usec = 0;
    fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);
//	printf("fs_sel = %d\n",fs_sel);
	if(fs_sel)
	{
		len = read(fd,rcv_buf,data_len);
//		printf("I am right!(version1.2) len = %d fs_sel = %d\n",len,fs_sel);
		return len;
	}
	else
	{
		printf("Sorry,I am wrong!\n");
		return FALSE;
	}
}


int UART0_Send(int fd, char *send_buf,int data_len)
{
    int len = 0;
    /*
    len = write(fd,send_buf,data_len);
	*/
	char c_tmp='b';
	int d_len=1;
	len=write(fd,&c_tmp,1);
    if (len == d_len )
    {
        printf("send data is %s\n",send_buf);
        return len;
    }
    else
    {
        tcflush(fd,TCOFLUSH);
        return FALSE;
    }
}
