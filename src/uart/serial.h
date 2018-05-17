/*
 * serical.h
 *
 *  Created on: 2018年3月30日
 *      Author: lhw
 */

#ifndef SRC_SERIAL_H_
#define SRC_SERIAL_H_ 1

 int UART0_Open(const char* port);
 void UART0_Close(int fd);
//extern int UART0_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity);
//extern int UART0_Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity);
 int UART0_Init(int fd,int speed);
 int UART0_Recv(int fd, unsigned char *rcv_buf,int data_len);
 int UART0_Send(int fd, char *send_buf,int data_len);
 int UART0_flush(int fd);
 bool UART0_CanRead(int fd);
#endif /* SRC_SERIAL_H_ */
