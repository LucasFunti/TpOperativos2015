################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm

# All of the sources participating in the build are defined here
-include sources.mk
-include so-commons-library/tests/unit-tests/subdir.mk
-include so-commons-library/tests/integration-tests/logger/subdir.mk
-include so-commons-library/src/commons/collections/subdir.mk
-include so-commons-library/src/commons/subdir.mk
-include planificador/subdir.mk
-include cpu/subdir.mk
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
all: 

# Tool invocations
: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/tp-2015-2c-signiorcodigo/so-commons-library" -o  $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) 
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
