################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/flows/MacQueue.cpp \
../src/flows/radio-bearer-instance.cpp \
../src/flows/radio-bearer-sink.cpp \
../src/flows/radio-bearer.cpp 

OBJS += \
./src/flows/MacQueue.o \
./src/flows/radio-bearer-instance.o \
./src/flows/radio-bearer-sink.o \
./src/flows/radio-bearer.o 

CPP_DEPS += \
./src/flows/MacQueue.d \
./src/flows/radio-bearer-instance.d \
./src/flows/radio-bearer-sink.d \
./src/flows/radio-bearer.d 


# Each subdirectory must supply rules for building sources it contributes
src/flows/%.o: ../src/flows/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


