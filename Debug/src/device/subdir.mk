################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/device/ENodeB.cpp \
../src/device/Gateway.cpp \
../src/device/HeNodeB.cpp \
../src/device/NetworkNode.cpp \
../src/device/UserEquipment.cpp 

OBJS += \
./src/device/ENodeB.o \
./src/device/Gateway.o \
./src/device/HeNodeB.o \
./src/device/NetworkNode.o \
./src/device/UserEquipment.o 

CPP_DEPS += \
./src/device/ENodeB.d \
./src/device/Gateway.d \
./src/device/HeNodeB.d \
./src/device/NetworkNode.d \
./src/device/UserEquipment.d 


# Each subdirectory must supply rules for building sources it contributes
src/device/%.o: ../src/device/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


