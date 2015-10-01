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
#include <commons/txt.h>

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
