/*
 * planificadorFunctions.c
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#include "planificadorFunctions.h"

void inicializarMutex(pthread_mutex_t mutex_readys,
		pthread_mutex_t mutex_ejecucion, pthread_mutex_t mutex_bloqueados) {
	pthread_mutex_init(&mutex_readys, NULL);
	pthread_mutex_init(&mutex_ejecucion, NULL);
	pthread_mutex_init(&mutex_bloqueados, NULL);
}
void destruirMutex(pthread_mutex_t mutex_readys,
		pthread_mutex_t mutex_ejecucion, pthread_mutex_t mutex_bloqueados) {
	pthread_mutex_destroy(&mutex_readys);
	pthread_mutex_destroy(&mutex_ejecucion);
	pthread_mutex_destroy(&mutex_bloqueados);
}
/* reconoce el identificador escrito por linea de comando */
int reconocerIdentificador() {
	int codigoOperacion;
	char *identificador = malloc(sizeof(char) * 10);
	scanf("%s", identificador);
	if (!strcmp(identificador, "correr")) {
		codigoOperacion = 1;
	} else if (!strcmp(identificador, "finalizar")) {
		codigoOperacion = 99;
	} else if (!strcmp(identificador, "ps")) {
		codigoOperacion = 2;
	} else if (!strcmp(identificador, "cpu")) {
		codigoOperacion = 3;
	} else if (!strcmp(identificador, "exit\n")) {
		return -1;
	}
	return codigoOperacion;
}

/* genera id del pcb */
int generarPID(int* pid) {
	*pid = *pid + 1;
	return *pid;
}

/* genera pcb */
tipo_pcb *generarPCB(int pid, char *path, int estado, char *nombre) {
	tipo_pcb *pcb = malloc(sizeof(tipo_pcb));
	pcb->id = pid;
	pcb->dirProceso = malloc(strlen(path) + 1);
	pcb->dirProceso = path;
	pcb->estado = estado;
	pcb->programCounter = 0;
	pcb->nombrePrograma = malloc(strlen(nombre) + 1);
	pcb->nombrePrograma = nombre;
	pcb->tiempoComienzo = time(NULL);

	return pcb;
}
/* Leer el archivo de config y guardarla en una estructura */
char *getPuerto() {
	t_config *planificador_config;
	planificador_config = config_create(
			"/tp-2015-2c-signiorcodigo/planificador/planificadorConfig");
	char * puerto = config_get_string_value(planificador_config,
			"PUERTO_ESCUCHA");
	return puerto;
}

char *getAlgoritmo() {
	t_config *planificador_config;
	planificador_config = config_create(
			"/tp-2015-2c-signiorcodigo/planificador/planificadorConfig");
	char* algoritmo = config_get_string_value(planificador_config,
			"ALGORITMO_PLANIFICACION");
	return algoritmo;
}

int getQuantum() {
	t_config *planificador_config;
	planificador_config = config_create(
			"/tp-2015-2c-signiorcodigo/planificador/planificadorConfig");
	int quantum = config_get_int_value(planificador_config, "PUERTO_ESCUCHA");
	return quantum;
}

/* inicializar las estructuras para el planificador */
void inicializarColecciones(t_queue * colaListos, t_queue * colaFinalizados,
		t_list * listaEjecutando, t_queue * entradaSalida) {
	colaListos = queue_create();
	colaFinalizados = queue_create();
	listaEjecutando = list_create();
	entradaSalida = queue_create();
}

/* recorro la lissta mostrando los estado de los procesos para la instruccion ps */
void mostrarEstadoDeLista(t_list *lista, char*estado, t_log * log_planificador) {
	int j;
	if (list_size(lista) == 0) {
		printf("La lista de planificacion ,%s, esta vacia\n", estado);

	} else {

		for (j = 0; j < list_size(lista); j++) {
			nodo_en_ejecucion *procesoEnEjecucion = list_get(lista, j);
			printf("mProc %d: %s -> %s \n", procesoEnEjecucion->proceso->id,
					procesoEnEjecucion->proceso->nombrePrograma, estado);
			log_info(log_planificador, "mProc %d: %s -> %s \n",
					procesoEnEjecucion->proceso->id,
					procesoEnEjecucion->proceso->nombrePrograma, estado);
		}

	}
}

void mostrarEstadoDeListos(t_queue *cola, char*estado, t_log * log_planificador) {
	int j;
	if (queue_size(cola) == 0) {
		printf("La cola de planificacion ,%s, esta vacia\n", estado);
	} else {
		for (j = 0; j < queue_size(cola); j++) {
			tipo_pcb *proceso = list_get(cola->elements, j);
			printf("mProc %d: %s -> %s \n", proceso->id,
					proceso->nombrePrograma, estado);
			log_info(log_planificador, "mProc %d: %s -> %s \n", proceso->id,
					proceso->nombrePrograma, estado);
		}
	}
}
void mostrarEstadoDeBloqueados(t_queue *cola, char*estado,
		t_log *log_planificador) {
	int j;
	if (queue_size(cola) == 0) {
		printf("La cola de planificacion ,%s, esta vacia\n", estado);
	} else {
		for (j = 0; j < queue_size(cola); j++) {
			nodo_en_ejecucion *proceso = list_get(cola->elements, j);
			printf("mProc %d: %s -> %s \n", proceso->proceso->id,
					proceso->proceso->nombrePrograma, estado);
			log_info(log_planificador, "mProc %d: %s -> %s \n",
					proceso->proceso->id, proceso->proceso->nombrePrograma,
					estado);
		}
	}
}

/* cambiar estado del proceso */
void cambiarEstado(tipo_pcb *proceso, int estado) {
	proceso->estado = estado;
	mostrarEstadoDeListos(colaListos, "Listos", log_planificador);
	mostrarEstadoDeLista(en_ejecucion, "Ejecucion", log_planificador);
	mostrarEstadoDeBloqueados(entradaSalida, "Bolqueados", log_planificador);
}

/*agrega un proceso a la cola de listos */
void agregarEnColaDeListos(tipo_pcb *proceso) {
	pthread_mutex_lock(&mutex_readys);
	queue_push(colaListos, proceso);
	log_info(log_planificador,
			"Colocado el proceso: %s con pid = %d en la cola de listos",
			proceso->nombrePrograma, proceso->id);
	proceso->tiempoIngreso = time(NULL);
	cambiarEstado(proceso, listo);
	pthread_mutex_unlock(&mutex_readys);

}

tipo_pcb * removerDeColaDeListos() {

	tipo_pcb *proceso = malloc(sizeof(tipo_pcb));
	pthread_mutex_lock(&mutex_readys);
	proceso = queue_pop(colaListos);
	time_t horaActual = time(NULL);
	proceso->tiempoEnReadys = difftime(horaActual, proceso->tiempoIngreso);
	pthread_mutex_unlock(&mutex_readys);
	return proceso;

}

void agregarAListaDeEjecucion(nodo_en_ejecucion *proceso) {

	pthread_mutex_lock(&mutex_ejecucion);

	list_add(en_ejecucion, proceso);

	log_info(log_planificador,
			"Colocado el proceso: %s con pid = %d en la cola de ejecucion",
			proceso->proceso->nombrePrograma, proceso->proceso->id);

	cambiarEstado(proceso->proceso, ejecucion);

	pthread_mutex_unlock(&mutex_ejecucion);

}
/* saca un proceso de la cola de listos y lo coloca en la lista de ejecucion*/
void cambiarAEstadoDeEjecucion(int socketEnEjecucion, int pid_cpu) {

	tipo_pcb * proceso = removerDeColaDeListos(mutex_readys, colaListos);

	nodo_en_ejecucion * procesoEjecutando = malloc(sizeof(nodo_en_ejecucion));

	procesoEjecutando->proceso = proceso;
	procesoEjecutando->socket = socketEnEjecucion;
	procesoEjecutando->pid_cpu = pid_cpu;
	procesoEjecutando->instrucciones_ejecutadas = 0;
	agregarAListaDeEjecucion(procesoEjecutando);

}

nodo_en_ejecucion * removerDeListaDeEjecucion(tipo_pcb *pcb) {

	bool encontrar_pid(void * nodo) {
		return ((((nodo_en_ejecucion*) nodo)->proceso->id) == pcb->id);
	}

	pthread_mutex_lock(&mutex_ejecucion);

	nodo_en_ejecucion * proceso = list_remove_by_condition(en_ejecucion,
			encontrar_pid);

	pthread_mutex_unlock(&mutex_ejecucion);

	return proceso;

}
void agregarAColaDeBloqueados(nodo_entrada_salida*io) {
	pthread_mutex_lock(&mutex_bloqueados);
	queue_push(entradaSalida, io);
	log_info(log_planificador,
			"Colocado el proceso: %s con pid = %d en la cola de bloqueados",
			io->proceso->nombrePrograma, io->proceso->id);
	cambiarEstado(io->proceso, bloqueado);
	pthread_mutex_unlock(&mutex_bloqueados);

}
/* saca un proceso de la lista de ejecucion y lo coloca en la cola de entrada salida */
void * cambiarEstadoABloqueado(void* data) {
	data_hilo *dataHilo = data;

	nodo_en_ejecucion * proceso = removerDeListaDeEjecucion(dataHilo->pcb);

	nodo_entrada_salida * io = malloc(sizeof(nodo_entrada_salida));
	io->proceso = proceso->proceso;
	log_info(log_planificador,
			"Se creo el hilo para el manejo de entrada salida del programa : %s\n",
			io->proceso->nombrePrograma);
	agregarAColaDeBloqueados(io);
	io->espera = dataHilo->tiempo;
	sleep(io->espera);

	log_info(log_planificador, "Se termino la entrada salida del proceso: %s\n",
			io->proceso->nombrePrograma);

	//TODO removerlo de la cola de bloqueados, pero no popeando, sino con un list-find

	io->proceso->programCounter++;

	liberarCpu(proceso->socket);

	sem_post(&cpu_libre);

	agregarEnColaDeListos(io->proceso);
	sem_post(&procesos_listos);

	pthread_exit(NULL);
}

/* agarra el proceso y coloca su puntero al final de la ultima instruccion para que
 * la cpu ejecute finalizar y termine el proceso */
int setProgramCounter(char *dirProceso) {
	FILE *archivo;
	int pc = 0;
	archivo = fopen(dirProceso, "r");
	if (archivo == NULL) {
		perror("Error opening file");
		return (-1);
	}
	int j;
	while (!feof(archivo)) {
		j = fgetc(archivo);
		if (j == ';') {
			pc++;
		} else if (j == 'f') {

			pc++;
			return pc;
			break;
		}
	}
	return -1;
}
/* GENERA EL CONTEXTO DE EJECUCION Y SE LO ENVIA A LA CPU */
void enviarContextoEjecucion(int socketCliente, int codigoOperacion,
		tipo_pcb *proceso, char *path, char *algoritmo, int quantum) {

	if (!strcmp(algoritmo, "FIFO")) {
		quantum = 0;
	}
	int tamanio = strlen(path) + 1;
	void *buffer = malloc(sizeof(int) * 5 + tamanio);
	memcpy(buffer, &codigoOperacion, sizeof(int));
	memcpy(buffer + sizeof(int), &proceso->programCounter, sizeof(int));
	memcpy(buffer + 2 * sizeof(int), &proceso->id, sizeof(int));
	memcpy(buffer + 3 * sizeof(int), &quantum, sizeof(int));
	memcpy(buffer + 4 * sizeof(int), &tamanio, sizeof(int));
	memcpy(buffer + 5 * sizeof(int), path, tamanio);
	send(socketCliente, buffer, (sizeof(int) * 5) + tamanio, MSG_WAITALL);
	free(buffer);

}
/* DESERIALIZAR UNA INSTRUCCION */
//codigoOperacion - rafagaEjecutada - pid de la cpu - puntero - resultado Instruccion
rafaga_t * deserializarInstruccion(void * data) {
	rafaga_t *instruccion = malloc(sizeof(rafaga_t));
	memcpy(&instruccion->rafagaEjecutada, data, sizeof(int));
	memcpy(&instruccion->pid_cpu, data + sizeof(int), sizeof(int));
	memcpy(&instruccion->PC, data + (sizeof(int) * 2), sizeof(int));
	memcpy(&instruccion->resultado_rafaga, data + (sizeof(int) * 3),
			sizeof(int));
	return instruccion;
}

/*FUNCION QUE INTERPRETA LO RECIBIDO POR ALGUNA CPU*/
void interpretarInstruccion(int instruccion, int socketCliente) {

	void * data = malloc(sizeof(int) * 4);
	void * dataIO = malloc(sizeof(int) * 2);
	int tamanio;
	nodo_en_ejecucion * proceso;
	switch (instruccion) {

	case finalizado:

		recv(socketCliente, &tamanio, sizeof(int), MSG_WAITALL);
		recv(socketCliente, data, sizeof(int) * 4, MSG_WAITALL);

		rafaga_t * instruccion;
		instruccion = deserializarInstruccion(data);

		bool encontrar_cpu(void * nodo) {
			return ((((nodo_en_ejecucion*) nodo)->pid_cpu)
					== instruccion->pid_cpu);
		}

		proceso = list_remove_by_condition(en_ejecucion, encontrar_cpu);

		agregarAFinalizados(colaFinalizados, proceso, log_planificador);

		free(data);

		liberarCpu(proceso->socket);

		sem_post(&cpu_libre);

		break;

	case finquantum:

		recv(socketCliente, &tamanio, sizeof(int), MSG_WAITALL);
		recv(socketCliente, data, sizeof(int) * 4, MSG_WAITALL);

		rafaga_t * unaInstruccion = deserializarInstruccion(data);

		bool encontrar_cpu_finQuantum(void * nodo) {
			return ((((nodo_en_ejecucion*) nodo)->pid_cpu)
					== unaInstruccion->pid_cpu);
		}

		proceso = list_remove_by_condition(en_ejecucion,
				encontrar_cpu_finQuantum);

		proceso->proceso->programCounter = proceso->proceso->programCounter + 1;

		agregarEnColaDeListos(proceso->proceso);

		liberarCpu(proceso->socket);

		sem_post(&cpu_libre);
		sem_post(&procesos_listos);

		free(data);

		break;

	case instruccionFinalizada:
		recv(socketCliente, data, sizeof(data), 0);
		rafaga_t * otraInstruccion = malloc(sizeof(rafaga_t));
		otraInstruccion = deserializarInstruccion(data);
		nodo_en_ejecucion * unProceso = malloc(sizeof(nodo_en_ejecucion));
		unProceso = list_find(en_ejecucion, encontrar_cpu);
		loguearRafaga(otraInstruccion, unProceso, log_planificador);
		unProceso->proceso->programCounter = unProceso->proceso->programCounter + 1;
		free(data);
		break;

	case entrada_salida:

		recv(socketCliente, &tamanio, sizeof(int), MSG_WAITALL);
		recv(socketCliente, dataIO, sizeof(int) * 2, MSG_WAITALL);

		int pid_cpu, tiempoIO;
		memcpy(&pid_cpu, dataIO, sizeof(int));
		memcpy(&tiempoIO, dataIO + sizeof(int), sizeof(int));

		bool encontrar_cpu_io(void * nodo) {
			nodo_en_ejecucion * nodito = nodo;
			return nodito->pid_cpu == pid_cpu;
		}

		proceso = list_find(en_ejecucion, encontrar_cpu_io);

		pthread_t hilo;

		data_hilo * dataHilo = obtenerDatosHilo(proceso, tiempoIO);

		pthread_create(&hilo, NULL, cambiarEstadoABloqueado, dataHilo);

		break;
	}
}

/* MOVER A COLA DE FINALIZADOS */
void agregarAFinalizados(t_queue *finalizados, nodo_en_ejecucion * proceso,
		t_log *log_planificador) {
	queue_push(finalizados, proceso);
	proceso->proceso->tiempoFinalizacion = time(NULL);
	double tiempoDeEjecucion = difftime(proceso->proceso->tiempoFinalizacion,
			proceso->proceso->tiempoComienzo);
	log_info(log_planificador,
			"Finalizado el proceso: %s con pid = %d en la cola de Finalizados.\n Su Tiempo de ejecucion es: %f - Su tiempo de espera es: %f\n",
			proceso->proceso->nombrePrograma, tiempoDeEjecucion,
			proceso->proceso->id, proceso->proceso->tiempoEnReadys);
}
/* LOGUEO DE UNA RAFAGA */
void loguearRafaga(rafaga_t *otraInstruccion, nodo_en_ejecucion * unProceso,
		t_log * log_planificador) {
	if (otraInstruccion->resultado_rafaga == 0) {
		if (otraInstruccion->rafagaEjecutada == 4) {
			log_info(log_planificador,
					"El proceso %s ejecuto la instruccion -iniciar- correctamente ",
					unProceso->proceso->nombrePrograma);
		} else if (otraInstruccion->rafagaEjecutada == 5) {
			log_info(log_planificador,
					"El proceso %s ejecuto la instruccion -leer- correctamente ",
					unProceso->proceso->nombrePrograma);
		} else if (otraInstruccion->rafagaEjecutada == 6) {
			log_info(log_planificador,
					"El proceso %s ejecuto la instruccion -escribir- correctamente ",
					unProceso->proceso->nombrePrograma);
		} else if (otraInstruccion->rafagaEjecutada == 7) {
			log_info(log_planificador,
					"El proceso %s ejecuto la instruccion -entrada salida- correctamente ",
					unProceso->proceso->nombrePrograma);
		} else if (otraInstruccion->rafagaEjecutada == 8) {
			log_info(log_planificador,
					"El proceso %s ejecuto la instruccion -finalizar- correctamente ",
					unProceso->proceso->nombrePrograma);
		}
	}
}
/* */
data_hilo *obtenerDatosHilo(nodo_en_ejecucion *Proceso, int tiempo) {
	data_hilo * dataHilo = malloc(sizeof(data_hilo));
	dataHilo->pcb = Proceso->proceso;
	dataHilo->tiempo = tiempo;
	return dataHilo;
}
/* FUNCION QUE ENVIA MSJ A LA CPU PARA AVERIGUAR SU PORCENTAJE */
void peticionPorcentajeUsoCpu(t_list * lista, int codigo) {
	int j;
	if (list_size(lista) == 0) {
		printf("No hay procesos en ejecución\n");

	} else {

		for (j = 0; j < list_size(lista); j++) {
			nodo_en_ejecucion *procesoEnEjecucion = list_get(lista, j);
			send(procesoEnEjecucion->socket, &codigo, sizeof(int), MSG_WAITALL);
		}

	}
}

void * ejecutarIngresoConsola() {
	int pid_a_finalizar = -1;
	while (1) {

		codigoOperacion = reconocerIdentificador();
		char *nombreProceso = malloc(512);
		switch (codigoOperacion) {

		case 1:/* correr */

			scanf("%s", nombreProceso);
			char *path = malloc(256);

			strcpy(path, "/tp-2015-2c-signiorcodigo/programas/");
			strcat(path, nombreProceso);

			printf("el path a enviar es: %s \n", path);

			int pid;
			pid = generarPID(&p_last_id);

			tipo_pcb *proceso = malloc(sizeof(tipo_pcb));
			proceso = generarPCB(pid, path, listo, nombreProceso);

			agregarEnColaDeListos(proceso);

			sem_post(&procesos_listos);

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
			agregarEnColaDeListos(procesoEnEjecucion->proceso);
			sem_post(&procesos_listos);
			free(procesoEnEjecucion);

			break;
		case 2:
			/* ps */

			mostrarEstadoDeLista(en_ejecucion, "Ejecutando", log_planificador);
			mostrarEstadoDeListos(colaListos, "Listo", log_planificador);
			mostrarEstadoDeBloqueados(entradaSalida, "Bloqueados",
					log_planificador);

			break;
		case 3:
			/* cpu */
			peticionPorcentajeUsoCpu(en_ejecucion, 3);
			break;
		}

	}
	return NULL;
}

void despacharProcesosListos(void * data) {

	while (1) {

		sem_wait(&cpu_libre);
		sem_wait(&procesos_listos);

		pthread_mutex_lock(&mutex_cpus);
		t_cpu * cpuLibre = list_find(listaCpu, cpuEstaLibre);
		pthread_mutex_unlock(&mutex_cpus);

		cambiarAEstadoDeEjecucion(cpuLibre->socket, cpuLibre->cpu_id);

		pthread_mutex_lock(&mutex_ejecucion);

		int index = list_size(en_ejecucion) - 1;

		nodo_en_ejecucion * procesoEjecutando = list_get(en_ejecucion, index);

		pthread_mutex_unlock(&mutex_ejecucion);

		enviarContextoEjecucion(cpuLibre->socket, 1, procesoEjecutando->proceso,
				procesoEjecutando->proceso->dirProceso, getAlgoritmo(),
				getQuantum());

	}

}

bool cpuEstaLibre(void * data) {
	t_cpu * cpu = data;
	return cpu->libre;
}

void liberarCpu(int socket) {

	bool coincideSocket(void * data) {
		t_cpu * cpu = data;
		return cpu->socket == socket;
	}

	pthread_mutex_lock(&mutex_cpus);

	t_cpu * cpuMatcheaSocket = list_find(listaCpu, coincideSocket);

	pthread_mutex_unlock(&mutex_cpus);
	cpuMatcheaSocket->libre = true;

}
