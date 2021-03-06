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
	int quantum = config_get_int_value(planificador_config, "QUANTUM");
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
void mostrarEstadoDeLista(char*estado,int codigoOperacion) {
	int j;
	if (list_size(en_ejecucion) == 0) {
		if(codigoOperacion == 2){
			printf("La lista de planificacion, %s, esta vacía\n",
							estado);
			log_info(log_planificador, "La lista de planificacion, %s, esta vacía",
							estado);

		}else{
		log_info(log_planificador, "La lista de planificacion, %s, esta vacía",
				estado);
		}
	} else {

		for (j = 0; j < list_size(en_ejecucion); j++) {
			nodo_en_ejecucion *procesoEnEjecucion = list_get(en_ejecucion, j);
			if(codigoOperacion == 2){
				printf("mProc %d: %s -> %s \n",
						procesoEnEjecucion->proceso->id,
						procesoEnEjecucion->proceso->nombrePrograma, estado);
				log_info(log_planificador, "mProc %d: %s -> %s ",
									procesoEnEjecucion->proceso->id,
									procesoEnEjecucion->proceso->nombrePrograma, estado);
			}else{
			log_info(log_planificador, "mProc %d: %s -> %s ",
					procesoEnEjecucion->proceso->id,
					procesoEnEjecucion->proceso->nombrePrograma, estado);
			}
		}

	}
}

void mostrarEstadoDeListos(char*estado,int codigoOperacion) {
	int j;
	if (queue_size(colaListos) == 0) {
		if(codigoOperacion == 2){
			printf("La cola de planificacion, %s, esta vacía\n",
				estado);
			log_info(log_planificador, "La cola de planificacion, %s, esta vacía",
				estado);
		}else{
			log_info(log_planificador, "La cola de planificacion, %s, esta vacía",
							estado);
		}
	} else {
		for (j = 0; j < queue_size(colaListos); j++) {
			tipo_pcb *proceso = list_get(colaListos->elements, j);
			if(codigoOperacion == 2){
				printf("mProc %d: %s -> %s \n", proceso->id,
					proceso->nombrePrograma, estado);
			log_info(log_planificador, "mProc %d: %s -> %s ", proceso->id,
					proceso->nombrePrograma, estado);
			}else{
				log_info(log_planificador, "mProc %d: %s -> %s", proceso->id,
									proceso->nombrePrograma, estado);

			}
		}
	}
}
void mostrarEstadoDeBloqueados(char*estado,int codigoOperacion) {
	int j;
	if (queue_size(entradaSalida) == 0) {
		if(codigoOperacion == 2){
			printf("La cola de planificacion, %s, esta vacía\n",
							estado);
			log_info(log_planificador, "La cola de planificacion, %s, esta vacía",
							estado);

		}else{
		log_info(log_planificador, "La cola de planificacion, %s, esta vacía",
				estado);
		}
	} else {
		for (j = 0; j < queue_size(entradaSalida); j++) {
			nodo_en_ejecucion *proceso = list_get(entradaSalida->elements, j);
			if(codigoOperacion == 2){
				printf("mProc %d: %s -> %s \n",
						proceso->proceso->id, proceso->proceso->nombrePrograma,
						estado);
				log_info(log_planificador, "mProc %d: %s -> %s",
									proceso->proceso->id, proceso->proceso->nombrePrograma,
									estado);

			}else{
			log_info(log_planificador, "mProc %d: %s -> %s",
					proceso->proceso->id, proceso->proceso->nombrePrograma,
					estado);
			}
		}
	}
}

/* cambiar estado del proceso */
void cambiarEstado(tipo_pcb *proceso, int estado) {
	proceso->estado = estado;
	mostrarEstadoDeListos("Listos",0);
	mostrarEstadoDeLista("Ejecucion",0);
	mostrarEstadoDeBloqueados("Bloqueados",0);
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
	proceso->tiempoEnReadys = proceso->tiempoEnReadys
			+ difftime(horaActual, proceso->tiempoIngreso);
	log_info(log_planificador,
			"Se removió el proceso: %s con pid = %d de la cola de listos",
			proceso->nombrePrograma, proceso->id);
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

	agregarAListaDeEjecucion(procesoEjecutando);

}

nodo_en_ejecucion * removerDeListaDeEjecucion(tipo_pcb *pcb) {

	bool encontrar_pid(void * nodo) {
		return ((((nodo_en_ejecucion*) nodo)->proceso->id) == pcb->id);
	}

	pthread_mutex_lock(&mutex_ejecucion);

	nodo_en_ejecucion * proceso = list_remove_by_condition(en_ejecucion,
			encontrar_pid);

	log_info(log_planificador,
			"Se removió el proceso: %s con pid = %d de la cola de ejecucion",
			proceso->proceso->nombrePrograma, proceso->proceso->id);

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
	sem_post(&input_output);

}
nodo_entrada_salida * quitarDeColaBloqueados() {

	nodo_entrada_salida * io = queue_pop(entradaSalida);

	log_info(log_planificador,
				"Se removió el proceso: %s con pid = %d de la cola de bloqueados",
				io->proceso->nombrePrograma, io->proceso->id);

	return io;
}

/* saca un proceso de la lista de ejecucion y lo coloca en la cola de entrada salida */
void cambiarEstadoABloqueado(void* data) {

	data_hilo *dataHilo = data;

	nodo_en_ejecucion * proceso = removerDeListaDeEjecucion(dataHilo->pcb);

	liberarCpu(proceso->socket);

	sem_post(&cpu_libre);

	nodo_entrada_salida * io = malloc(sizeof(nodo_entrada_salida));

	io->proceso = proceso->proceso;

	io->espera = dataHilo->tiempo;

	agregarAColaDeBloqueados(io);
}

void * manejarEntradaSalida() {
	while (1) {

		sem_wait(&input_output);

		pthread_mutex_lock(&mutex_entrada_salida);

		nodo_entrada_salida * io = quitarDeColaBloqueados();

		pthread_mutex_unlock(&mutex_entrada_salida);

		log_info(log_planificador, "mProc %s en entrada-salida de tiempo %d",
				io->proceso->nombrePrograma, io->espera);

		sleep(io->espera);

		agregarEnColaDeListos(io->proceso);

		sem_post(&procesos_listos);


	}
	return NULL;
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
	log_info(log_planificador,"Se envio el contexto de ejecucion a la cpu con socket asignado número: %d",socketCliente);
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

void ejecutarlogueoInstruccionesEjecutadas(void * data, int cantidadResultados,
		nodo_en_ejecucion * proceso) {

	int i;
	for (i = 0; i < cantidadResultados; i++) {

		rafaga_t * instruccion = malloc(sizeof(rafaga_t));

		memcpy(&instruccion->rafagaEjecutada, data + i * 2 * (sizeof(int)),
				sizeof(int));

		memcpy(&instruccion->resultado_rafaga,
				data + i + sizeof(int) * 2 * (sizeof(int)), sizeof(int));

		loguearRafaga(instruccion, proceso);

		free(instruccion);

	}
}
/*FUNCION QUE INTERPRETA LO RECIBIDO POR ALGUNA CPU*/
void interpretarInstruccion(int instruccion, int socketCliente) {

	void * dataIO = malloc(sizeof(int) * 2);
	int tamanio;
	int pid;
	int cantidadResultados;
	nodo_en_ejecucion * proceso;
	switch (instruccion) {

	case finalizado:

		recv(socketCliente, &tamanio, sizeof(int), MSG_WAITALL);
		void * data = malloc(tamanio - 2 *sizeof(int));

		rafaga_t *instruccion = malloc(sizeof(rafaga_t));

		recv(socketCliente,&instruccion->pid_cpu,sizeof(int), MSG_WAITALL);

		recv(socketCliente,&cantidadResultados, sizeof(int), MSG_WAITALL);

		recv(socketCliente, data, tamanio - 2 * sizeof(int), MSG_WAITALL);

		bool encontrar_cpu(void * nodo) {
			return ((((nodo_en_ejecucion*) nodo)->pid_cpu)
					== instruccion->pid_cpu);
		}

		proceso = list_remove_by_condition(en_ejecucion, encontrar_cpu);
		if(proceso->proceso->tiempoRespuesta == NULL){
			proceso->proceso->tiempoRespuesta = time(NULL);
		}
		ejecutarlogueoInstruccionesEjecutadas(data, cantidadResultados,
				proceso);

		agregarAFinalizados(colaFinalizados, proceso, log_planificador);

		free(data);

		liberarCpu(proceso->socket);

		log_info(log_planificador,"Se libera la cpu: %d por finalizacion de proceso",proceso->pid_cpu);

		sem_post(&cpu_libre);

		break;

	case finquantum:

		recv(socketCliente, &tamanio, sizeof(int), MSG_WAITALL);
		data = malloc(tamanio - 3 * sizeof(int));

		rafaga_t * unaInstruccion = malloc(sizeof(rafaga_t));
		recv(socketCliente,&unaInstruccion->pid_cpu, sizeof(int),MSG_WAITALL);
		recv(socketCliente,&unaInstruccion->PC, sizeof(int),MSG_WAITALL);
		recv(socketCliente,&cantidadResultados, sizeof(int) , MSG_WAITALL);

		recv(socketCliente, data, tamanio - 3 * sizeof(int), MSG_WAITALL);

		bool encontrar_cpu_finQuantum(void * nodo) {
			return ((((nodo_en_ejecucion*) nodo)->pid_cpu)
					== unaInstruccion->pid_cpu);
		}

		proceso = list_remove_by_condition(en_ejecucion,
				encontrar_cpu_finQuantum);

		if(proceso->proceso->tiempoRespuesta == NULL){
			proceso->proceso->tiempoRespuesta = time(NULL);
		}

		ejecutarlogueoInstruccionesEjecutadas(data, cantidadResultados,
				proceso);

		proceso->proceso->programCounter = unaInstruccion->PC;

		agregarEnColaDeListos(proceso->proceso);

		liberarCpu(proceso->socket);

		log_info(log_planificador,"Se libera la cpu: %d por fin de quantum",proceso->pid_cpu);

		sem_post(&cpu_libre);

		sem_post(&procesos_listos);

		free(data);

		break;

	case fallaEjecucion:

		recv(socketCliente,&pid,sizeof(int),MSG_WAITALL);
		recv(socketCliente, &pid, sizeof(int), MSG_WAITALL);

		bool encontrar_cpu_falla(void * nodo) {
			return ((((nodo_en_ejecucion *) nodo)->proceso->id) == pid);
		}

		proceso = list_remove_by_condition(en_ejecucion, encontrar_cpu_falla);

		if(proceso->proceso->tiempoRespuesta == 0){
			proceso->proceso->tiempoRespuesta = time(NULL);
		}

		liberarCpu(proceso->socket);

		log_info(log_planificador,"Se libera la cpu: %d por falla de ejecucion del proceso %d",proceso->pid_cpu,proceso->proceso->id);

		sem_post(&cpu_libre);

		break;

	case entrada_salida:

		recv(socketCliente, &tamanio, sizeof(int), MSG_WAITALL);

		dataIO = malloc(tamanio - 4 * sizeof(int));

		int pid_cpu, tiempoIO, punteroActualizado;

		recv(socketCliente, &pid_cpu, sizeof(int), MSG_WAITALL);
		recv(socketCliente, &tiempoIO, sizeof(int), MSG_WAITALL);
		recv(socketCliente, &punteroActualizado, sizeof(int), MSG_WAITALL);
		recv(socketCliente, &cantidadResultados, sizeof(int), MSG_WAITALL);

		recv(socketCliente, dataIO, tamanio - 4 * (sizeof(int)), MSG_WAITALL);

		bool encontrar_cpu_io(void * nodo) {
			nodo_en_ejecucion * nodito = nodo;
			return nodito->pid_cpu == pid_cpu;
		}

		proceso = list_find(en_ejecucion, encontrar_cpu_io);

		if(proceso->proceso->tiempoRespuesta == NULL){
			proceso->proceso->tiempoRespuesta = time(NULL);
		}

		ejecutarlogueoInstruccionesEjecutadas(dataIO, cantidadResultados,
				proceso);

		proceso->proceso->programCounter = punteroActualizado;

		data_hilo * dataHilo = obtenerDatosHilo(proceso, tiempoIO);

		cambiarEstadoABloqueado(dataHilo);

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
	double tiempoRespuesta = difftime(proceso->proceso->tiempoRespuesta,
			proceso->proceso->tiempoComienzo);
	log_info(log_planificador,
			"Finalizado el proceso: %s con pid = %d \n Movido a la cola de Finalizados.\n Su Tiempo de ejecucion es: %f \n Su tiempo de espera es: %f \n Su  tiempo de respuesta es: %f\n",
			proceso->proceso->nombrePrograma, proceso->proceso->id,
			tiempoDeEjecucion, proceso->proceso->tiempoEnReadys,
			tiempoRespuesta);
}
/* LOGUEO DE UNA RAFAGA */
void loguearRafaga(rafaga_t *otraInstruccion, nodo_en_ejecucion * unProceso) {
	if (otraInstruccion->rafagaEjecutada == 4) {
		log_info(log_planificador,
				"El proceso %s ejecuto la instruccion -iniciar- con valor : %s",
				unProceso->proceso->nombrePrograma,
				otraInstruccion->resultado_rafaga ? "true" : "false");
	} else if (otraInstruccion->rafagaEjecutada == 5) {
		log_info(log_planificador,
				"El proceso %s ejecuto la instruccion -leer- con valor : %s",
				unProceso->proceso->nombrePrograma,
				otraInstruccion->resultado_rafaga ? "true" : "false");
	} else if (otraInstruccion->rafagaEjecutada == 6) {
		log_info(log_planificador,
				"El proceso %s ejecuto la instruccion -escribir- con valor : %s",
				unProceso->proceso->nombrePrograma,
				otraInstruccion->resultado_rafaga ? "true" : "false");
	} else if (otraInstruccion->rafagaEjecutada == 7) {
		log_info(log_planificador,
				"El proceso %s ejecuto la instruccion -entrada salida- con valor : %s",
				unProceso->proceso->nombrePrograma,
				otraInstruccion->resultado_rafaga ? "true" : "false");
	} else if (otraInstruccion->rafagaEjecutada == 8) {
		log_info(log_planificador,
				"El proceso %s ejecuto la instruccion -finalizar- con valor : %s",
				unProceso->proceso->nombrePrograma,
				otraInstruccion->resultado_rafaga ? "true" : "false");
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
void peticionPorcentajeUsoCpu(int codigo) {
	int j;
	for (j = 0; j < list_size(listaCpu); j++) {
		t_cpu *unaCpu = list_get(listaCpu, j);
		send(unaCpu->socket, &codigo, sizeof(int), MSG_WAITALL);
	}
}

void * ejecutarIngresoConsola() {
	int pid_a_finalizar = -2;
	while (1) {

		codigoOperacion = reconocerIdentificador();
		char *nombreProceso = malloc(512);
		switch (codigoOperacion) {

		case 1:/* correr */

			scanf("%s", nombreProceso);
			char *path = malloc(256);

			strcpy(path, "/tp-2015-2c-signiorcodigo/programas/");
			strcat(path, nombreProceso);

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

			nodo_en_ejecucion *procesoEnEjecucion = list_find(en_ejecucion, encontrar_pid);

			if(procesoEnEjecucion == NULL){
				log_info(log_planificador,"El proceso a finalizar con pid: %d, no se encuentra en la cola de ejecucion");

			}else{

			log_info(log_planificador,"Se ejecuta la instruccion Finalizar PID sobre el proceso con pid: %d",procesoEnEjecucion->proceso->id);

			int codigo = 99;

			send(procesoEnEjecucion->socket,&codigo,sizeof(int),MSG_WAITALL);
			}
			break;
		case 2:
			/* ps */

			mostrarEstadoDeLista("Ejecutando",2);
			mostrarEstadoDeListos( "Listo",2);
			mostrarEstadoDeBloqueados( "Bloqueados",2);

			break;
		case 3:
			/* cpu */

			peticionPorcentajeUsoCpu(3);

			break;
		}

	}
	return NULL;
}

void *despacharProcesosListos() {

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

		cpuLibre->libre = false;

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
