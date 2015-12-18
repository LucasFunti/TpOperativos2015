#include "planificador.h"

#define BACKLOG 100
#define PACKAGESIZE 32

int main(int argc, char **argv) {
	remove("/tp-2015-2c-signiorcodigo/planificador/log_planificador");
	log_planificador = log_create("/tp-2015-2c-signiorcodigo/planificador/log_planificador", "PLANIFICADOR",
	false, LOG_LEVEL_INFO);

	colaListos = queue_create();
	entradaSalida = queue_create();
	en_ejecucion = list_create();
	colaFinalizados = queue_create();
	listaCpu = list_create();
	pthread_mutex_init(&mutex_readys, NULL);
	pthread_mutex_init(&mutex_ejecucion, NULL);
	pthread_mutex_init(&mutex_bloqueados, NULL);
	pthread_mutex_init(&mutex_cpus, NULL);
	pthread_mutex_init(&mutex_entrada_salida, NULL);
	sem_init(&procesos_listos, 1, 0);
	sem_init(&cpu_libre, 1, 0);
	sem_init(&input_output, 1, 0);

	char * port = getPuerto();

	int socketEscucha, retornoPoll;
	int fd_index = 0;

	struct pollfd fileDescriptors[100];
	int cantfds = 0;
	socketEscucha = setup_listen("localhost", port);
	listen(socketEscucha, 1024);

	fileDescriptors[0].fd = socketEscucha;
	fileDescriptors[0].events = POLLIN;
	cantfds++;

	int enviar = 1;
	int pid_cpu;
	int socketConActividad;
	int posiblePorcentaje;

	pthread_t hiloConsola, hiloDespachadorListos, hiloIO;
	pthread_create(&hiloConsola, NULL, ejecutarIngresoConsola, NULL);
	pthread_create(&hiloDespachadorListos, NULL, despacharProcesosListos, NULL);
	pthread_create(&hiloIO, NULL, manejarEntradaSalida, NULL);

	while (enviar) {
		llamadaPoll: retornoPoll = poll(fileDescriptors, cantfds, -1);

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

					t_cpu * nuevaCpu = malloc(sizeof(t_cpu));


					list_add(listaCpu, nuevaCpu);

					fileDescriptors[cantfds].fd = socketCliente;
					fileDescriptors[cantfds].events = POLLIN;
					cantfds++;

					recv(socketCliente, &pid_cpu, sizeof(int), 0);

					nuevaCpu->cpu_id = pid_cpu;
					nuevaCpu->socket = socketCliente;
					nuevaCpu->libre = true;

					log_info(log_planificador,
							"Se conecto la cpu con pid: %d, en el socket %d",
							nuevaCpu->cpu_id, socketCliente);

					sem_post(&cpu_libre);

					goto llamadaPoll;
				} else {

					codigoOperacion = -33;
					socketConActividad = fileDescriptors[fd_index].fd;
					goto seguir;

				}
			}
		}

		seguir: recv(socketConActividad, &posiblePorcentaje, sizeof(int),
		MSG_PEEK);

		if (posiblePorcentaje == 43) {

			int pid_cpu, porcentaje;

			//Es para quemar el codOp
			recv(socketConActividad, &pid_cpu, sizeof(int),
			MSG_WAITALL);
			//Es para quemar el tamanio
			recv(socketConActividad, &pid_cpu, sizeof(int),
			MSG_WAITALL);

			recv(socketConActividad, &pid_cpu, sizeof(int),
			MSG_WAITALL);
			recv(socketConActividad, &porcentaje, sizeof(int),
			MSG_WAITALL);

			printf("Cpu %d: %d\n", pid_cpu, porcentaje);

			log_info(log_planificador,"Cpu %d: %d\n", pid_cpu, porcentaje);

			goto llamadaPoll;

		} else {

			int instruccion;

			recv(socketConActividad, &instruccion, sizeof(int),
			MSG_WAITALL);

			interpretarInstruccion(instruccion, socketConActividad);

		}

	}
	destruirMutex(mutex_readys, mutex_ejecucion, mutex_bloqueados);
	return 0;
}
