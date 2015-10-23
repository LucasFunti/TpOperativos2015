/*
 * cpu.c
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <commons/string.h>
#include <limits.h>
#include "../libreriaSigniorCodigo/libreriaCPU.h"
#include "../libreriaSigniorCodigo/libSockets.h"


#define BACKLOG 5
#define PACKAGESIZE 32

int main(int argc, char **argv) {
	//invoco la funcion para leer los datos del file de config y los guardo en la estructura
	t_config_cpu config_cpu;
	config_cpu = read_config_cpu_file();
	pthread_t hilo0;
	pthread_create(&hilo0, NULL, iniciarcpu, NULL);

	free(config_cpu.ipMemoria);
	free(config_cpu.ipPlanificador);
	free(config_cpu.puerto_memoria);
	free(config_cpu.puerto_planificador);
	return 0;
}
