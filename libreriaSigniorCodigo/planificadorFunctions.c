/*
 * planificadorFunctions.c
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "planificadorFunctions.h"

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
	} else
		codigoOperacion = 3;
	if (!strcmp(identificador, "exit\n")) {
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
tipo_pcb generarPCB(int pid, char *path, int estado) {
	tipo_pcb pcb;
	pcb.id = pid;
	pcb.dirProceso = path;
	pcb.estado = estado;
	pcb.programCounter = 0;

	return pcb;
}
