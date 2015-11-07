/*
 * cpu.c
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <commons/string.h>
#include <commons/log.h>
#include <limits.h>
#include "../libreriaSigniorCodigo/libreriaCPU.h"
#include "../libreriaSigniorCodigo/libSockets.h"
#include "../libreriaSigniorCodigo/planificadorFunctions.h"

#define BACKLOG 5
#define PACKAGESIZE 32



int main(int argc, char **argv){
	// Inicio el archivo log
	t_hilo infoHilo;
	remove("log_cpu");
	t_log *log_cpu = log_create("log_cpu", "CPU", true, LOG_LEVEL_INFO);

/*	int i;
	int cantHilos = getHilos();
	pthread_t hilos[cantHilos];
	for (i = 0; i < cantHilos; i++) {
		infoHilo.idHilo = &hilos[i];
		pthread_create(&hilos[i], NULL, iniciarcpu, (void *) &infoHilo);
		};
	for (i = 0; i < cantHilos; i++) {
		pthread_join(hilos[i], NULL);
	} */

	printf("ingrese la accion a testear, o escriba 'ayuda' para ver comandos disponibles:\n");
	char *accion = malloc(sizeof(char) * 32);
	scanf("%[^\n]%*c", accion);
	while(strcmp(accion,"exit")!=0){
		testCpuFunction(accion);
		printf("ingrese la accion a testear, o escriba 'ayuda' para ver comandos disponibles:\n");
		scanf("%[^\n]%*c", accion);

	};





	return 0;
}
