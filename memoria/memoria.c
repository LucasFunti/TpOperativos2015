/*
 * memoria.c
 *
<<<<<<< HEAD
 *  Created on: 5/9/2015
=======
 *  Created on: 28/9/2015
>>>>>>> e5bed079387090f438690c736e7ce6b16c1bb69c
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
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
		printf("Error en la carga de información");
		return -1;
	}
	int listenningSocket;
	listenningSocket = socket(serverInfoA->ai_family, serverInfoA->ai_socktype,
			serverInfoA->ai_protocol);
	if (listenningSocket == -1){
		printf("Error en la creacion del socket escucha");
		return -2;
	}
	if (bind(listenningSocket, serverInfoA->ai_addr, serverInfoA->ai_addrlen) == -1){
		printf("Error en el bind");
		return -3;
	}
	freeaddrinfo(serverInfoA);

	if (getaddrinfo(IP, PUERTO_EMISOR, &hintsB, &serverInfoB) != 0){
		printf("Error en la carga de informacion");
		return -4;
	}

	int serverSocket;
	serverSocket = socket(serverInfoB->ai_family, serverInfoB->ai_socktype,
			serverInfoB->ai_protocol);
	if (serverSocket == -1){
		printf("Error en la creacion del servidor");
		return -5;
	}
	if (connect(serverSocket, serverInfoB->ai_addr, serverInfoB->ai_addrlen) == -1){
		printf("Error en la conexion con el proceso swap");
		return -6;
	}
	freeaddrinfo(serverInfoB);

	if (listen(listenningSocket, BACKLOG) == -1){
		printf("Error en la escucha");
		return -7;
	}

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,
			&addrlen);
	if (socketCliente == -1){
		printf("Error en aceptar la conexion");
		return -8;
	}

int main() {
	int socketCpu, socketSwap;
	socketCpu = conectarServidor("localhost",PUERTO_RECEPTOR , BACKLOG);
	socketSwap = conectarCliente(IP,PUERTO_EMISOR );

	char package[PACKAGESIZE];
	int status = 1;

	printf("Cliente conectado. Esperando mensajes:\n");

	while (status != 0) {

		status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
		if (status != 0){

		status = recv(socketCpu, (void*) package, PACKAGESIZE, 0);
		if (status != 0) {


			printf("Mensaje Recibido\n %s", package);

		}

		send(serverSocket, package, sizeof(package), 0);
	}

	close(socketCliente);
	close(listenningSocket);

}

		send(socketSwap, package, sizeof(package), 0);
	}
	close(socketCpu);
	close(socketSwap);
	return 0;
}


