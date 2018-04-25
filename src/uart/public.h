#ifndef __PUBILIC_H
#define __PUBILIC_H 1
#include <time.h>
#include <stdint.h>

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
#endif