/*
 * read_PR911_PRO.cpp
 *
 *  Created on: 2018年4月19日
 *      Author: lhw
 */
#include "serial.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <cmath>
#include "READ_UART.h"
#include <fcntl.h>

/*
 * 返回 true 时，数据可用，否则不正确
 * 频率：5Hz
 * 传入陀螺仪数据
 * data, len
 * 数据 , 长度
 */

Encoder_data DY::read_pr911_pro(int usrt_fd)
{
	Encoder_data dy;
	uint8_t buf[4] = {
		0x55, 0xaa, 0x00, 0xff // 4 字节帧起始符
	};
	ssize_t len;
	while (true)
	{
		uint8_t data[13] = {0};
		len = read(usrt_fd, data, 4);
		if (buf[0] != data[0] ||
			buf[1] != data[1] ||
			buf[2] != data[2] ||
			buf[3] != data[3])
			continue;
		float angle;
		short pos = 4;
		len = read(usrt_fd, &data[4], 9);
		if (len != 9)
			continue;
		for (short i = 0; pos < 8; pos++, i++)
		{
			memset((char *)&angle + i, data[pos], 1);
		}
		unsigned short lspeed, rspeed;
		memset((char *)&lspeed, data[pos++], 1);
		memset((char *)&lspeed + 1, data[pos++], 1);
		memset((char *)&rspeed, data[pos++], 1);
		memset((char *)&rspeed + 1, data[pos++], 1);
		int num = 0;
		uint8_t ch = '\0';
		for (pos = 0; pos < 12; pos++) // 累加和校验
			num = (num + data[pos]) % 0xffff;
		ch = (uint8_t)(num & 0xff);
		dy.angle = angle;
		dy.lspeed = lspeed;
		dy.rspeed = rspeed;
		time(&(dy.t));
		if (ch == data[12])
		{
			// printf("crc ok!\n");
			return dy;
		}
		printf("error on DY::read_pr911_pro , crc check error: %02x\n",(unsigned char)ch);
	}
}
DY::DY()
{
	int ret;
	// printf("now in DY::DY()\n");
	this->usrt_fd = UART0_Open("/dev/ttyS2");
	// printf("setup!port\n");
	if (usrt_fd < 0)
		printf("Open [DY] Error.Exit App!");
	do
	{
		ret = UART0_Init(usrt_fd, 115200);
		printf("Set Port Exactly!\n");
	} while (-1 == ret);
	// printf("now exit DY::DY()!\n");
}
DY::~DY()
{
	UART0_Close(usrt_fd);
	printf("closed [DY] fd = %d\n", usrt_fd);
}
Encoder_data DY::pull()
{
	return this->read_pr911_pro(usrt_fd);
}
/* 一些获取到的数据实例：
// 正常的数据 42byte
fa
c0
af0b
521da5002400
391ea5002400
761ea6002400
1a1ea6002400
7e1da6002400
871da7002400
ffff
// 正常的数据 42byte
fa
ab
b50b
c215e6001b00
8f16e4001c00
9216e2001c00
7916e0001c00
0f16de001c00
3717dc001c00
8787
// 会有缺失的数据（缺失校验码） 40byte
fa
bd
af0b
761ca8002400
381da8002400
451ca7002400
7b1da7002400
0a1da6002400
931ca6002400
				*/
LDS::LDS()
{ // 发生开始命令
	this->usrt_fd = UART0_Open("/dev/ttyS1");
	if (usrt_fd < 0)
		printf("Open [LDS] Error.Exit App!");
	int ret;
	do
	{
		ret = UART0_Init(usrt_fd, 230400);
		printf("Set Port Exactly!\n");
	} while (-1 == ret);
	char c_tmp = 0x62;
	for (int i = 0; i < 3; i++)
	{
		int len = write(usrt_fd, &c_tmp, 1);
		if (len <= 0)
			printf("Send command err\n");
	}
//
//		this->usrt_fd = open("../uart_data", O_RDONLY);
//		if (usrt_fd <= 0)
//			printf("can't open file !\n");
		printf("now exit LDS::LDS()\n");
}
LDS::~LDS()
{ // 发生结束命令
	this->shutting_down_ = true;
	char c_tmp = 0x65;
	for (int i = 0; i < 3; i++)
	{
		int len = write(usrt_fd, &c_tmp, 1);
		if (len <= 0)
			printf("Send command err\n");
	}
	UART0_Close(usrt_fd);
	printf("closed [LDS] fd = %d\n", usrt_fd);
}
Laser_data LDS::pull()
{
	return this->read_lds(usrt_fd);
}
Laser_data LDS::read_lds(int usrt_fd)
{
	uint8_t good_sets = 0;
	uint32_t motor_speed = 0;
	uint16_t rpms = 0;
	this->shutting_down_ = false;
	Laser_data s = Laser_data();
	Laser_data *scan = &s;

	bool flag = false;
	uint16_t good = 0,ret,count = 0,index = 0, len = 0;
	static uint8_t raw_bytes[2520],last,temp;
	static bool last_read_a0;
    memset(raw_bytes,0,sizeof(raw_bytes));
	while (len < 2520 && !shutting_down_)
	{
	    if(last_read_a0){
	        temp = 0xfa;
	        printf("last read 0xfa!");
	    }else{
//	        ret = read(usrt_fd, &temp, 1);
			ret = UART0_Recv(usrt_fd,&temp,1);
//	        printf("%02x ",temp);
	        if (ret <= 0) {
                printf("read error:%d \n", ret);
                break;
            }
	    }
		if (temp == 0xfa)
		{
			flag = false;
			if(last_read_a0){
			    temp = 0xa0;
			    printf("last read 0xa0!");
			}else{
//			    ret = read(usrt_fd, &temp, 1);
				ret = UART0_Recv(usrt_fd,&temp,1);
//			    printf("%02x ",temp);
			    if (ret <= 0)
                {
                    printf("read error:%d \n", ret);
                    break;
                }
			}
			if (temp == 0xa0)
			{
			    last_read_a0 = false;
				if (index > 2518 || len > 0){
				    last_read_a0 = true;
				    break;
				}
				raw_bytes[index] = 0xfa;
				raw_bytes[index + 1] = 0xa0;
				len = index + 2;
				index += 42;
				last = 0xa0;
				good = 0;
				count++;
				good++;
			}
			else if (temp < 0xdc && 0xa0 < temp)
			{
				if (index > 2518)
					break;
				raw_bytes[index] = 0xfa;
				raw_bytes[index + 1] = temp;
				len = index + 2;
				if (temp > last)
				{
					index += 42 * (temp - last);
					last = temp;
					good++;
				}
				else
				{
					flag = true;
				}
			}
		}
		else if (!flag)
		{
			raw_bytes[len++] = temp;
		}
	}
	// // 已经固定的激光型号？？
	// scan->angle_min = 0.0;
	// scan->angle_max = 2.0 * M_PI;
	// scan->angle_increment = (2.0 * M_PI / 360.0);
	// scan->range_min = 0.12;
	// scan->range_max = 3.5;
	//  scan->ranges.resize(360);
	//  scan->intensities.resize(360);

	// 42*60 = 2520
	// 一次性读入 60组数据进来，每组有6个角度的信息，总共是360度的信息

	//read data in sets of 6
	for (uint16_t i = 0; i < len; i = i + 42)
	{
		//			printf("%02x,%02x\n", raw_bytes[i], raw_bytes[i + 1]);
		if (raw_bytes[i] == 0xFA && raw_bytes[i + 1] == (0xA0 + i / 42)) //&& CRC check
		{
			good_sets++;
			motor_speed += (raw_bytes[i + 3] << 8) + raw_bytes[i + 2]; //accumulate count for avg. time increment
			rpms = (raw_bytes[i + 3] << 8 | raw_bytes[i + 2]) / 10;
			for (uint16_t j = i + 4; j < i + 40; j = j + 6)
			{
				index = 6 * (i / 42) + (j - 4 - i) / 6;

				// Four bytes per reading
				uint8_t byte0 = raw_bytes[j];
				uint8_t byte1 = raw_bytes[j + 1];
				uint8_t byte2 = raw_bytes[j + 2];
				uint8_t byte3 = raw_bytes[j + 3];

				// Remaining bits are the range in mm
				uint16_t intensity = (byte1 << 8) + byte0;

				// Last two bytes represent the uncertanty or intensity, might also be pixel area of target...
				// uint16_t intensity = (byte3 << 8) + byte2;
				uint16_t range = (byte3 << 8) + byte2;
				scan->ranges[359 - index] = range / 1000.0;
				scan->intensities[359 - index] = intensity;
//				printf("r[%d]=%f,", 359 - index, range / 1000.0);
//				printf("%f ",range/1000.0);
			}
		}
	}
	if(good_sets == 0)
		printf("x / 0 error!! in READ_UART.cpp good_set =  %d", good_sets);
	else
	    scan->time_increment = motor_speed / good_sets / 1e8;
	time(&(scan->t));
	//printf("\ncount:%d,good:%d good_set:%d time:%ld\n---------------------------------------------\n",count,good,good_sets,scan->t);
	return s;
}