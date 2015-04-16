################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/device/IPClassifier/Classifier.cpp \
../src/device/IPClassifier/ClassifierParameters.cpp 

OBJS += \
./src/device/IPClassifier/Classifier.o \
./src/device/IPClassifier/ClassifierParameters.o 

CPP_DEPS += \
./src/device/IPClassifier/Classifier.d \
./src/device/IPClassifier/ClassifierParameters.d 


# Each subdirectory must supply rules for building sources it contributes
src/device/IPClassifier/%.o: ../src/device/IPClassifier/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


