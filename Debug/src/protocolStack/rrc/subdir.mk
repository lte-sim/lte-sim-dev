################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/protocolStack/rrc/rrc-entity.cc 

OBJS += \
./src/protocolStack/rrc/rrc-entity.o 

CC_DEPS += \
./src/protocolStack/rrc/rrc-entity.d 


# Each subdirectory must supply rules for building sources it contributes
src/protocolStack/rrc/%.o: ../src/protocolStack/rrc/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


