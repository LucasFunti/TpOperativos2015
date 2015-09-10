/*
 * swap.c
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


#define PUERTO "6668"
#define BACKLOG 5
#define PACKAGESIZE 1024

int main(){

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(NULL, PUERTO, &hints, &serverInfo) != 0){
		printf("Error en la carga de informacion");
		return -1;
	}

	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if (listenningSocket == -1){
		printf("Error en la creacion de socket escucha");
		return -2;
	}
	if (bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen) == -1){
		printf("Error en el bind");
		return -3;
	}
	freeaddrinfo(serverInfo);

	if (listen(listenningSocket, BACKLOG) == -1) {
		printf("Error en la escucha");
		return -4;
	}


	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);
	if (socketCliente == -1){
		printf("Error en aceptar la conexion");
		return -5;
	}
	char package[PACKAGESIZE];
	int status = 1;

	printf("Cliente conectado. Esperando mensajes:\n");

	while (status != 0){
		status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
		if (status != 0) {
			printf("%s \n ", package);
			printf("Mensaje Recibido\n");
		}

	}

	close(socketCliente);
	close(listenningSocket);

	return 0;
}

