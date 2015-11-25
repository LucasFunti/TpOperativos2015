/*
 * finalizar.c
 *
 *  Created on: 7/11/2015
 *      Author: utnso
 */
#include "finalizar.h"

void finalizar(int pid, bool es_test) {

	if (tlb_habilitada(memoriaConfig)) {

		tlb_remover_pid(pid, memoriaConfig, es_test);

	} else {

		tabla_paginas_remover_pid(pid, memoriaConfig, es_test);

	}

}

