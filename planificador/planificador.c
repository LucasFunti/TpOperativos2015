/*
 * planificador.c
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
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <libSockets.h>
#include <commons/config.h>
#include <commons/txt.h>


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


int reconocerIdentificador() {
	int codigoOperacion;
	char *identificador = malloc(sizeof(char) * 10);
	scanf("%s", identificador);
	if (!strcmp(identificador, "correr")) {
		codigoOperacion = 1;
	} else if (!strcmp(identificador, "finalizar")) {
		codigoOperacion = 99;
	} else if (!strcmp(identificador, "ps")) {
		codigoOperacion = 2;
	} else
		codigoOperacion = 3;
	if (!strcmp(identificador, "exit\n")) {
		return -1;
	}
	return codigoOperacion;
}

typedef struct {
	int id;
	char *dirProceso;
	int estado;
} pcb;

typedef struct {
	int codigoOperacion;
	int tamanio;
	char *mensaje;
} Paquete;

char *serializar(Paquete *unPaquete) {
	void *buffer = malloc(
			sizeof(int) + sizeof(int) + sizeof(char) * unPaquete->tamanio);
	memcpy(buffer, &unPaquete->codigoOperacion, sizeof(int));
	memcpy(buffer + sizeof(int), &unPaquete->tamanio, sizeof(int));
	memcpy(buffer + sizeof(int) + sizeof(int), unPaquete->mensaje,
			unPaquete->tamanio);
	return buffer;
}

Paquete generarPaquete(int codigoOperacion, int tamMessage, char *message) {
	Paquete paquete;

	paquete.codigoOperacion = codigoOperacion;
	paquete.tamanio = tamMessage;
	paquete.mensaje = malloc(tamMessage);
	strcpy(paquete.mensaje, message);

	return paquete;
}

int main(int argc, char **argv) {
	int socketCliente;
	socketCliente = conectarServidor("localhost", PUERTO_EMISOR, BACKLOG);
	char message[PACKAGESIZE];

	int enviar = 1;
	while (enviar) {
		int codigoOperacion;

		codigoOperacion = reconocerIdentificador();
		printf("el identificador es: %d \n", codigoOperacion);

		switch (codigoOperacion) {
		case 1:
			scanf("%s", message);
			size_t tamMessage;
			char *path = malloc(80);
			realpath(message, path);
			printf("el path es: %s", path);
			tamMessage = strlen(path);
			Paquete paquete;
			paquete = generarPaquete(codigoOperacion, tamMessage, path);
			char *buffer = serializar(&paquete);

			send(socketCliente, buffer,
					sizeof(int) + sizeof(int) + paquete.tamanio, 0);
			free(buffer);
			break;
		case 99:
			break;

		}

		/*if (enviar)
		 send(socketCliente, message, sizeof(int), 0);*/

	}
	close(socketCliente);

	return 0;

}
