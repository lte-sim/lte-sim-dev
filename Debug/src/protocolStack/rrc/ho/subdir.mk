################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/protocolStack/rrc/ho/handover-entity.cpp \
../src/protocolStack/rrc/ho/ho-manager.cpp \
../src/protocolStack/rrc/ho/position-based-ho-manager.cpp \
../src/protocolStack/rrc/ho/power-based-ho-manager.cpp 

OBJS += \
./src/protocolStack/rrc/ho/handover-entity.o \
./src/protocolStack/rrc/ho/ho-manager.o \
./src/protocolStack/rrc/ho/position-based-ho-manager.o \
./src/protocolStack/rrc/ho/power-based-ho-manager.o 

CPP_DEPS += \
./src/protocolStack/rrc/ho/handover-entity.d \
./src/protocolStack/rrc/ho/ho-manager.d \
./src/protocolStack/rrc/ho/position-based-ho-manager.d \
./src/protocolStack/rrc/ho/power-based-ho-manager.d 


# Each subdirectory must supply rules for building sources it contributes
src/protocolStack/rrc/ho/%.o: ../src/protocolStack/rrc/ho/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


