################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/device/CqiManager/cqi-manager.cpp \
../src/device/CqiManager/fullband-cqi-manager.cpp \
../src/device/CqiManager/wideband-cqi-manager.cpp 

OBJS += \
./src/device/CqiManager/cqi-manager.o \
./src/device/CqiManager/fullband-cqi-manager.o \
./src/device/CqiManager/wideband-cqi-manager.o 

CPP_DEPS += \
./src/device/CqiManager/cqi-manager.d \
./src/device/CqiManager/fullband-cqi-manager.d \
./src/device/CqiManager/wideband-cqi-manager.d 


# Each subdirectory must supply rules for building sources it contributes
src/device/CqiManager/%.o: ../src/device/CqiManager/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


