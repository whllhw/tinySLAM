#ifndef __PUBILIC_H
#define __PUBILIC_H 1

#include <stdint.h>

struct Laser_data
{
//	float angle_min;
//	float angle_max;
//	float angle_increment;
//	float range_min;
//	float range_max;
	uint16_t ranges[360];		   // 距离信息
	uint16_t intensities[360]; // 强度信息
	float time_increment;
	long t; // 采集数据的时间
};

struct Encoder_data
{
	float angle;
	unsigned short lspeed, rspeed;
	long t; // 采集数据的时间
};
#endif