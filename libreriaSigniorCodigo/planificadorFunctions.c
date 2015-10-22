/*
 * planificadorFunctions.c
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commons/config.h>
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
/* Leer el archivo de config y guardarla en una estructura */
t_config_planificador read_config_planificador() {
	t_config_planificador unConfig;
	t_config *archivoConfiguracion;
	archivoConfiguracion =
			config_create(
					"/home/utnso/git/tp-2015-2c-signiorcodigo/planificador/planificadorConfig");
	unConfig.puerto = config_get_string_value(archivoConfiguracion,
			"PUERTO_ESCUCHA");
	unConfig.algoritmo = config_get_string_value(archivoConfiguracion,
			"ALGORITMO_PLANIFICACION");

	return unConfig;
}

/* inicializar las estructuras para el planificador */
void inicializarColecciones(t_list * listaNuevos, t_queue * colaListos,
		t_queue * colaFinalizados, t_queue * cola_cpu_libres,
		t_list * listaEjecutando,t_list * entradaSalida)
{
	listaNuevos = list_create();
	colaListos = queue_create();
	colaFinalizados = queue_create();
	cola_cpu_libres = queue_create();
	listaEjecutando = list_create();
	entradaSalida = list_create();
}
