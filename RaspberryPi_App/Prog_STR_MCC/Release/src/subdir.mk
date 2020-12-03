################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/softPwm.c 

CPP_SRCS += \
../src/MCC_PID.cpp \
../src/Prog_STR_MCC.cpp \
../src/serial.cpp 

OBJS += \
./src/MCC_PID.o \
./src/Prog_STR_MCC.o \
./src/serial.o \
./src/softPwm.o 

C_DEPS += \
./src/softPwm.d 

CPP_DEPS += \
./src/MCC_PID.d \
./src/Prog_STR_MCC.d \
./src/serial.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-unknown-linux-gnueabihf-g++ -I"/home/geoff/eclipse-workspace/Prog_STR_MCC/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-unknown-linux-gnueabihf-gcc -I"/home/geoff/eclipse-workspace/Prog_STR_MCC/include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


