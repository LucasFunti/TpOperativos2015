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
#include <libSockets.h>
#include <commons/config.h>
#include <commons/txt.h>


#define IP "127.0.0.1"
#define PUERTO_EMISOR "6666"
#define BACKLOG 5
#define PACKAGESIZE 32

int p_last_id = 0;

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
			int pid ;

			pid = generarPID(&p_last_id);
			printf("el pid del proceso es: %d\n", pid);

			tipo_pcb currentPCB;
			currentPCB = generarPCB(pid, path, listo);
			printf("El estado del proceso %d es: %d \n", currentPCB.id,
					currentPCB.estado);

			send(socketCliente, buffer,
					sizeof(int) + sizeof(int) + paquete.tamanio, 0);

			free(buffer);
			free(path);
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
