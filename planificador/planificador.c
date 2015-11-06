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
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include "../libreriaSigniorCodigo/libSockets.h"
#include <poll.h>

#define BACKLOG 100
#define PACKAGESIZE 32

int p_last_id = 0;

int main(int argc, char **argv) {

	//genero el archivo log
	t_log *log_planificador = log_create("log_planificador", "PLANIFICADOR",
	true, LOG_LEVEL_INFO);

	pthread_mutex_t mutex_readys;
	pthread_mutex_t mutex_ejecucion;
	pthread_mutex_t mutex_bloqueados;
	inicializarMutex(mutex_readys, mutex_ejecucion, mutex_bloqueados);
	//generar estructuras necesarias para el planificador (colas)
	t_queue *colaListos = queue_create();
	t_queue *entrada_salida = queue_create();
	t_list *en_ejecucion = list_create();
//	t_queue *colaFinalizados=queue_create();

	char * port = getPuerto();
//	char *algoritmo = getAlgoritmo();

	int  socketCliente, socketEscucha, retornoPoll, fd_index;
	struct sockaddr_in client_address;

	struct pollfd fileDescriptors[4];
	int cantfds = 0;
	socketEscucha = setup_listen("localhost",port);

	fileDescriptors[0].fd = socketEscucha;
	fileDescriptors[0].events = POLLIN;
	printf("SOCKET = %d\n", fileDescriptors[0].fd);

	int pid_a_finalizar;
	int enviar = 1;

	while (enviar) {

		int codigoOperacion;

		codigoOperacion = reconocerIdentificador();
		char *nombreProceso = malloc(64 * sizeof(char));
		switch (codigoOperacion) {
		case 1:/* correr */
			//leo el nombre del proceso
			scanf("%s", nombreProceso);
			//busco el path del proceso y calculo el tamaño del path
			char *path = malloc(128);
			realpath(nombreProceso, path);

			//le genero un id al proceso
			int pid;
			pid = generarPID(&p_last_id);

			//genero el pcb del proceso
			tipo_pcb *proceso = malloc(sizeof(tipo_pcb));
			proceso = generarPCB(pid, path, listo, nombreProceso);
			agregarEnColaDeListos(proceso, mutex_readys, colaListos,
					log_planificador, entrada_salida, en_ejecucion);

//			cambiarEstadoABloqueado(proceso, mutex_bloqueados, entrada_salida,
//					mutex_ejecucion, en_ejecucion, mutex_readys, colaListos,
//					log_planificador);

			retornoPoll = poll(fileDescriptors, cantfds, 100000);
			printf("retorno del poll = %d\n", retornoPoll);
			if (retornoPoll == -1) {
				printf("Error en la funcion poll\n");
			}
			for (fd_index = 0; fd_index < cantfds; fd_index++) {
				if (fileDescriptors[fd_index].revents & POLLIN) {
					if (fileDescriptors[fd_index].fd == socketEscucha) {

						socketCliente = esperarConexionEntrante(socketEscucha,BACKLOG,log_planificador);

						fileDescriptors[cantfds].fd = socketCliente;
						fileDescriptors[cantfds].events = POLLIN;
						cantfds++;
						Paquete *paquete = malloc(sizeof(Paquete));
						paquete = generarPaquete(codigoOperacion,
								strlen(proceso->dirProceso) + 1, path,
								proceso->programCounter);
						printf(
								"El paquete a enviar contiene:\nCod= %d -- mensaje= %s\n",
								paquete->codigoOperacion, paquete->path);
						void *buffer = serializar(paquete);
						send(socketCliente, buffer,
								sizeof(int) + sizeof(int) + sizeof(int)
										+ paquete->tamanio, 0);
						cambiarAEstadoDeEjecucion(mutex_readys, colaListos,
								mutex_ejecucion, en_ejecucion, entrada_salida,
								log_planificador, socketCliente);
						free(buffer);
						free(path);

					}
				}
			}

			break;
		case 99:
			/*finalizar*/

			scanf("%d", &pid_a_finalizar);
			bool encontrar_pid(void * nodo) {
				return ((((nodo_en_ejecucion*) nodo)->proceso->id)
						== pid_a_finalizar);
			}
			nodo_en_ejecucion *procesoEnEjecucion = malloc(
					sizeof(nodo_en_ejecucion));
			procesoEnEjecucion = list_remove_by_condition(en_ejecucion,
					encontrar_pid);
			printf("VAMOS A FINALIZAR AL PROCESO CON PID: %d\n",
					procesoEnEjecucion->proceso->id);
//			int PC = -1;
//			PC = setProgramCounter(procesoEnEjecucion->proceso->dirProceso);
			//enviar el proceso a la cpu con el programCounter obtenido

			free(procesoEnEjecucion);

			break;
		case 2:
			/* ps */

			mostrarEstadoDeLista(en_ejecucion, "Ejecutando");
			mostrarEstadoDeListos(colaListos, "Listo");
			mostrarEstadoDeBloqueados(entrada_salida, "Bloqueados");

			break;
		case 3:
			/* cpu */
			break;
		}

		retornoPoll = poll(fileDescriptors, cantfds, 0);
		if (retornoPoll == -1) {
			printf("Error en la funcion poll\n");
			exit(0);
		}

		/*if (enviar)
		 send(socketCliente, message, sizeof(int), 0);*/

	}
//	close(socketCliente);
	destruirMutex(mutex_readys, mutex_ejecucion, mutex_bloqueados);

	return 0;

}

