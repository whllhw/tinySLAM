/*
 * read_PR911_PRO.cpp
 *
 *  Created on: 2018年4月19日
 *      Author: lhw
 */
#include <string.h>
#include <stdio.h>
#include "serial.h"
#include <unistd.h>
#include <ctime>
#include <cmath>

#define uint8_t unsigned char
#define uint32_t unsigned int
#define uint16_t unsigned short int

struct SCAN
{
	float angle_min;
	float angle_max;
	float angle_increment;
	float range_min;
	float range_max;
	float ranges[360];		   // 距离信息
	uint16_t intensities[360]; // 强度信息
	float time_increment;
	time_t t; // 采集数据的时间
};

struct Dynamixel
{
	float angle;
	unsigned short lspeed, rspeed;
	time_t t; // 采集数据的时间
};
/*
 * 返回 true 时，数据可用，否则不正确
 * 频率：5Hz
 * 传入陀螺仪数据
 * data, len
 * 数据 , 长度
 */
class DY
{
  private:
	int usrt_fd;
	Dynamixel read_pr911_pro(int usrt_fd)
	{
		char data[13];
		Dynamixel dy;
		char buf[4] = {
			0x55, 0xaa, 0x00, 0xff // 4 字节帧起始符
		};
		while (true)
		{
			int len = read(usrt_fd, data, 4);
			if (buf[0] != data[0] &&
				buf[1] != data[1] &&
				buf[2] != data[2] &&
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
			char ch = '\0';
			for (pos = 0; pos < 12; pos++) // 累加和校验
				num = (num + data[pos]) % 0xffff;
			ch = (char)(num & 0xff);
			dy.angle = angle;
			dy.lspeed = lspeed;
			dy.rspeed = rspeed;
			time(&(dy.t));
			if (ch == data[12])
				return dy;
			// printf("%02x",(unsigned char)ch);
		}
	}

  public:
	DY()
	{
		int usrt_fd = UART0_Open("/dev/ttyS1");
		UART0_Init(usrt_fd, 230400);
	}
	~DY()
	{
		UART0_Close(usrt_fd);
	}
	Dynamixel pull()
	{
		return this->read_pr911_pro(usrt_fd);
	}
};
class LDS
{
  private:
	int usrt_fd;
	SCAN read_lds(int usrt_fd)
	{
		uint8_t temp_char;
		uint8_t start_count = 0;
		bool got_scan = false;
		uint8_t raw_bytes[2520] = {0};
		uint8_t good_sets = 0;
		uint32_t motor_speed = 0;
		uint16_t rpms = 0;
		int index;
		int len = 0;
		bool shutting_down_ = false;
		SCAN s = SCAN();
		SCAN *scan = &s;
		while (!shutting_down_ && !got_scan)
		{
			// Wait until first data sync of frame: 0xFA, 0xA0
			//		boost::asio::read(serial_,
			//				boost::asio::buffer(&raw_bytes[start_count], 1));
			len = read(usrt_fd, raw_bytes, 1);

			if (start_count == 0)
			{
				if (raw_bytes[start_count] == 0xFA)
				{
					start_count = 1;
				}
			}
			else if (start_count == 1)
			{
				if (raw_bytes[start_count] == 0xA0)
				{
					start_count = 0;

					// Now that entire start sequence has been found, read in the rest of the message
					got_scan = true;
					//
					//				boost::asio::read(serial_,
					//						boost::asio::buffer(&raw_bytes[2], 2518));
					scan->angle_min = 0.0;
					scan->angle_max = 2.0 * M_PI;
					scan->angle_increment = (2.0 * M_PI / 360.0);
					scan->range_min = 0.12;
					scan->range_max = 3.5;
					//  scan->ranges.resize(360);
					//  scan->intensities.resize(360);
					// 42*60 = 2520
					// 一次性读入 60组数据进来，每组有6个角度的信息，总共是360度的信息
					len = read(usrt_fd, &raw_bytes[2], 2518);
					//read data in sets of 6
					for (uint16_t i = 0; i < len + 2; i = i + 42)
					{
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
								printf("r[%d]=%f,", 359 - index, range / 1000.0);
							}
						}
					}

					scan->time_increment = motor_speed / good_sets / 1e8;
					time(&(scan->t));
				}
				else
				{
					start_count = 0;
				}
			}
		}
		return s;
	}

  public:
	LDS()
	{ // 发生开始命令
		this->usrt_fd = UART0_Open("/dev/ttyS2");
		UART0_Init(usrt_fd, 115200);
		char c_tmp = 0x62;
		for (int i = 0; i < 3; i++)
		{
			int len = write(usrt_fd, &c_tmp, 1);
			if (len <= 0)
				printf("Send command err\n");
		}
	}
	~LDS()
	{ // 发生结束命令
		char c_tmp = 0x65;
		for (int i = 0; i < 3; i++)
		{
			int len = write(usrt_fd, &c_tmp, 1);
			if (len <= 0)
				printf("Send command err\n");
		}
		UART0_Close(usrt_fd);
	}
	SCAN pull()
	{
		return this->read_lds(usrt_fd);
	}
};
int main()
{
	char data[] = {0x55, 0xaa, 0x00, 0xff, 0x0a, 0xd7, 0xa3, 0x3d, 0x15, 0x00,
				   0x16, 0x00, 0xea};
}
