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

extern t_queue * colaListos;
extern t_queue * colaFinalizados;
extern t_queue * cola_cpu_libres;
extern t_list * listaEjecutando;
extern t_queue * entradaSalida;
extern pthread_mutex_t mutex_readys;
extern pthread_mutex_t mutex_ejecucion;
extern pthread_mutex_t mutex_bloqueados;
extern t_log *log_planificador;

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
	tipo_pcb pcb;
	int peso;
	int socket_prog;
} nodo_proceso;

typedef struct {
	int socket;
	int pid;
} tipo_cpu;

typedef struct {
	nodo_proceso proceso;
	tipo_cpu cpu;
} nodo_en_ejecucion;

typedef struct {
	nodo_proceso* proceso;
	int espera;
} nodo_entrada_salida;

int reconocerIdentificador();

int generarPID(int* pid);
<<<<<<< HEAD
tipo_pcb *generarPCB(int pid, char *path, int estado, char *nombre);

char *getPuerto();

char *getAlgoritmo();

void inicializarColecciones();

void mostrarEstadoDeLista(t_list *lista, char*estado);

void mostrarEstadoDeCola(t_queue *cola, char*estado);

void agregarEnColaDeListos(nodo_proceso *proceso) ;

void cambiarAEstadoDeEjecucion() ;

void agregarEnColaDeBloqueados(tipo_pcb pcb, int espera) ;

void agregarDeBloqueadoAListo(t_queue *entrada_salida, t_queue *readys,
		t_log *log_planificador);

void cambiarEstado(nodo_proceso *proceso, int estado);

void finalizarProceso(int pid);

int setProgramCounter(char *dirProceso);
=======
tipo_pcb generarPCB(int pid, char *path, int estado,char *nombre);
t_config_planificador read_config_planificador();
void inicializarColecciones(t_list *listaNuevos, t_queue *colaListos,
		t_queue *colaFinalizados, t_queue*cola_cpu_libres,
		t_list *listaEjecutando, t_list *entradaSalida);
void mostrarEstadoDeLista(t_list *lista,char*estado);
void mostrarEstadoDeCola(t_queue *cola,char*estado);
>>>>>>> b49e770e571f7b349561922e05d0e270911eed9b

#endif /* PLANIFICADORFUNCTIONS_H_ */
