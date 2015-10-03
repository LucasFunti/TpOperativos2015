################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: cpu

dependents:
	-cd /home/utnso/git/tp-2015-2c-signiorcodigo/libreriaSigniorCodigo/Debug && $(MAKE) clean all

# Tool invocations
cpu: $(OBJS) $(USER_OBJS) /home/utnso/git/tp-2015-2c-signiorcodigo/libreriaSigniorCodigo/Debug/liblibreriaSigniorCodigo.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/git/tp-2015-2c-signiorcodigo/libreriaSigniorCodigo/Debug" -L/home/utnso/git/tp-2015-2c-signiorcodigo/so-commons-library/src/commons -o "cpu" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) cpu
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/git/tp-2015-2c-signiorcodigo/libreriaSigniorCodigo/Debug/liblibreriaSigniorCodigo.so:

-include ../makefile.targets
