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
#include <commons/config.h>

#include "../libreriaSigniorCodigo/libSockets.h"


#define PACKAGESIZE 32
#define BACKLOG 5

int main() {
	char * puertoCpu;
	char * ipSwap;
	char * puertoSwap;
	t_config *memoriaConfig;
	memoriaConfig = config_create("/home/utnso/git/tp-2015-2c-signiorcodigo/memoria/memoriaConfig");
	puertoCpu = config_get_string_value(memoriaConfig,"PUERTO_ESCUCHA");
	ipSwap = config_get_string_value(memoriaConfig,"IP_SWAP");
	puertoSwap = config_get_string_value(memoriaConfig,"PUERTO_SWAP");


	int socketCpu, socketSwap;
	socketCpu = conectarServidor("localhost", puertoCpu, BACKLOG);
	socketSwap = conectarCliente(ipSwap, puertoSwap);

	char package[PACKAGESIZE];
	int status = 1;

	printf("Cliente conectado. Esperando mensajes:\n");

	while (status != 0) {

		status = recv(socketCpu, (void*) package, PACKAGESIZE, 0);
		if (status != 0) {

			printf("Mensaje Recibido\n %s", package);

		}

		send(socketSwap, package, sizeof(package), 0);
	}
	close(socketCpu);
	close(socketSwap);
	return 0;
}

