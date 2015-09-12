/*
 * cpu.c
 *
 *  Created on: 5/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#define IP "127.0.0.1"
#define PUERTO_PLANIFICADOR "6666"
#define PUERTO_MEMORIA "6667"
#define BACKLOG 5
#define PACKAGESIZE 32

int main(int argc, char **argv) {

	/* Informacion para el servidor Planificador*/
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	/* Informacion para el servidor Memoria */
	struct addrinfo hintsM;
	struct addrinfo *serverInfoM;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	memset(&hintsM, 0, sizeof(hintsM));
	hintsM.ai_family = AF_UNSPEC;
	hintsM.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(IP, PUERTO_PLANIFICADOR, &hints, &serverInfo) != 0) {
		printf("Error en la carga de informacion\n");
		return -4;
	}

	if (getaddrinfo(IP, PUERTO_MEMORIA, &hintsM, &serverInfoM) != 0) {
		printf("Error en la carga de informacion\n");
		return -4;
	}

	int serverPlanificador;
	serverPlanificador = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	if (serverPlanificador == -1) {
		printf("Error en la creacion del socket servidor\n");
		return -5;
	}
	int serverMemoria;
	serverMemoria = socket(serverInfoM->ai_family, serverInfoM->ai_socktype,
			serverInfoM->ai_protocol);
	if (serverMemoria == -1) {
		printf("Error en la creacion del socket servidor\n");
		return -5;
	}

	if (connect(serverPlanificador, serverInfo->ai_addr, serverInfo->ai_addrlen)
			== -1) {
		printf("Error en la conexion\n");
		return -6;
	}
	freeaddrinfo(serverInfo);

	if (connect(serverMemoria, serverInfoM->ai_addr, serverInfoM->ai_addrlen)
			== -1) {
		printf("Error en la conexion\n");
		return -6;
	}
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

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
