cmake_minimum_required(VERSION 3.10)
PROJECT(uart)
INCLUDE_DIRECTORIES(.)
AUX_SOURCE_DIRECTORY(. DIR_SRCS) # source code
SET(x ${DIR_SRCS}) # set env var
#find_package(Threads)
#target_link_libraries(uart "${CMAKE_THREAD_LIBS_INIT}") # -lpthread
ADD_EXECUTABLE(${PROJECT_NAME} ${x})
#SET(CMAKE_C_COMPILER "/home/lhw/tina/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-gcc")
#SET(CMAKE_CXX_COMPILER "/home/lhw/tina/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-g++")