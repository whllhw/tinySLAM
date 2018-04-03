################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CoreSLAM.cpp \
../src/CoreSLAM_ext.cpp \
../src/CoreSLAM_loop_closing.cpp \
../src/CoreSLAM_random.cpp \
../src/CoreSLAM_state.cpp \
../src/main.cpp \
../src/read_seria.cpp \
../src/serial.cpp \
../src/test2.cpp 

OBJS += \
./src/CoreSLAM.o \
./src/CoreSLAM_ext.o \
./src/CoreSLAM_loop_closing.o \
./src/CoreSLAM_random.o \
./src/CoreSLAM_state.o \
./src/main.o \
./src/read_seria.o \
./src/serial.o \
./src/test2.o 

CPP_DEPS += \
./src/CoreSLAM.d \
./src/CoreSLAM_ext.d \
./src/CoreSLAM_loop_closing.d \
./src/CoreSLAM_random.d \
./src/CoreSLAM_state.d \
./src/main.d \
./src/read_seria.d \
./src/serial.d \
./src/test2.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


