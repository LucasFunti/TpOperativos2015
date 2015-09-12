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
#define PUERTO_EMISOR "6666"
#define BACKLOG 5
#define PACKAGESIZE 32

int main() {

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(NULL, PUERTO_EMISOR, &hints, &serverInfo) != 0) {
		printf("Error en la carga de informacion del servidor\n");
		return -1;
	}

	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	if (listenningSocket == -1) {
		printf("Error en la creacion del socket escucha");
		return -2;
	}

	if (bind(listenningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen)
			== -1) {
		printf("Error en el bind");
		return -3;
	}
	freeaddrinfo(serverInfo);

	listen(listenningSocket, BACKLOG);

	struct sockaddr_in addr;// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,
			&addrlen);
	int enviar = 1;
	char message[PACKAGESIZE];

	printf(
			"Conectado al servidor. Bienvenido al sistema, ya puede enviar mensajes. Escriba 'exit' para salir\n");

	while (enviar) {
		fgets(message, PACKAGESIZE, stdin);
		if (!strcmp(message, "exit\n"))
			enviar = 0;
		if (enviar)
			send(socketCliente, message, strlen(message) + 1, 0);
	}

	close(socketCliente);
	close(listenningSocket);

	return 0;

}
