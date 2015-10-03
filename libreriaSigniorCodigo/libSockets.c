/*
 * libSockets.c
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "libSockets.h"

struct addrinfo* cargarInfoSocket(char *IP, char* Port) {
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	int error;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (!strcmp(IP, "localhost")) {
		hints.ai_flags = AI_PASSIVE;
		error = getaddrinfo(NULL, Port, &hints, &serverInfo);
	} else
		error = getaddrinfo(IP, Port, &hints, &serverInfo);
	if (error != 0) {
		printf("Problema con el getaddrinfo()\n");
		return NULL;
	}
	return serverInfo;
}

int conectarCliente(char *IP, char* Port) {
	struct addrinfo* serverInfo = cargarInfoSocket(IP, Port);
	if (serverInfo == NULL) {
		return -1;
	}
	int serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	if (serverSocket == -1) {
		printf("Error en la creacion del socket\n");
		return -1;
	}
	if (connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen)
			== -1) {
		printf("No se pudo conectar con el socket servidor\n");
		close(serverSocket);
		return -1;
	}
	freeaddrinfo(serverInfo);
	return serverSocket;
}

/* Funcion que serializa una estructura paquete */
char *serializar(Paquete *unPaquete) {
	void *buffer = malloc(
			sizeof(int)/*ID*/+ sizeof(int)/*ProgCounter*/+ sizeof(int)/*Tamañopath*/
			+ sizeof(char) * unPaquete->tamanio);
	memcpy(buffer, &unPaquete->codigoOperacion, sizeof(int));
	memcpy(buffer + sizeof(int), &unPaquete->programCounter, sizeof(int));
	memcpy(buffer + sizeof(int) + sizeof(int), &unPaquete->tamanio,
			sizeof(int));
	memcpy(buffer + sizeof(int) + sizeof(int) + sizeof(int), unPaquete->mensaje,
			unPaquete->tamanio);
	return buffer;
}
/*deserializar la estructura paquete - devuelve la direccion a la estructura Paquete */
Paquete *deserializar(char *buffer) {
	Paquete *unPaquete = malloc(sizeof(Paquete));
	memcpy(&unPaquete->codigoOperacion, buffer, sizeof(int));
	memcpy(&unPaquete->programCounter, buffer + sizeof(int), sizeof(int));
	memcpy(&unPaquete->tamanio, buffer + sizeof(int) + sizeof(int),
			sizeof(int));
	memcpy(&unPaquete->mensaje, buffer + (sizeof(int) * 3), unPaquete->tamanio);
	return &unPaquete;
}
/* Funcion que genera un paquete. agarra los valores correspondientes y
 * los coloca dentro de la estructura Paquete */

Paquete generarPaquete(int codigoOperacion, int tamMessage, char *message,
		int programCounter) {
	Paquete paquete;

	paquete.codigoOperacion = codigoOperacion;
	paquete.programCounter = programCounter;
	paquete.tamanio = tamMessage;
	paquete.mensaje = malloc(tamMessage);
	strcpy(paquete.mensaje, message);

	return paquete;
}

/* funcion que representa el uso de la funcion select() para leer
 * de diferentes problemas */

int conectarServidor(char* IP, char* Port, int backlog) {
//	fd_set master;
//	fd_set read_fds;
//	int fdmax;
//	int i;
//	int addrlen;
//	int newfd;
//	FD_ZERO(&master);
//	FD_ZERO(&read_fds);
	struct addrinfo* serverInfo = cargarInfoSocket(IP, Port);
	if (serverInfo == NULL)
		return -1;
	int socketEscucha;
	socketEscucha = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	if (bind(socketEscucha, serverInfo->ai_addr, serverInfo->ai_addrlen)
			== -1) {
		printf("Error en el Bind \n");
	}
	freeaddrinfo(serverInfo);
	if (listen(socketEscucha, backlog) == -1) {
		printf("error en la escucha de un cliente");
		return -5;
	}

//	FD_SET(socketEscucha, &master);
//	fdmax = socketEscucha;
//	for (;;) {
//		read_fds = master;
//		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
//			perror("Error en el Select");
//			exit(1);
//		}
//		for (i = 0; i <= fdmax; i++) {
//			if (FD_ISSET(i, &read_fds)) {
//				if (i == socketEscucha) {
//					addrlen = sizeof(serverInfo);
//					if ((newfd = accept(socketEscucha,
//							(struct sockaddr *) &serverInfo, &addrlen)) == -1) {
//						perror("accept");
//					} else {
//						FD_SET(newfd, &master);
//						if (newfd > fdmax) {
//							fdmax = newfd;
//						}
//
//					}
//				}else {
//					//gestionar datos cliente
//					//recivir datos de un cliente
//				}
//			}
//		}
//	}
//
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(socketEscucha, (struct sockaddr *) &addr,
			&addrlen);
	if (socketCliente == -1) {
		printf("Error en la conexion, en la funcion accept\n");
		return -2;
	}
	return socketCliente;
}

/*  */

