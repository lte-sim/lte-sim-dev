################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/protocolStack/mac/AMCModule.cpp \
../src/protocolStack/mac/harq-manager.cpp \
../src/protocolStack/mac/henb-mac-entity.cpp 

CC_SRCS += \
../src/protocolStack/mac/enb-mac-entity.cc \
../src/protocolStack/mac/mac-entity.cc \
../src/protocolStack/mac/ue-mac-entity.cc 

OBJS += \
./src/protocolStack/mac/AMCModule.o \
./src/protocolStack/mac/enb-mac-entity.o \
./src/protocolStack/mac/harq-manager.o \
./src/protocolStack/mac/henb-mac-entity.o \
./src/protocolStack/mac/mac-entity.o \
./src/protocolStack/mac/ue-mac-entity.o 

CC_DEPS += \
./src/protocolStack/mac/enb-mac-entity.d \
./src/protocolStack/mac/mac-entity.d \
./src/protocolStack/mac/ue-mac-entity.d 

CPP_DEPS += \
./src/protocolStack/mac/AMCModule.d \
./src/protocolStack/mac/harq-manager.d \
./src/protocolStack/mac/henb-mac-entity.d 


# Each subdirectory must supply rules for building sources it contributes
src/protocolStack/mac/%.o: ../src/protocolStack/mac/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/protocolStack/mac/%.o: ../src/protocolStack/mac/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


