################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libSockets.c \
../libreriaCPU.c \
../planificadorFunctions.c \
../swapFunctions.c 

OBJS += \
./libSockets.o \
./libreriaCPU.o \
./planificadorFunctions.o \
./swapFunctions.o 

C_DEPS += \
./libSockets.d \
./libreriaCPU.d \
./planificadorFunctions.d \
./swapFunctions.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


