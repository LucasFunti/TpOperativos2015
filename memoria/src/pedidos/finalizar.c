/*
 * finalizar.c
 *
 *  Created on: 7/11/2015
 *      Author: utnso
 */
#include "finalizar.h"

void finalizar(int pid) {

	loggearInfo(
			string_from_format(
					"Se finaliza el proceso %d. El mismo tuvo %d aciertos sobre %d pedidos a la tlb",
					pid, registroTlb[pid].aciertos, registroTlb[pid].pedidos));

	if (tlb_habilitada()) {

		tlb_remover_pid(pid);

	} else {

		tabla_paginas_remover_pid(pid);

	}

}
