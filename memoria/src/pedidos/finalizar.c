/*
 * finalizar.c
 *
 *  Created on: 7/11/2015
 *      Author: utnso
 */
#include "finalizar.h"

void finalizar(int pid, t_config * configuraciones) {

	if (tlb_habilitada(configuraciones)) {

		tlb_remover_pid(pid, configuraciones);

	} else {

		tabla_paginas_remover_pid(pid, configuraciones);

	}

}

