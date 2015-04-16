################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/core/spectrum/bandwidth-manager.cpp \
../src/core/spectrum/transmitted-signal.cpp 

OBJS += \
./src/core/spectrum/bandwidth-manager.o \
./src/core/spectrum/transmitted-signal.o 

CPP_DEPS += \
./src/core/spectrum/bandwidth-manager.d \
./src/core/spectrum/transmitted-signal.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/spectrum/%.o: ../src/core/spectrum/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


