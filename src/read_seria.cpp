/*
 * read_seria.cpp
 *
 *  Created on: 2018年3月30日
 *      Author: lhw
 */

#include "serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <signal.h>

// 线程安全
static volatile int keepRunning = 1;

void sig_handler( int sig )
{
    if ( sig == SIGINT)
    {
        keepRunning = 0;
    }
}

#define  BUF_SIZE    1024
void read_usrt (int usrt_fd);
int main0(int argc,char *argv[]){
	signal( SIGINT, sig_handler );
	int usrt_fd,ret,nBaud;
	const char *device;
	if(argc == 1){
		perror("Need params");
		return 0;
	}
	if(strcmp(argv[1],"1")==0){
		device = "/dev/ttyS1";
		nBaud = 230400;
	}else{
		device = "/dev/ttyS2";
		nBaud = 115200;
	}
	printf("%s %d\n",device,nBaud);

	usrt_fd = UART0_Open(device);
	if(usrt_fd <0 )
		printf("Open %s Error.Exit App!",argv[1]);
	do{
		ret = UART0_Init(usrt_fd,nBaud,0,8,1,'N');
		printf("Set Port Exactly!\n");
	}while(-1 == ret );

	printf("send start command\n");
	char c_tmp=0x62;
	for(int i=0;i<3;i++){
		int len=write(usrt_fd,&c_tmp,1);
		if(len<=0)
			printf("Send command err\n");
	}
	read_usrt(usrt_fd);
	return 0;
}

void read_usrt (int usrt_fd)
{
    printf("start read_usrt\n");
    int     n = 0;
    char    buf[BUF_SIZE];
    while(keepRunning){
        memset(buf,0,BUF_SIZE);
        n = read(usrt_fd,buf,BUF_SIZE);
        if(n>0){
        	for(int i=0;i<n;i++){
        		printf("%02x",buf[i]);
        	}
        	printf("\n");
		   if(0==strncmp(buf,"exit",4))
			   break;
        }
    }
    printf("send stop command\n");
	char c_tmp=0x65;
	for(int i=0;i<3;i++){
		int len=write(usrt_fd,&c_tmp,1);
		if(len<=0)
			printf("Send command err\n");
	}

    printf("read_usrt exit...\n");
    UART0_Close(usrt_fd);
}


