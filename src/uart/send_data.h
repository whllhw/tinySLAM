#ifndef __SEND_DATA_H
#define __SEND_DATA_H 1
#include "bridge.h"
void push_sensor_data(Encoder_data e,Laser_data l,unsigned long time);
void setdown();
#endif