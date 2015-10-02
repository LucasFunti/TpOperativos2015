/*
 * planificadorFunctions.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef PLANIFICADORFUNCTIONS_H_
#define PLANIFICADORFUNCTIONS_H_

enum estado {
	listo = 98, ejecucion, bloqueado, finalizado
};

typedef struct {
	int id;
	char *dirProceso;
	int estado;
	int programCounter;
} tipo_pcb;

int reconocerIdentificador();
int generarPID(int* pid);
tipo_pcb generarPCB(int pid, char *path, int estado);

#endif /* PLANIFICADORFUNCTIONS_H_ */
