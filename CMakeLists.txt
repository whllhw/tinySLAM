cmake_minimum_required(VERSION 3.10)
#CMAKE_POLICY( SET CMP0003 OLD )
PROJECT(tinyslam)# name of this project
AUX_SOURCE_DIRECTORY(src DIR_SRCS) # source code
SET(x ${DIR_SRCS}) # set env var
find_package(SDL)
#find_library(SDL)
#find_library(SDL_gfx)
#link_directories(${LINK_DIR})# .so .dll .dylib 相当于gcc 中的-L参数
#include_directories(${INC_DIR})# 相当于gcc/clang 中的-I(i的大写字母)参数
ADD_EXECUTABLE(${PROJECT_NAME} ${x})
target_link_libraries(
        ${PROJECT_NAME}
        SDL
        SDL_gfx
        curses
) # -l
#SET(CMAKE_C_COMPILER "/home/lhw/tina/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-gcc")
#SET(CMAKE_CXX_COMPILER "/home/lhw/tina/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-musl/toolchain/bin/arm-openwrt-linux-g++")