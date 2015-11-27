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
#include <planificadorFunctions.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include <libSockets.h>
#include <poll.h>

#define BACKLOG 100
#define PACKAGESIZE 32

int p_last_id = 0;

int main(int argc, char **argv) {
	t_log *log_planificador = log_create("log_planificador", "PLANIFICADOR",
	false, LOG_LEVEL_INFO);

	pthread_mutex_t mutex_readys;
	pthread_mutex_t mutex_ejecucion;
	pthread_mutex_t mutex_bloqueados;
	inicializarMutex(mutex_readys, mutex_ejecucion, mutex_bloqueados);
	t_queue *colaListos = queue_create();
	t_queue *entrada_salida = queue_create();
	t_list *en_ejecucion = list_create();
	t_queue *colaFinalizados = queue_create();

	char * port = getPuerto();
	char * algoritmo = getAlgoritmo();
	int quantum = getQuantum();

	int socketCliente, socketEscucha, retornoPoll, fd_index;

	struct pollfd fileDescriptors[4];
	int cantfds = 0;
	socketEscucha = setup_listen("localhost", port);
	listen(socketEscucha, BACKLOG);

	fileDescriptors[0].fd = socketEscucha;
	fileDescriptors[0].events = POLLIN;
	printf("SOCKET = %d\n", socketEscucha);

	int pid_a_finalizar;
	int enviar = 1;

	while (enviar) {
		int codigoOperacion;
		codigoOperacion = reconocerIdentificador();
		char *nombreProceso = malloc(64 * sizeof(char));
		switch (codigoOperacion) {
		case 1:/* correr */
			scanf("%s", nombreProceso);
			char *path = malloc(256);
			realpath(nombreProceso, path);

			int pid;
			pid = generarPID(&p_last_id);

			tipo_pcb *proceso = malloc(sizeof(tipo_pcb));
			proceso = generarPCB(pid, path, listo, nombreProceso);

			agregarEnColaDeListos(proceso, mutex_readys, colaListos,
					log_planificador, entrada_salida, en_ejecucion);

			retornoPoll = poll(fileDescriptors, cantfds, 100000);
			printf("retorno del poll = %d\n", retornoPoll);
			if (retornoPoll == -1) {
				printf("Error en la funcion poll\n");
			}
			if (fileDescriptors[fd_index].revents & POLLIN) {
				if (fileDescriptors[fd_index].fd == socketEscucha) {

					socketCliente = esperarConexionEntrante(socketEscucha,
					BACKLOG, log_planificador);

					fileDescriptors[cantfds].fd = socketCliente;
					fileDescriptors[cantfds].events = POLLIN;
					cantfds++;

					enviarContextoEjecucion(socketCliente, codigoOperacion,
							proceso, path,algoritmo, quantum);

					cambiarAEstadoDeEjecucion(mutex_readys, colaListos,
							mutex_ejecucion, en_ejecucion, entrada_salida,
							log_planificador, fileDescriptors[cantfds].fd);

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
			int PC = -1;
			PC = setProgramCounter(procesoEnEjecucion->proceso->dirProceso);

			procesoEnEjecucion->proceso->programCounter = PC;
			agregarEnColaDeListos(procesoEnEjecucion->proceso, mutex_readys, colaListos,
					log_planificador, entrada_salida, en_ejecucion);
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
			peticionPorcentajeUsoCpu(en_ejecucion,3);
			break;
		}

		retornoPoll = poll(fileDescriptors, cantfds, 0);
		if (retornoPoll == -1) {
			printf("Error en la funcion poll\n");
			exit(0);
		}
		for (fd_index = 0; fd_index < cantfds; fd_index++) {
			if (fileDescriptors[fd_index].revents & POLLIN) {
				if (fileDescriptors[fd_index].fd == socketEscucha) {

					int instruccion;
					recv(fileDescriptors[fd_index].fd, &instruccion,
							sizeof(int), 0);
					interpretarInstruccion(instruccion,
							fileDescriptors[fd_index].fd, mutex_readys,
							colaListos, log_planificador, entrada_salida,
							en_ejecucion, colaFinalizados, mutex_bloqueados,
							mutex_ejecucion);

				}
			}
		}
	}
	close(socketCliente);
	destruirMutex(mutex_readys, mutex_ejecucion, mutex_bloqueados);

	return 0;

}

