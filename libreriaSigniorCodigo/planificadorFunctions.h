/*
 * planificadorFunctions.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef PLANIFICADORFUNCTIONS_H_
#define PLANIFICADORFUNCTIONS_H_
#include <commons/log.h>
#include <commons/collections/queue.h>

enum estado {
	listo = 98, ejecucion, bloqueado, finalizado
};

typedef struct {
	char *puerto;
	char *algoritmo;
} t_config_planificador;

typedef struct {
	int id;
	char *dirProceso;
	int estado;
	int programCounter;
	char *nombrePrograma;
} tipo_pcb;

typedef struct {
	tipo_pcb * proceso;
	int socket;
} nodo_en_ejecucion;

typedef struct {
	tipo_pcb * proceso;
	int espera;
} nodo_entrada_salida;

void inicializarMutex(pthread_mutex_t mutex_readys,
		pthread_mutex_t mutex_ejecucion, pthread_mutex_t mutex_bloqueados);

void destruirMutex(pthread_mutex_t mutex_readys,
		pthread_mutex_t mutex_ejecucion, pthread_mutex_t mutex_bloqueados);

int reconocerIdentificador();

int generarPID(int* pid);

tipo_pcb *generarPCB(int pid, char *path, int estado, char *nombre);

char *getPuerto();

char *getAlgoritmo();

void inicializarColecciones(t_queue * colaListos, t_queue * colaFinalizados,
		t_list * listaEjecutando, t_queue * entradaSalida);

void mostrarEstadoDeLista(t_list *lista, char*estado);

void mostrarEstadoDeListos(t_queue *cola, char*estado);

void mostrarEstadoDeBloqueados(t_queue *cola, char*estado) ;

void agregarAListaDeEjecucion(pthread_mutex_t mutex_ejecucion,
		t_list *listaEjecutando, nodo_en_ejecucion *proceso, t_queue*colaListos,
		t_queue*entrada_salida, t_log*log_planificador) ;

tipo_pcb * removerDeColaDeListos(pthread_mutex_t mutex_readys,
		t_queue*colaListos);

void agregarEnColaDeListos(tipo_pcb *proceso, pthread_mutex_t mutex_readys,
		t_queue *colaListos, t_log*log_planificador, t_queue*entrada_salida,
		t_list*en_ejecucion);

void cambiarAEstadoDeEjecucion(pthread_mutex_t mutex_readys, t_queue*colaListos,
		pthread_mutex_t mutex_ejecucion, t_list *listaEjecutando,
		t_queue*entrada_salida, t_log*log_planificador,int socketEnEjecucion);

nodo_en_ejecucion * removerDeListaDeEjecucion(tipo_pcb *pcb, pthread_mutex_t mutex_ejecucion,
		t_list*listaEjecutando) ;

void agregarAColaDeBloqueados(pthread_mutex_t mutex_bloqueados,
		t_queue *entradaSalida, nodo_entrada_salida*io, t_queue*colaListos,
		t_list*listaEjecutando,t_log*log_planificador) ;

void cambiarEstadoABloqueado(tipo_pcb *pcb, pthread_mutex_t mutex_bloqueados,
		t_queue*entradaSalida, pthread_mutex_t mutex_ejecucion,
		t_list*listaEjecutando, pthread_mutex_t mutex_readys,
		t_queue*colaListos, t_log*log_planificador);

void cambiarEstado(tipo_pcb *proceso, int estado, t_queue*colaListos,
		t_queue*entrada_salida, t_list*en_ejecucion);

int setProgramCounter(char *dirProceso);

#endif /* PLANIFICADORFUNCTIONS_H_ */
