################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/mobility/ConstantPosition.cpp \
../src/mobility/Manhattan.cpp \
../src/mobility/Mobility.cpp \
../src/mobility/RandomDirection.cpp \
../src/mobility/RandomWalk.cpp \
../src/mobility/RandomWaypoint.cpp 

OBJS += \
./src/mobility/ConstantPosition.o \
./src/mobility/Manhattan.o \
./src/mobility/Mobility.o \
./src/mobility/RandomDirection.o \
./src/mobility/RandomWalk.o \
./src/mobility/RandomWaypoint.o 

CPP_DEPS += \
./src/mobility/ConstantPosition.d \
./src/mobility/Manhattan.d \
./src/mobility/Mobility.d \
./src/mobility/RandomDirection.d \
./src/mobility/RandomWalk.d \
./src/mobility/RandomWaypoint.d 


# Each subdirectory must supply rules for building sources it contributes
src/mobility/%.o: ../src/mobility/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


