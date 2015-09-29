/*
 * swap.c
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <libSockets.h>
#include <commons/config.h>

#define PUERTO "6668"
#define BACKLOG 5
#define PACKAGESIZE 32

int main() {
	int socketCliente;
	socketCliente = conectarServidor("localhost", PUERTO,BACKLOG);
	char package[PACKAGESIZE];
	int status = 1;

	printf("Cliente conectado. Esperando mensajes:\n");
	while (status != 0) {
		status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
		if (status != 0) {

			printf("Mensaje Recibido\n %s", package);
		}
	}

	close(socketCliente);
	return 0;
}
