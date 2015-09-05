/*
 * planificador.c
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
#define PUERTO "6667"
#define PACKAGESIZE 1024

int main(int argc, char **argv) {
	int puntero_socket;

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(IP, PUERTO, &hints, &serverInfo);// Carga en serverInfo los datos de la conexion

	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);

	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);	// No lo necesitamos mas

	char* message = malloc(sizeof(char) * PACKAGESIZE);

	strcpy(message, argv[1]);
	strcat(message, " ");
	strcat(message, argv[2]);
	send(serverSocket, message, strlen(message) + 1, 0);
	free(message);
	close(serverSocket);
	return 0;

	/* ADIO'! */
}

