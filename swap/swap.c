/*
 * swap.c
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


#define PUERTO "6668"
#define BACKLOG 5
#define PACKAGESIZE 32

<<<<<<< HEAD
typedef struct {
	char *instruccion;
	int cantidadPaginas;
} t_instruccion;

int reconocerInstruccion() {
	return 0;
}

int main(int argc, char **argv) {
=======
int main() {


	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(NULL, PUERTO, &hints, &serverInfo) != 0) {
		printf("Error en la carga de informacion");
		return -1;
	}

	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	if (listenningSocket == -1) {
		printf("Error en la creacion de socket escucha");
		return -2;
	}
	if (bind(listenningSocket, serverInfo->ai_addr, serverInfo->ai_addrlen)
			== -1) {
		printf("Error en el bind");
		return -3;
	}
	freeaddrinfo(serverInfo);

	if (listen(listenningSocket, BACKLOG) == -1) {
		printf("Error en la escucha");
		return -4;
	}

	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr,
			&addrlen);
	if (socketCliente == -1) {
		printf("Error en aceptar la conexion");
		return -5;
	}

>>>>>>> branch 'master' of https://github.com/sisoputnfrba/tp-2015-2c-signiorcodigo
	int socketCliente;
<<<<<<< HEAD
	socketCliente = conectarServidor("localhost", PUERTO, BACKLOG);
=======
	socketCliente = conectarServidor("localhost", PUERTO,BACKLOG);

>>>>>>> branch 'master' of https://github.com/sisoputnfrba/tp-2015-2c-signiorcodigo
	char package[PACKAGESIZE];
	int status = 1;

	/*llenar swap con \0 (funcion) */
	FILE *swap = fopen("swap.data", "w");
	fseek(swap, 512 * 256, SEEK_SET);
	fputc('\0', swap);
	fclose(swap);
	printf("Cliente conectado. Esperando mensajes:\n");

<<<<<<< HEAD
=======



>>>>>>> branch 'master' of https://github.com/sisoputnfrba/tp-2015-2c-signiorcodigo
	while (status != 0) {
		status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
		char*instruccion = malloc(10);
		int paginas;
		scanf("%s %d",instruccion,paginas);
		if (status != 0) {
<<<<<<< HEAD
			int instruccion;
			instruccion = reconocerInstruccion();
			switch (instruccion) {
			case 4:/*iniciar*/
				/* corroborar espacio disponible en el archivo-(funcion)*/
				break;
			case 5:/*leer*/
				/*Buscar proceso en el archivo (funcion) */
				/* Si existe - devolver contenido de la pagina n solicitada (funcion) */
				break;
			case 6:/*escribir*/
				break;
			case 7:/*entrada salida*/
				break;
			case 8:/*finalizar*/
				/*Buscar proceso en el archivo (funcion) */
				/* si existe liberarlo de memoria */
				break;
			}
			printf("Mensaje Recibido\n %s", package);
=======


				printf("Mensaje Recibido\n %s", package);
			}
>>>>>>> branch 'master' of https://github.com/sisoputnfrba/tp-2015-2c-signiorcodigo
		}



	close(socketCliente);
	close(listenningSocket);

	return 0;
}

