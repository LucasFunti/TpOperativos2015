/*
 * planificador.c
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <libSockets.h>
#include <commons/config.h>

#define IP "127.0.0.1"
#define PUERTO_EMISOR "6666"
#define BACKLOG 5
#define PACKAGESIZE 32

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

int main(int argc, char **argv) {
	int socketCliente;
	socketCliente = conectarServidor("localhost", PUERTO_EMISOR, BACKLOG);
	char message[PACKAGESIZE];

	int enviar = 1;
	while (enviar) {
		int codigoOperacion;

		codigoOperacion = reconocerIdentificador();
		printf("el identificador es: %d", codigoOperacion);
		switch (codigoOperacion) {
		case 1:
			scanf("%s", message);
			size_t tamMessage;
			tamMessage = strlen(message);
			void *buffer = malloc(
					sizeof(int) + sizeof(int) + sizeof(char) * tamMessage);
			memcpy(buffer, &codigoOperacion, sizeof(int));
			memcpy(buffer + sizeof(int), &tamMessage, sizeof(int));
			memcpy(buffer + sizeof(int) + sizeof(int), message, tamMessage);
			send(socketCliente, buffer,
					sizeof(int) + sizeof(int) + sizeof(char) * tamMessage, 0);
			break;

		}

		/*if (enviar)
		 send(socketCliente, message, sizeof(int), 0);*/

	}
	close(socketCliente);
	return 0;

}
