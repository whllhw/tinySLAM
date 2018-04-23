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


#define uint8_t unsigned char
#define uint32_t unsigned int
#define uint16_t unsigned short int

struct TMP {
	float angle;
	unsigned short lspeed, rspeed;
};
/*
 * 频率：5Hz
 * 传入陀螺仪数据
 * data, len
 * 数据 , 长度
 */
bool read_pr911_pro(char *data, int len, TMP *tmp) {
	if (len != 13) { 		         // 本次数据长度错误
		return false;
	}
	char buf[4] = { 0x55, 0xaa, 0x00, 0xff      // 4 字节帧起始符
			};
	short pos = 0;
	for (; pos < 4; pos++) {
		if (data[pos] != buf[pos])   // 起始符不正确
			return false;
	}
	float angle;
	for (short i = 0; pos < 8; pos++, i++) {
		memset((char *) &angle + i, data[pos], 1);
	}
	unsigned short lspeed, rspeed;
	memset((char *) &lspeed, data[pos++], 1);
	memset((char *) &lspeed + 1, data[pos++], 1);
	memset((char *) &rspeed, data[pos++], 1);
	memset((char *) &rspeed + 1, data[pos++], 1);
	char ch = '\0';
	for (pos = 0; pos < 12; pos++) {
		ch ^= data[pos];
	}
	tmp->angle = angle;
	tmp->lspeed = lspeed;
	tmp->rspeed = rspeed;
	if (ch ^ data[12] == 0x00) {
		for (int i = 0; i < 13; i++) {
			printf("0x%02x ", (unsigned char) data[i]);
		}
		return 0;
	}
	return true;
}

bool read_lds(int usrt_fd) {
	uint8_t temp_char;
	uint8_t start_count = 0;
	bool got_scan = false;
	uint8_t raw_bytes[2520] = { 0 };
	uint8_t good_sets = 0;
	uint32_t motor_speed = 0;
	uint16_t rpms = 0;
	int index;
	int len = 0;
	bool shutting_down_ = false;

	while (!shutting_down_ && !got_scan) {
		// Wait until first data sync of frame: 0xFA, 0xA0
//		boost::asio::read(serial_,
//				boost::asio::buffer(&raw_bytes[start_count], 1));
		len = read(usrt_fd,raw_bytes,1);

		if (start_count == 0) {
			if (raw_bytes[start_count] == 0xFA) {
				start_count = 1;
			}
		} else if (start_count == 1) {
			if (raw_bytes[start_count] == 0xA0) {
				start_count = 0;

				// Now that entire start sequence has been found, read in the rest of the message
				got_scan = true;
//
//				boost::asio::read(serial_,
//						boost::asio::buffer(&raw_bytes[2], 2518));
		        // scan->angle_min = 0.0;
		        // scan->angle_max = 2.0*M_PI;
		        // scan->angle_increment = (2.0*M_PI/360.0);
		        // scan->range_min = 0.12;
		        // scan->range_max = 3.5;
		        // scan->ranges.resize(360);
		        // scan->intensities.resize(360);
				len = read(usrt_fd,&raw_bytes[2],2518);

				//read data in sets of 6
				for (uint16_t i = 0; i < len + 2; i = i + 42) {
					if (raw_bytes[i] == 0xFA
							&& raw_bytes[i + 1] == (0xA0 + i / 42)) //&& CRC check
									{
						good_sets++;
						motor_speed += (raw_bytes[i + 3] << 8)
								+ raw_bytes[i + 2]; //accumulate count for avg. time increment
						rpms = (raw_bytes[i + 3] << 8 | raw_bytes[i + 2]) / 10;

						for (uint16_t j = i + 4; j < i + 40; j = j + 6) {
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

							// scan->ranges[359-index] = range / 1000.0;
							// scan->intensities[359-index] = intensity;
							printf("r[%d]=%f,", 359 - index, range / 1000.0);
						}
					}
				}

				// scan->time_increment = motor_speed/good_sets/1e8;
			} else {
				start_count = 0;
			}
		}
	}
}
}

int main() {
char data[] = { 0x55, 0xaa, 0x00, 0xff, 0x0a, 0xd7, 0xa3, 0x3d, 0x15, 0x00,
		0x16, 0x00, 0xea };
TMP tmp;
if (!read_pr911_pro(data, 13, &tmp))
	printf("%f %d %d\n", tmp.angle, tmp.lspeed, tmp.rspeed);
}

