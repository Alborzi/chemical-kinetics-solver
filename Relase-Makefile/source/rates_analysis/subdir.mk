################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/rates_analysis/Rates_Analysis.cpp \
../source/rates_analysis/ReportAccuracy.cpp 

OBJS += \
./source/rates_analysis/Rates_Analysis.o \
./source/rates_analysis/ReportAccuracy.o 

CPP_DEPS += \
./source/rates_analysis/Rates_Analysis.d \
./source/rates_analysis/ReportAccuracy.d 


# Each subdirectory must supply rules for building sources it contributes
source/rates_analysis/%.o: ../source/rates_analysis/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=gnu++14 -I"X:\workspace\CKS branches\headers" -O2 -m64 -march=native -Wall -c -fmessage-length=0 -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

