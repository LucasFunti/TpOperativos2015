/*
 * planificadorFunctions.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef PLANIFICADORFUNCTIONS_H_
#define PLANIFICADORFUNCTIONS_H_

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
tipo_pcb generarPCB(int pid, char *path, int estado,char *nombre);
t_config_planificador read_config_planificador();
void inicializarColecciones(t_list *listaNuevos, t_queue *colaListos,
		t_queue *colaFinalizados, t_queue*cola_cpu_libres,
		t_list *listaEjecutando, t_list *entradaSalida);
void mostrarEstadoDeLista(t_list *lista,char*estado);
void mostrarEstadoDeCola(t_queue *cola,char*estado);

#endif /* PLANIFICADORFUNCTIONS_H_ */
