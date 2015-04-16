################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/protocolStack/protocols/TransportProtocol.cpp 

OBJS += \
./src/protocolStack/protocols/TransportProtocol.o 

CPP_DEPS += \
./src/protocolStack/protocols/TransportProtocol.d 


# Each subdirectory must supply rules for building sources it contributes
src/protocolStack/protocols/%.o: ../src/protocolStack/protocols/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


