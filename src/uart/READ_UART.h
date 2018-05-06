#ifndef __READ_UART_H
#define __READ_UART_H 1

// #define uint8_t unsigned char
// #define uint32_t unsigned int
// #define uint16_t unsigned short int

#include "public.h"

class LDS
{
  private:
	int usrt_fd;
	Laser_data read_lds(int usrt_fd);
	bool shutting_down_;
  public:
	LDS();
	~LDS();
	Laser_data pull();
};

class DY{
	private:
	int usrt_fd;
	bool shutting_down_;
	public:
	DY();
	~DY();
	Encoder_data pull();
};

#endif