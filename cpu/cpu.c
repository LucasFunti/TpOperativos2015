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
#define PUERTO_RECEPTOR "6666"
#define PUERTO_EMISOR "6667"
#define BACKLOG 5
#define PACKAGESIZE 1024
int main(int argc, char **argv) {

	struct addrinfo hintsA;
	struct addrinfo *serverInfoA;

	struct addrinfo hintsB;
	struct addrinfo *serverInfoB;

	memset(&hintsA, 0, sizeof(hintsA));
	hintsA.ai_family = AF_UNSPEC;
	hintsA.ai_flags = AI_PASSIVE;
	hintsA.ai_socktype = SOCK_STREAM;

	memset(&hintsB, 0, sizeof(hintsB));
	hintsB.ai_family = AF_UNSPEC;
	hintsB.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(NULL, PUERTO_RECEPTOR, &hintsA, &serverInfoA) != 0){
		printf("Error en la carga de informacion\n");
		return -1;
	}
	int listenningSocket;
	listenningSocket = socket(serverInfoA->ai_family, serverInfoA->ai_socktype,
			serverInfoA->ai_protocol);
	if (listenningSocket == -1) {
		printf("Error en la creacion de socket escucha\n");
		return -2;
	}
	if (bind(listenningSocket, serverInfoA->ai_addr, serverInfoA->ai_addrlen) == -1){
		printf("Error en el bind\n");
		return -3;
	}
	freeaddrinfo(serverInfoA);

	if (getaddrinfo(IP, PUERTO_EMISOR, &hintsB, &serverInfoB) != 0){
		printf("Error en la carga de informacion\n");
		return -4;
	}

	int serverSocket;
	serverSocket = socket(serverInfoB->ai_family, serverInfoB->ai_socktype,
			serverInfoB->ai_protocol);
	if (serverSocket == -1){
		printf("Error en la creacion del socket servidor\n");
		return -5;
	}
	if (connect(serverSocket, serverInfoB->ai_addr, serverInfoB->ai_addrlen) == -1){
		printf("Error en la conexion\n");
		return -6;
	}
	freeaddrinfo(serverInfoB);

	if (listen(listenningSocket, BACKLOG) == -1){
		printf("Error en la escucha\n");
		return -7;
	}

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,
			&addrlen);
	if (socketCliente == -1){
		printf("Error en aceptar la conexion\n");
		return -8;
	}
	char package[PACKAGESIZE];
	int status = 1;

	while (status != 0) {
		status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
		if (status != 0){
			printf("%s \n ", package);
			printf("Mensaje Recibido\n");
		}
		send(serverSocket, package, PACKAGESIZE + 1, 0);

	}

	close(socketCliente);
	close(listenningSocket);

	return 0;
}
