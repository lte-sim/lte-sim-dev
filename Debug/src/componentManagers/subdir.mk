################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/componentManagers/FlowsManager.cpp \
../src/componentManagers/FrameManager.cpp \
../src/componentManagers/NetworkManager.cpp 

OBJS += \
./src/componentManagers/FlowsManager.o \
./src/componentManagers/FrameManager.o \
./src/componentManagers/NetworkManager.o 

CPP_DEPS += \
./src/componentManagers/FlowsManager.d \
./src/componentManagers/FrameManager.d \
./src/componentManagers/NetworkManager.d 


# Each subdirectory must supply rules for building sources it contributes
src/componentManagers/%.o: ../src/componentManagers/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


