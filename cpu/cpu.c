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
	int cantHilos, i;
	cantHilos = getHilos();
	pthread_t hilos[cantHilos];
	for (i = 0; i > cantHilos; i++) {
		pthread_create(&hilos[i], NULL, iniciarcpu, NULL);
	}

	return 0;
}
