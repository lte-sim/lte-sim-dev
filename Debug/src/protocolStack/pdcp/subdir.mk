################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/protocolStack/pdcp/pdcp-entity.cpp 

OBJS += \
./src/protocolStack/pdcp/pdcp-entity.o 

CPP_DEPS += \
./src/protocolStack/pdcp/pdcp-entity.d 


# Each subdirectory must supply rules for building sources it contributes
src/protocolStack/pdcp/%.o: ../src/protocolStack/pdcp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


