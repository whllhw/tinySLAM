/*
 * serical.h
 *
 *  Created on: 2018年3月30日
 *      Author: lhw
 */

#ifndef SRC_SERIAL_H_
#define SRC_SERIAL_H_

extern int UART0_Open(const char* port);
extern void UART0_Close(int fd);
//extern int UART0_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity);
extern int UART0_Init(int fd, int speed);//,int flow_ctrl,int databits,int stopbits,int parity);
extern int UART0_Init(int fd,int speed);


#endif /* SRC_SERIAL_H_ */
