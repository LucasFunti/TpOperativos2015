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
#include "planificadorFunctions.h"

void inicializarMutex() {
	pthread_mutex_init(&mutex_readys, NULL);
	pthread_mutex_init(&mutex_ejecucion, NULL);
	pthread_mutex_init(&mutex_bloqueados, NULL);
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
	pcb->dirProceso = malloc(strlen(path));
	pcb->dirProceso = path;
	pcb->estado = estado;
	pcb->programCounter = 0;
	pcb->nombrePrograma = malloc(strlen(nombre));
	pcb->nombrePrograma = nombre;

	return pcb;
}
/* Leer el archivo de config y guardarla en una estructura */
char *getPuerto() {
	t_config *planificador_config;
	planificador_config =
			config_create(
					"/home/utnso/git/tp-2015-2c-signiorcodigo/planificador/planificadorConfig");
	char* puerto = config_get_string_value(planificador_config,
			"PUERTO_ESCUCHA");
	return puerto;
}

char *getAlgoritmo() {
	t_config *planificador_config;
	planificador_config =
			config_create(
					"/home/utnso/git/tp-2015-2c-signiorcodigo/planificador/planificadorConfig");
	char* algoritmo = config_get_string_value(planificador_config,
			"ALGORITMO_PLANIFICACION");
	return algoritmo;
}

/* inicializar las estructuras para el planificador */
<<<<<<< HEAD
void inicializarColecciones() {
=======
void inicializarColecciones(t_list * listaNuevos, t_queue * colaListos,
	t_queue * colaFinalizados, t_queue * cola_cpu_libres, t_list * listaEjecutando, t_list * entradaSalida) {
	listaNuevos = list_create();
>>>>>>> b49e770e571f7b349561922e05d0e270911eed9b
	colaListos = queue_create();
	colaFinalizados = queue_create();
	cola_cpu_libres = queue_create();
	listaEjecutando = list_create();
	entradaSalida = queue_create();
}


/* recorro la lissta mostrando los estado de los procesos para la instruccion ps */
void mostrarEstadoDeLista(t_list *lista, char*estado) {
	int j;
	for (j = 0; j < list_size(lista); j++) {
		nodo_en_ejecucion *procesoEnEjecucion = malloc(
				sizeof(nodo_en_ejecucion));
		procesoEnEjecucion = list_get(lista, j);
		printf("mProc %d: %s -> %s \n", procesoEnEjecucion->proceso.pcb.id,
				procesoEnEjecucion->proceso.pcb.nombrePrograma, estado);
		free(procesoEnEjecucion);
	}
}

void mostrarEstadoDeCola(t_queue *cola, char*estado) {
	int j;
	for (j = 0; j < queue_size(cola); j++) {
		nodo_proceso *proceso = malloc(sizeof(nodo_proceso));
		proceso = list_get(cola->elements, j);
		printf("mProc %d: %s -> %s \n", proceso->pcb.id,
				proceso->pcb.nombrePrograma, estado);
		free(proceso);
	}
}

/*agrega un proceso a la cola de listos */
void agregarEnColaDeListos(nodo_proceso *proceso) {
	pthread_mutex_lock(&mutex_readys);
	queue_push(colaListos, &proceso);
	log_info(log_planificador,
			"Colocado el proceso: %s con pid = %d en la cola de listos",
			proceso, proceso->pcb.id);
	pthread_mutex_unlock(&mutex_readys);
}
/* cambiar estado del proceso */
void cambiarEstado(nodo_proceso *proceso, int estado) {
	proceso->pcb.estado = estado;
}

/* saca un proceso de la cola de listos y lo coloca en la lista de ejecucion*/
void cambiarAEstadoDeEjecucion() {
	nodo_proceso *proceso = malloc(sizeof(nodo_proceso));
	pthread_mutex_lock(&mutex_readys);
	proceso = queue_pop(colaListos);
	pthread_mutex_unlock(&mutex_readys);
	pthread_mutex_lock(&mutex_ejecucion);
	list_add(listaEjecutando, proceso);
	cambiarEstado(proceso, ejecucion);
	pthread_mutex_unlock(&mutex_ejecucion);
}
/* saca un proceso de la lista de ejecucion y lo coloca en la cola de entrada salida */
void agregarEnColaDeBloqueados(tipo_pcb pcb, int espera) {

	bool encontrar_pid(void * nodo) {
		return ((((nodo_en_ejecucion*) nodo)->proceso.pcb.id) == pcb.id);
	}
	pthread_mutex_lock(&mutex_ejecucion);
	list_remove_by_condition(listaEjecutando, encontrar_pid);
	pthread_mutex_unlock(&mutex_ejecucion);
	nodo_entrada_salida * io = malloc(sizeof(nodo_entrada_salida));

	io->proceso->pcb = pcb;
	pthread_mutex_lock(&mutex_bloqueados);
	queue_push(entradaSalida, io);
	cambiarEstado(io->proceso, bloqueado);
	pthread_mutex_unlock(&mutex_bloqueados);
	//manejo de los tiempos de entrada salida
	sleep(io->espera);
	agregarEnColaDeListos(io->proceso);

}

//void agregarDeBloqueadoAListo() {
//	pthread_mutex_lock(&mutex_bloqueados);
//	nodo_entrada_salida *bloqueado = malloc(sizeof(bloqueado));
//	bloqueado = queue_pop(entradaSalida);
//	pthread_mutex_unlock(&mutex_bloqueados);
//	nodo_proceso *proceso = malloc(sizeof(nodo_proceso));
//	proceso->pcb = bloqueado->proceso->pcb;
//	pthread_mutex_lock(&mutex_readys);
//	queue_push(colaListos, proceso);
//	pthread_mutex_unlock(&mutex_readys);
//	cambiarEstado(proceso, listo);
//	free(bloqueado);
//}

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
