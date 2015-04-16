################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/protocolStack/packet/Header.cpp \
../src/protocolStack/packet/Packet.cpp \
../src/protocolStack/packet/PacketTAGs.cpp 

CC_SRCS += \
../src/protocolStack/packet/packet-burst.cc 

OBJS += \
./src/protocolStack/packet/Header.o \
./src/protocolStack/packet/Packet.o \
./src/protocolStack/packet/PacketTAGs.o \
./src/protocolStack/packet/packet-burst.o 

CC_DEPS += \
./src/protocolStack/packet/packet-burst.d 

CPP_DEPS += \
./src/protocolStack/packet/Header.d \
./src/protocolStack/packet/Packet.d \
./src/protocolStack/packet/PacketTAGs.d 


# Each subdirectory must supply rules for building sources it contributes
src/protocolStack/packet/%.o: ../src/protocolStack/packet/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/protocolStack/packet/%.o: ../src/protocolStack/packet/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


