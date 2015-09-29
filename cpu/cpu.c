/*
 * cpu.c
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <libSockets.h>
#include <commons/config.h>

#define IP "127.0.0.1"
#define PUERTO_PLANIFICADOR "6666"
#define PUERTO_MEMORIA "6667"
#define BACKLOG 5
#define PACKAGESIZE 32

int main(int argc, char **argv) {
	int serverPlanificador,serverMemoria;
	serverPlanificador = conectarCliente(IP,PUERTO_PLANIFICADOR);
	serverMemoria = conectarCliente(IP,PUERTO_MEMORIA);
	char package[PACKAGESIZE];
	int status = 1;

	while (status != 0) {
		status = recv(serverPlanificador, (void*) package, PACKAGESIZE, 0);
		if (status != 0) {

			printf("Mensaje Recibido\n %s", package);

		}
		send(serverMemoria, package, sizeof(package), 0);

	}
	close(serverPlanificador);
	close(serverMemoria);
	return 0;
}
