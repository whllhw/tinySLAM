#!/bin/bash
g++ -c READ_UART.cpp 
g++ -c serial.cpp 
g++ -c bridge.cpp
ar -crs libctest.so READ_UART.o serial.o bridge.o
rm *.o