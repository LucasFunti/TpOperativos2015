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

int main(int argc, char **argv) {
	int socketCliente;
	socketCliente = conectarServidor("localhost", PUERTO_EMISOR, BACKLOG);
	char message[PACKAGESIZE];
	int enviar = 1;
	while (enviar) {
		fgets(message, PACKAGESIZE, stdin);
		if (!strcmp(message, "exit\n"))
			enviar = 0;
		if (enviar)
			send(socketCliente, message, strlen(message) + 1, 0);

	}
	close(socketCliente);
	return 0;

}
