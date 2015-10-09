/*
 * planificador.c
 *

 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "../libreriaSigniorCodigo/planificadorFunctions.h"
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include "../libreriaSigniorCodigo/libSockets.h"

#define BACKLOG 5
#define PACKAGESIZE 32

int p_last_id = 0;

int main(int argc, char **argv) {
	//genero el arhivo config y guardo los datos del mismo en estructura
	t_config_planificador config_planificador;
	config_planificador = read_config_planificador();

	//generar estructuras necesarias para el planificador (colas)
	t_queue *ready;
	ready = queue_create();
//	t_queue *entrada_salida;
//	entrada_salida = queue_create();
//	t_queue *en_ejecucion;
//	en_ejecucion = queue_create();

	int socketEscucha, maxfd, i, j, socketCliente;
	struct sockaddr_in clienteInfo;
	socklen_t addrlen = sizeof(clienteInfo);
	fd_set read_fds;
	fd_set master;
	socketEscucha = conectarServidor("localhost", config_planificador.puerto,
	BACKLOG);
	listen(socketEscucha, BACKLOG);
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(socketEscucha, &master);
	maxfd = socketEscucha;
	for (;;) {
		read_fds = master;
		if (select(maxfd + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(1);
		}
		for (i = 0; i <= maxfd; i++) {
			if (FD_ISSET(i, &read_fds)) { // ¡¡tenemos datos!!
				if (i == socketEscucha) {
					/* Si ha sido así, conectamos con el cliente */
					if ((socketCliente = accept(socketEscucha,
							(struct sockaddr *) &clienteInfo, &addrlen)) < 0) {
						printf("\n\nError en la conexion 1\n");
						exit();
					} else {
						FD_SET(socketCliente, &master);
						if (socketCliente > maxfd) { // actualizar el máximo
							maxfd = socketCliente;
						}
					}
				} else {
					// gestionar datos de un cliente
					//aca supongo que va a ir todo lo que estoy haciendo abajo

				}
			}
		}

		char proceso[PACKAGESIZE];

		int enviar = 1;
		while (enviar) {
			int codigoOperacion;

			codigoOperacion = reconocerIdentificador();
			printf("el identificador es: %d \n", codigoOperacion);

			switch (codigoOperacion) {
			case 1:/* correr */

				//leo el nombre del proceso
				scanf("%s", proceso);

				//busco el path del proceso y calculo el tamaño del path
				char *path = malloc(128);
				realpath(proceso, path);
				printf("el path es: %s", path);
				size_t tamMessage;
				tamMessage = strlen(path);

				//le genero un id al proceso
				int pid;
				pid = generarPID(&p_last_id);
				printf("el pid del proceso es: %d\n", pid);

				//genero el pcb del proceso
				tipo_pcb currentPCB;
				currentPCB = generarPCB(pid, path, listo);
				printf("El estado del proceso %d es: %d \n", currentPCB.id,
						currentPCB.estado);

				//colocarlo en la cola de readys
				queue_push(ready, &currentPCB);

				if (!strcmp(config_planificador.algoritmo, "FIFO")) {
					//me fijo si tengo algun proceso en la cola de listos
					int colavacia;
					colavacia = queue_is_empty(ready); //si da 1 = vacia, 0 = tiene
					if (colavacia == 1) {
						printf("No hay procesos en la cola de listos");

					} else {
						//ver que haya alguna cpu disponible - select()

						//agarro el primer elemento y lo envio a la cpu
						//genero el paquete, para enviar a la cpu
						Paquete paquete;
						paquete = generarPaquete(codigoOperacion,
								tamMessage + 1, path,
								currentPCB.programCounter);
						char *buffer = serializar(&paquete);
						send(socketCliente, buffer,
								sizeof(int) + sizeof(int) + sizeof(int)
										+ paquete.tamanio, 0);
						free(buffer);
						free(path);
					}

				}

				break;
			case 99:/*finalizar*/
				break;

			}

			/*if (enviar)
			 send(socketCliente, message, sizeof(int), 0);*/

		}
		close(socketCliente);
		free(config_planificador.puerto);
		free(config_planificador.algoritmo);

		return 0;

	}
