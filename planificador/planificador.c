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

int main(int argc, char **argv) {
	log_planificador = log_create("log_planificador", "PLANIFICADOR",
	false, LOG_LEVEL_INFO);

	colaListos = queue_create();
	entradaSalida = queue_create();
	en_ejecucion = list_create();
	colaFinalizados = queue_create();
	pthread_mutex_init(&mutex_readys, NULL);
	pthread_mutex_init(&mutex_ejecucion, NULL);
	pthread_mutex_init(&mutex_bloqueados, NULL);

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

	int enviar = 1;
	int pid_cpu;

	pthread_t hiloConsola;
	pthread_create(&hiloConsola, NULL, ejecutarIngresoConsola, NULL);

	while (enviar) {
		llamadaPoll:
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

					recv(socketCliente, &pid_cpu, sizeof(int), 0);
					goto llamadaPoll;
				} else {

					codigoOperacion = -33;

					goto seguir;

				}
			}
		}

		seguir: if (queue_size(colaListos) != 0) {
			tipo_pcb * proceso = malloc(sizeof(tipo_pcb));
			proceso = queue_pop(colaListos);
			enviarContextoEjecucion(fileDescriptors[cantfds - 1].fd,
					codigoOperacion, proceso, proceso->dirProceso, algoritmo,
					quantum);
			cambiarAEstadoDeEjecucion(mutex_readys, colaListos, mutex_ejecucion,
					en_ejecucion, entradaSalida, log_planificador,
					fileDescriptors[cantfds - 1].fd, pid_cpu);
		}

		if (list_size(en_ejecucion) != 0) {

			for (fd_index = 0; fd_index < cantfds; fd_index++) {
				if (fileDescriptors[fd_index].revents & POLLIN) {
					if (fileDescriptors[fd_index].fd != socketEscucha) {

						int instruccion;
						recv(fileDescriptors[fd_index].fd, &instruccion,
								sizeof(int), MSG_WAITALL);
						interpretarInstruccion(instruccion,
								fileDescriptors[fd_index].fd, mutex_readys,
								colaListos, log_planificador, entradaSalida,
								en_ejecucion, colaFinalizados, mutex_bloqueados,
								mutex_ejecucion);

					}
				}
			}
		}

	}
	destruirMutex(mutex_readys, mutex_ejecucion, mutex_bloqueados);
	return 0;
}
