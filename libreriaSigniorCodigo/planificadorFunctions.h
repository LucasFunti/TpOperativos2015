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
	int id;
	char *dirProceso;
	int estado;
	int programCounter;
} tipo_pcb;

typedef struct {
	char *puerto;
	char *algoritmo;
} t_config_planificador;

int reconocerIdentificador();
int generarPID(int* pid);
tipo_pcb generarPCB(int pid, char *path, int estado);
t_config_planificador read_config_planificador();

#endif /* PLANIFICADORFUNCTIONS_H_ */
