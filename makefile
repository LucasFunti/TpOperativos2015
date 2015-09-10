# Constantes
CC = gcc
DEP = /home/utnso/tp-2015-2c-signiorcodigo/so-commons-library/src/build/libcommons.so

all: cpu memoria planificador swap
	-mkdir ./bin
	 
cpu:
	$(CC) -L"$(DEP)" /cpu/cpu.c -o bin/cpu
	
memoria:
	$(CC) -L"$(DEP)" /memoria/memoria.c -o bin/memoria
	
planificador:
	$(CC) -L"$(DEP)" /planificador/planificador.c -o bin/planificador
	
swap:
	$(CC) -L"$(DEP)" /swap/swap.c -o bin/swap
