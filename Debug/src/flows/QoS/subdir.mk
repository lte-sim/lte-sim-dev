################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/flows/QoS/QoSForEXP.cpp \
../src/flows/QoS/QoSForFLS.cpp \
../src/flows/QoS/QoSForM_LWDF.cpp \
../src/flows/QoS/QoSParameters.cpp 

OBJS += \
./src/flows/QoS/QoSForEXP.o \
./src/flows/QoS/QoSForFLS.o \
./src/flows/QoS/QoSForM_LWDF.o \
./src/flows/QoS/QoSParameters.o 

CPP_DEPS += \
./src/flows/QoS/QoSForEXP.d \
./src/flows/QoS/QoSForFLS.d \
./src/flows/QoS/QoSForM_LWDF.d \
./src/flows/QoS/QoSParameters.d 


# Each subdirectory must supply rules for building sources it contributes
src/flows/QoS/%.o: ../src/flows/QoS/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


