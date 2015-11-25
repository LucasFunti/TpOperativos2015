/*
 * finalizar.c
 *
 *  Created on: 7/11/2015
 *      Author: utnso
 */
#include "finalizar.h"

void finalizar(int pid, bool es_test) {

	if (elemento_del_puntero_clock_m != NULL
			&& pid == elemento_del_puntero_clock_m->pid) {
		elemento_del_puntero_clock_m = NULL;
	}

	loggearInfo(
			string_from_format(
					"Se finaliza el proceso %d. El mismo tuvo %d aciertos sobre %d pedidos a la tlb",
			pid, registroTlb[pid].aciertos, registroTlb[pid].pedidos));

	if (tlb_habilitada(memoriaConfig)) {

		tlb_remover_pid(pid, memoriaConfig, es_test);

	} else {

		tabla_paginas_remover_pid(pid, memoriaConfig, es_test);

	}

}

