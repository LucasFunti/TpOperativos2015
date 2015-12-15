#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include <pthread.h>
#include <poll.h>
#include <signiorCodigo/libSockets.h>
#include "planificadorFunctions.h"

#define BACKLOG 100
#define PACKAGESIZE 32

int p_last_id = 0;

int main(int argc, char **argv) {
	t_log *log_planificador = log_create("log_planificador", "PLANIFICADOR",
	false, LOG_LEVEL_INFO);

	pthread_mutex_t mutex_readys;
	pthread_mutex_t mutex_ejecucion;
	pthread_mutex_t mutex_bloqueados;
//	inicializarMutex(mutex_readys, mutex_ejecucion, mutex_bloqueados);
	pthread_mutex_init(&mutex_readys, NULL);
	pthread_mutex_init(&mutex_ejecucion, NULL);
	pthread_mutex_init(&mutex_bloqueados, NULL);
	t_queue *colaListos = queue_create();
	t_queue *entrada_salida = queue_create();
	t_list *en_ejecucion = list_create();
	t_queue *colaFinalizados = queue_create();

	char * port = getPuerto();
	char * algoritmo = getAlgoritmo();
	int quantum = getQuantum();

	int socketEscucha, retornoPoll;
	int fd_index = 0;

	struct pollfd fileDescriptors[4];
	int cantfds = 0;
	socketEscucha = setup_listen("localhost", port);
	listen(socketEscucha, 1024);

	fileDescriptors[0].fd = socketEscucha;
	fileDescriptors[0].events = POLLIN;
	cantfds++;
	//printf("SOCKET = %d\n", socketEscucha);

	int pid_a_finalizar;
	int enviar = 1;
	int codigoOperacion;
	int pid_cpu;

	while (enviar) {
		retornoPoll = poll(fileDescriptors, cantfds, -1);
		printf("retorno del poll = %d\n", retornoPoll);
		if (retornoPoll == -1) {
			printf("Error en la funcion poll\n");
		}
		for (fd_index = 0; fd_index < cantfds; fd_index++) {
			if (fileDescriptors[fd_index].revents & POLLIN) {
				if (fileDescriptors[fd_index].fd == socketEscucha) {
					listen(socketEscucha, BACKLOG);
					struct sockaddr_in addr;
					socklen_t addrlen = sizeof(addr);
					int socketCliente = accept(socketEscucha,
							(struct sockaddr *) &addr, &addrlen);
					log_info(log_planificador,
							"Se conecto una cpu en el socket %d",
							socketCliente);

					fileDescriptors[cantfds].fd = socketCliente;
					fileDescriptors[cantfds].events = POLLIN;
					cantfds++;

					recv(socketCliente ,&pid_cpu,sizeof(int),0);
				} else {

					codigoOperacion = -33;

					goto hacerSwitch;

				}
			}
		}

		codigoOperacion = reconocerIdentificador();
		char *nombreProceso = malloc(512);
		hacerSwitch: switch (codigoOperacion) {
		case 1:/* correr */
			scanf("%s", nombreProceso);
			char *path = malloc(256);
			realpath(nombreProceso, path);
//			strcpy(path,"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/Debug/corto");
			printf("el path a enviar es: %s \n", path);
			int pid;
			pid = generarPID(&p_last_id);

			tipo_pcb *proceso = malloc(sizeof(tipo_pcb));
			proceso = generarPCB(pid, path, listo, nombreProceso);

			agregarEnColaDeListos(proceso, mutex_readys, colaListos,
					log_planificador, entrada_salida, en_ejecucion);

			enviarContextoEjecucion(fileDescriptors[cantfds - 1].fd,
					codigoOperacion, proceso, path, algoritmo, quantum);

			cambiarAEstadoDeEjecucion(mutex_readys, colaListos, mutex_ejecucion,
					en_ejecucion, entrada_salida, log_planificador,
					fileDescriptors[cantfds - 1].fd,pid_cpu);

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
			agregarEnColaDeListos(procesoEnEjecucion->proceso, mutex_readys,
					colaListos, log_planificador, entrada_salida, en_ejecucion);
			free(procesoEnEjecucion);

			break;
		case 2:
			/* ps */

			mostrarEstadoDeLista(en_ejecucion, "Ejecutando", log_planificador);
			mostrarEstadoDeListos(colaListos, "Listo", log_planificador);
			mostrarEstadoDeBloqueados(entrada_salida, "Bloqueados",
					log_planificador);

			break;
		case 3:
			/* cpu */
			peticionPorcentajeUsoCpu(en_ejecucion, 3);
			break;
		}

		/*	retornoPoll = poll(fileDescriptors, cantfds, 0);
		 if (retornoPoll == -1) {
		 printf("Error en la funcion poll\n");
		 exit(0);
		 }*/
		for (fd_index = 0; fd_index < cantfds; fd_index++) {
			if (fileDescriptors[fd_index].revents & POLLIN) {
				if (fileDescriptors[fd_index].fd != socketEscucha) {

					int instruccion;
					recv(fileDescriptors[fd_index].fd, &instruccion,
							sizeof(int), MSG_WAITALL);
					interpretarInstruccion(instruccion,
							fileDescriptors[fd_index].fd, mutex_readys,
							colaListos, log_planificador, entrada_salida,
							en_ejecucion, colaFinalizados, mutex_bloqueados,
							mutex_ejecucion);

				}
			}
		}

	}
	destruirMutex(mutex_readys, mutex_ejecucion, mutex_bloqueados);

	return 0;
}
