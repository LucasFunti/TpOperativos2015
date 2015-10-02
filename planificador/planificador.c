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
