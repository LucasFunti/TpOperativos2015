/*
 * planificadorFunctions.c
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <signiorCodigo/libSockets.h>
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
	planificador_config =
			config_create(
					"/tp-2015-2c-signiorcodigo/planificador/planificadorConfig");
	char * puerto = config_get_string_value(planificador_config,
			"PUERTO_ESCUCHA");
	return puerto;
}

char *getAlgoritmo() {
	t_config *planificador_config;
	planificador_config =
			config_create(
					"/tp-2015-2c-signiorcodigo/planificador/planificadorConfig");
	char* algoritmo = config_get_string_value(planificador_config,
			"ALGORITMO_PLANIFICACION");
	return algoritmo;
}

int getQuantum() {
	t_config *planificador_config;
	planificador_config =
			config_create(
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
void cambiarEstado(tipo_pcb *proceso, int estado, t_queue*colaListos,
		t_queue*entrada_salida, t_list*en_ejecucion, t_log * log_planificador) {
	proceso->estado = estado;
	mostrarEstadoDeListos(colaListos, "Listos", log_planificador);
	mostrarEstadoDeLista(en_ejecucion, "Ejecucion", log_planificador);
	mostrarEstadoDeBloqueados(entrada_salida, "Bolqueados", log_planificador);
}

/*agrega un proceso a la cola de listos */
void agregarEnColaDeListos(tipo_pcb *proceso, pthread_mutex_t mutex_readys,
		t_queue *colaListos, t_log*log_planificador, t_queue*entrada_salida,
		t_list*en_ejecucion) {
	pthread_mutex_lock(&mutex_readys);
	queue_push(colaListos, proceso);
	log_info(log_planificador,
			"Colocado el proceso: %s con pid = %d en la cola de listos",
			proceso->nombrePrograma, proceso->id);
	proceso->tiempoIngreso = time(NULL);
	cambiarEstado(proceso, listo, colaListos, entrada_salida, en_ejecucion,
			log_planificador);
	pthread_mutex_unlock(&mutex_readys);

}

tipo_pcb * removerDeColaDeListos(pthread_mutex_t mutex_readys,
		t_queue*colaListos) {
	tipo_pcb *proceso = malloc(sizeof(tipo_pcb));
	pthread_mutex_lock(&mutex_readys);
	proceso = queue_pop(colaListos);
	time_t horaActual = time(NULL);
	proceso->tiempoEnReadys = difftime(horaActual, proceso->tiempoIngreso);
	pthread_mutex_unlock(&mutex_readys);
	return proceso;
}
void agregarAListaDeEjecucion(pthread_mutex_t mutex_ejecucion,
		t_list *listaEjecutando, nodo_en_ejecucion *proceso, t_queue*colaListos,
		t_queue*entrada_salida, t_log*log_planificador) {
	pthread_mutex_lock(&mutex_ejecucion);
	list_add(listaEjecutando, proceso);
	log_info(log_planificador,
			"Colocado el proceso: %s con pid = %d en la cola de ejecucion",
			proceso->proceso->nombrePrograma, proceso->proceso->id);
	cambiarEstado(proceso->proceso, ejecucion, colaListos, entrada_salida,
			listaEjecutando, log_planificador);
	pthread_mutex_unlock(&mutex_ejecucion);

}
/* saca un proceso de la cola de listos y lo coloca en la lista de ejecucion*/
void cambiarAEstadoDeEjecucion(pthread_mutex_t mutex_readys, t_queue*colaListos,
		pthread_mutex_t mutex_ejecucion, t_list *listaEjecutando,
		t_queue*entrada_salida, t_log*log_planificador, int socketEnEjecucion,
		int pid_cpu) {
	tipo_pcb *proceso = malloc(sizeof(tipo_pcb));
	proceso = removerDeColaDeListos(mutex_readys, colaListos);
	nodo_en_ejecucion*procesoEjecutando = malloc(sizeof(nodo_en_ejecucion));
	procesoEjecutando->proceso = proceso;
	procesoEjecutando->socket = socketEnEjecucion;
	procesoEjecutando->pid_cpu = pid_cpu;
	procesoEjecutando->instrucciones_ejecutadas = 0;
	agregarAListaDeEjecucion(mutex_ejecucion, listaEjecutando,
			procesoEjecutando, colaListos, entrada_salida, log_planificador);
}

nodo_en_ejecucion * removerDeListaDeEjecucion(tipo_pcb *pcb,
		pthread_mutex_t mutex_ejecucion, t_list*listaEjecutando) {
	nodo_en_ejecucion * proceso = malloc(sizeof(nodo_en_ejecucion));
	bool encontrar_pid(void * nodo) {
		return ((((nodo_en_ejecucion*) nodo)->proceso->id) == pcb->id);
	}
	pthread_mutex_lock(&mutex_ejecucion);
	proceso = list_remove_by_condition(listaEjecutando, encontrar_pid);
	pthread_mutex_unlock(&mutex_ejecucion);
	return proceso;

}
void agregarAColaDeBloqueados(pthread_mutex_t mutex_bloqueados,
		t_queue *entradaSalida, nodo_entrada_salida*io, t_queue*colaListos,
		t_list*listaEjecutando, t_log*log_planificador) {
	pthread_mutex_lock(&mutex_bloqueados);
	queue_push(entradaSalida, io);
	log_info(log_planificador,
			"Colocado el proceso: %s con pid = %d en la cola de bloqueados",
			io->proceso->nombrePrograma, io->proceso->id);
	cambiarEstado(io->proceso, bloqueado, colaListos, entradaSalida,
			listaEjecutando, log_planificador);
	pthread_mutex_unlock(&mutex_bloqueados);

}
/* saca un proceso de la lista de ejecucion y lo coloca en la cola de entrada salida */
void * cambiarEstadoABloqueado(void* data) {
	data_hilo *dataHilo = data;
	nodo_en_ejecucion * proceso = malloc(sizeof(nodo_en_ejecucion));
	proceso = removerDeListaDeEjecucion(dataHilo->pcb,
			dataHilo->mutex_ejecucion, dataHilo->listaEjecutando);

	nodo_entrada_salida * io = malloc(sizeof(nodo_entrada_salida));
	io->proceso = proceso->proceso;
	log_info(dataHilo->log_planificador,"Se creo el hilo para el manejo de entrada salida del programa : %s\n",io->proceso->nombrePrograma);
	agregarAColaDeBloqueados(dataHilo->mutex_bloqueados,
			dataHilo->entradaSalida, io, dataHilo->colaListos,
			dataHilo->listaEjecutando, dataHilo->log_planificador);
	io->espera = dataHilo->tiempo;
	sleep(io->espera);
	log_info(dataHilo->log_planificador,"Se termino la entrada salida del proceso: %s\n",io->proceso->nombrePrograma);
	io->proceso->programCounter = io->proceso->programCounter + 1;
	agregarEnColaDeListos(io->proceso, dataHilo->mutex_readys,
			dataHilo->colaListos, dataHilo->log_planificador,
			dataHilo->entradaSalida, dataHilo->listaEjecutando);
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
	free(path);

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
void interpretarInstruccion(int instruccion, int socketCliente,
		pthread_mutex_t mutex_readys, t_queue *colaListos,
		t_log*log_planificador, t_queue*entradaSalida, t_list*en_ejecucion,
		t_queue*finalizados, pthread_mutex_t mutex_bloqueados,
		pthread_mutex_t mutex_ejecucion) {

	void *data = malloc(sizeof(int) * 5);
	void * dataIO = malloc(sizeof(int) * 2);
	switch (instruccion) {
	case finalizado:
		recv(socketCliente, data, sizeof(data), 0);
		rafaga_t * instruccion = malloc(sizeof(rafaga_t));
		instruccion = deserializarInstruccion(data);
		nodo_en_ejecucion * procesoAFinalizar = malloc(
				sizeof(nodo_en_ejecucion));
		bool encontrar_cpu(void * nodo) {
			return ((((nodo_en_ejecucion*) nodo)->pid_cpu)
					== instruccion->pid_cpu);
		}

		procesoAFinalizar = list_remove_by_condition(en_ejecucion,
				encontrar_cpu);
		agregarAFinalizados(finalizados, procesoAFinalizar, log_planificador);
		log_info(log_planificador,
				"Se cerro la conexión con la cpu %d en el socket %d",
				instruccion->pid_cpu, socketCliente);
		free(data);
		break;
	case finquantum:
		recv(socketCliente, data, sizeof(data), 0);
		rafaga_t * unaInstruccion = malloc(sizeof(rafaga_t));
		unaInstruccion = deserializarInstruccion(data);
		bool encontrar_cpu_finQuantum(void * nodo) {
			return ((((nodo_en_ejecucion*) nodo)->pid_cpu)
					== unaInstruccion->pid_cpu);
		}
		nodo_en_ejecucion *proceso = malloc(sizeof(nodo_en_ejecucion));
		proceso = list_remove_by_condition(en_ejecucion,
				encontrar_cpu_finQuantum);
		agregarEnColaDeListos(proceso->proceso, mutex_readys, colaListos,
				log_planificador, entradaSalida, en_ejecucion);
		proceso->proceso->programCounter = proceso->proceso->programCounter + 1;
		free(data);
		break;
	case instruccionFinalizada:
		recv(socketCliente, data, sizeof(data), 0);
		rafaga_t * otraInstruccion = malloc(sizeof(rafaga_t));
		otraInstruccion = deserializarInstruccion(data);
		nodo_en_ejecucion * unProceso = malloc(sizeof(nodo_en_ejecucion));
		unProceso = list_find(en_ejecucion, encontrar_cpu);
		unProceso->instrucciones_ejecutadas =
				unProceso->instrucciones_ejecutadas + 1;
		loguearRafaga(otraInstruccion, unProceso, log_planificador);
		unProceso->proceso->programCounter = unProceso->proceso->programCounter + 1;
		free(data);
		break;
	case entrada_salida:
		recv(socketCliente, dataIO, sizeof(int), MSG_WAITALL);
		recv(socketCliente, dataIO, sizeof(int) * 2, MSG_WAITALL);
		int pid_cpu, tiempoIO;
		memcpy(&pid_cpu, dataIO, sizeof(int));
		memcpy(&tiempoIO, dataIO + sizeof(int), sizeof(int));
		bool encontrar_cpu_io(void * nodo) {
			nodo_en_ejecucion * nodito = nodo;
			return nodito->pid_cpu == pid_cpu;
		}
		nodo_en_ejecucion * Proceso = malloc(sizeof(nodo_en_ejecucion));
		Proceso = list_find(en_ejecucion, encontrar_cpu_io);
		pthread_t hilo;
		data_hilo *dataHilo = malloc(sizeof(data_hilo));
		dataHilo = obtenerDatosHilo(dataHilo, Proceso, mutex_readys, colaListos,
				log_planificador, entradaSalida, en_ejecucion, finalizados,
				mutex_bloqueados, mutex_ejecucion,tiempoIO);
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
data_hilo *obtenerDatosHilo(data_hilo * dataHilo, nodo_en_ejecucion *Proceso,
		pthread_mutex_t mutex_readys, t_queue *colaListos,
		t_log*log_planificador, t_queue*entradaSalida, t_list*en_ejecucion,
		t_queue*finalizados, pthread_mutex_t mutex_bloqueados,
		pthread_mutex_t mutex_ejecucion, int tiempo) {
	dataHilo->pcb = Proceso->proceso;
	dataHilo->colaListos = colaListos;
	dataHilo->entradaSalida = entradaSalida;
	dataHilo->listaEjecutando = en_ejecucion;
	dataHilo->log_planificador = log_planificador;
	dataHilo->mutex_bloqueados = mutex_bloqueados;
	dataHilo->mutex_ejecucion = mutex_ejecucion;
	dataHilo->mutex_readys = mutex_readys;
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
			send(procesoEnEjecucion->socket, &codigo, sizeof(int), 0);
			int pid_cpu, porcentaje;
			recv(procesoEnEjecucion->socket, &pid_cpu, sizeof(int), 0);
			recv(procesoEnEjecucion->socket, &porcentaje, sizeof(int), 0);
			printf("Cpu %d: %d\n", pid_cpu, porcentaje);
		}

	}
}

void * ejecutarIngresoConsola(){
	int pid_a_finalizar=-1;
	while(1){
		codigoOperacion = reconocerIdentificador();
		char *nombreProceso = malloc(512);
		switch (codigoOperacion) {
		case 1:/* correr */
			scanf("%s", nombreProceso);
			char *path = malloc(256);
//			realpath(nombreProceso, path);
			strcpy(path,"/tp-2015-2c-signiorcodigo/programas/");
			strcat(path,nombreProceso);

			printf("el path a enviar es: %s \n", path);
			int pid;
			pid = generarPID(&p_last_id);

			tipo_pcb *proceso = malloc(sizeof(tipo_pcb));
			proceso = generarPCB(pid, path, listo, nombreProceso);

			agregarEnColaDeListos(proceso, mutex_readys, colaListos,
					log_planificador, entradaSalida, en_ejecucion);

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
					colaListos, log_planificador, entradaSalida, en_ejecucion);
			free(procesoEnEjecucion);

			break;
		case 2:
			/* ps */

			mostrarEstadoDeLista(en_ejecucion, "Ejecutando", log_planificador);
			mostrarEstadoDeListos(colaListos, "Listo", log_planificador);
			mostrarEstadoDeBloqueados(entradaSalida, "Bloqueados",log_planificador);

			break;
		case 3:
			/* cpu */
			peticionPorcentajeUsoCpu(en_ejecucion, 3);
			break;
		}

	}
	return NULL;
}
