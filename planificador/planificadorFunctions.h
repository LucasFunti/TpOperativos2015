/*
 * planificadorFunctions.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef PLANIFICADORFUNCTIONS_H_
#define PLANIFICADORFUNCTIONS_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <signiorCodigo/libSockets.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include "planificador.h"

enum estado {
	listo = 98,
	ejecucion = 10,
	bloqueado = 11,
	finalizado = 23,
	finquantum = 22,
	fallaEjecucion = 21,
	entrada_salida = 20
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
	time_t tiempoComienzo;
	time_t tiempoFinalizacion;
	time_t tiempoIngreso;
	double tiempoEnReadys;
	time_t tiempoRespuesta;
} tipo_pcb;

typedef struct {
	tipo_pcb * proceso;
	int socket;
	int pid_cpu;
	int instrucciones_ejecutadas;
} nodo_en_ejecucion;

typedef struct {
	tipo_pcb * proceso;
	int espera;
} nodo_entrada_salida;

typedef struct {
	tipo_pcb *pcb;
	int tiempo;
} data_hilo;

typedef struct {
	int rafagaEjecutada;
	int pid_cpu;
	int PC;
	bool resultado_rafaga;
} rafaga_t;

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

void mostrarEstadoDeBloqueados(t_queue *cola, char*estado);

void agregarAListaDeEjecucion(nodo_en_ejecucion *proceso);

tipo_pcb * removerDeColaDeListos();

void agregarEnColaDeListos(tipo_pcb *proceso);

void cambiarAEstadoDeEjecucion(int socketEnEjecucion, int pid_cpu);

nodo_en_ejecucion * removerDeListaDeEjecucion(tipo_pcb *pcb);

void agregarAColaDeBloqueados(nodo_entrada_salida*io);

void cambiarEstado(tipo_pcb *proceso, int estado);

int setProgramCounter(char *dirProceso);

void enviarContextoEjecucion(int socketCliente, int codigoOperacion,
		tipo_pcb *proceso, char *path, char *algoritmo, int quantum);

void interpretarInstruccion(int instruccion, int socketCliente);

data_hilo * obtenerDatosHilo(nodo_en_ejecucion *proceso, int tiempo);

void loguearRafaga(rafaga_t *otraInstruccion, nodo_en_ejecucion * unProceso);

void ejecutarlogueoInstruccionesEjecutadas(void * data,int cantidadResultados,
		nodo_en_ejecucion * proceso);

void agregarAFinalizados(t_queue *finalizados, nodo_en_ejecucion * proceso,
		t_log *log_planificador);

void peticionPorcentajeUsoCpu(int codigo);

void * ejecutarIngresoConsola();

void liberarCpu(int socket);

void * despacharProcesosListos();

bool cpuEstaLibre(void * data);

nodo_entrada_salida * quitarDeColaBloqueados() ;

void cambiarEstadoABloqueado(void* data)  ;

void *manejarEntradaSalida() ;

#endif /* PLANIFICADORFUNCTIONS_H_ */
