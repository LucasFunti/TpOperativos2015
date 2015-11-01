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
void inicializarColecciones(t_queue * colaListos, t_queue * colaFinalizados,
		t_list * listaEjecutando, t_queue * entradaSalida) {
	colaListos = queue_create();
	colaFinalizados = queue_create();
	listaEjecutando = list_create();
	entradaSalida = queue_create();
}

/* recorro la lissta mostrando los estado de los procesos para la instruccion ps */
void mostrarEstadoDeLista(t_list *lista, char*estado) {
	int j;
	if (list_size(lista) == 0) {
		printf("La lista de planificacion ,%s, esta vacia\n", estado);

	} else {

		for (j = 0; j < list_size(lista); j++) {
			nodo_en_ejecucion *procesoEnEjecucion= list_get(lista, j);
			printf("mProc %d: %s -> %s \n", procesoEnEjecucion->proceso->id,
					procesoEnEjecucion->proceso->nombrePrograma, estado);
		}

	}
}

void mostrarEstadoDeCola(t_queue *cola, char*estado) {
	int j;
	if (queue_size(cola) == 0) {
		printf("La cola de planificacion ,%s, esta vacia\n", estado);
	} else {
		for (j = 0; j < queue_size(cola); j++) {
			tipo_pcb *proceso= list_get(cola->elements, j);
			printf("mProc %d: %s -> %s \n", proceso->id,
					proceso->nombrePrograma, estado);
		}
	}
}

/* cambiar estado del proceso */
void cambiarEstado(tipo_pcb *proceso, int estado, t_queue*colaListos,
		t_queue*entrada_salida, t_list*en_ejecucion) {
	proceso->estado = estado;
	mostrarEstadoDeCola(colaListos, "Listos");
	mostrarEstadoDeLista(en_ejecucion, "Ejecucion");
	mostrarEstadoDeCola(entrada_salida, "Bolqueados");
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
	pthread_mutex_unlock(&mutex_readys);
	cambiarEstado(proceso, listo, colaListos, entrada_salida, en_ejecucion);
}

tipo_pcb * removerDeColaDeListos(pthread_mutex_t mutex_readys,
		t_queue*colaListos) {
	tipo_pcb *proceso = malloc(sizeof(tipo_pcb));
	pthread_mutex_lock(&mutex_readys);
	proceso = queue_pop(colaListos);
	pthread_mutex_unlock(&mutex_readys);
	return proceso;
}
void agregarAListaDeEjecucion(pthread_mutex_t mutex_ejecucion,
		t_list *listaEjecutando, nodo_en_ejecucion *proceso,t_queue*colaListos,t_queue*entrada_salida) {
	pthread_mutex_lock(&mutex_ejecucion);
	list_add(listaEjecutando, proceso);
	cambiarEstado(proceso->proceso, ejecucion,colaListos,entrada_salida,listaEjecutando);
	pthread_mutex_unlock(&mutex_ejecucion);

}
/* saca un proceso de la cola de listos y lo coloca en la lista de ejecucion*/
void cambiarAEstadoDeEjecucion(pthread_mutex_t mutex_readys, t_queue*colaListos,
		pthread_mutex_t mutex_ejecucion, t_list *listaEjecutando,t_queue*entrada_salida) {
	tipo_pcb *proceso = malloc(sizeof(tipo_pcb));
	proceso = removerDeColaDeListos(mutex_readys, colaListos);
	nodo_en_ejecucion*procesoEjecutando=malloc(sizeof(nodo_en_ejecucion));
	procesoEjecutando->proceso=proceso;
	agregarAListaDeEjecucion(mutex_ejecucion, listaEjecutando, procesoEjecutando,colaListos,entrada_salida);
}

void removerDeListaDeEjecucion(tipo_pcb *pcb, pthread_mutex_t mutex_ejecucion,
		t_list*listaEjecutando) {
	bool encontrar_pid(void * nodo) {
		return ((((nodo_en_ejecucion*) nodo)->proceso->id) == pcb->id);
	}
	pthread_mutex_lock(&mutex_ejecucion);
	list_remove_by_condition(listaEjecutando, encontrar_pid);
	pthread_mutex_unlock(&mutex_ejecucion);

}
/* saca un proceso de la lista de ejecucion y lo coloca en la cola de entrada salida */
void agregarEnColaDeBloqueados(tipo_pcb *pcb, pthread_mutex_t mutex_bloqueados,
		t_queue*entradaSalida, pthread_mutex_t mutex_ejecucion,
		t_list*listaEjecutando, pthread_mutex_t mutex_readys,
		t_queue*colaListos, t_log*log_planificador,t_queue*entrada_salida) {
	removerDeListaDeEjecucion(pcb, mutex_ejecucion, listaEjecutando);
	nodo_entrada_salida * io = malloc(sizeof(nodo_entrada_salida));
	io->proceso = pcb;
	pthread_mutex_lock(&mutex_bloqueados);
	queue_push(entradaSalida, io);
	cambiarEstado(io->proceso, bloqueado,colaListos,entrada_salida,listaEjecutando);
	pthread_mutex_unlock(&mutex_bloqueados);
	//manejo de la espera de la entrada salida

	agregarEnColaDeListos(io->proceso, mutex_readys, colaListos,
			log_planificador,entrada_salida,listaEjecutando);

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
