//
// Created by lhw on 18-5-17.
//

#ifndef TINYSLAM_RECV_DATA_H
#define TINYSLAM_RECV_DATA_H

#include <stdint.h>
struct Laser_data
{
	uint16_t ranges[360];		   // 距离信息
	// 720
	// 4
	float time_increment;
};

struct Encoder_data
{
	// 4
	float angle;
	// 2 2
	unsigned short lspeed, rspeed;
};

struct UnionData{
    struct Encoder_data e;
    struct Laser_data l;
    unsigned int time;
};

UnionData pull();

#endif //TINYSLAM_RECV_DATA_H
