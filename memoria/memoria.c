/*
 * memoria.c
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

#define IP "127.0.0.1"
#define PUERTO_RECEPTOR "6667"
#define PUERTO_EMISOR "6668"
#define PACKAGESIZE 32
#define BACKLOG 5

int main() {
	int socketCpu, socketSwap;
	socketCpu = conectarServidor("localhost",PUERTO_RECEPTOR , BACKLOG);
	socketSwap = conectarCliente(IP,PUERTO_EMISOR );
	char package[PACKAGESIZE];
	int status = 1;

	printf("Cliente conectado. Esperando mensajes:\n");

	while (status != 0) {
		status = recv(socketCpu, (void*) package, PACKAGESIZE, 0);
		if (status != 0) {

			printf("Mensaje Recibido\n %s", package);

		}
		send(socketSwap, package, sizeof(package), 0);
	}
	close(socketCpu);
	close(socketSwap);
	return 0;
}

