/*
 * iniciar_n.c
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */
#include "iniciar_n.h"

bool iniciar_n(int pid, int cantidad_paginas) {

	bool swap_puede;

	if (test) {
		swap_puede = true;
	} else {
		swap_puede = swap_iniciar(pid, cantidad_paginas);
	}

	int cantidad_maxima_marcos_proceso = config_get_int_value(memoriaConfig,
			"MAXIMO_MARCOS_POR_PROCESO");

	if (swap_puede) {

		crear_estructura_para_proceso(pid, cantidad_paginas);

		return true;

	} else {

		return false;

	}

}

void crear_estructura_para_proceso(int pid, int cantidad_paginas) {

	int i;
	for (i = 0; i < cantidad_paginas; i++) {
		//False porque recién se crean y no tienen datos, entonces no están modificados y están en memoria
		tabla_paginas_aniadir_item(pid, i, 0);
	}

}

int marco_libre(int pid) {

	pid_matchear = pid;

	if (marcos_libres() == 0) {

		if (tiene_marcos_asignados(pid)) {

			//Swappear uno ya existente

			char * algoritmo = config_get_string_value(memoriaConfig,
					"ALGORITMO_REMPLAZO");

			if (es_fifo(algoritmo)) {
				return fifo();
			}

			if (es_lru(algoritmo)) {
				return lru();
			}

			return clock_m();

		} else {
			return -1;
		}

	} else {
		int cantidad_maxima_marcos_proceso = config_get_int_value(memoriaConfig,
				"MAXIMO_MARCOS_POR_PROCESO");

		if (marcos_asignados(pid) < cantidad_maxima_marcos_proceso) {

			//Le doy un marco nuevo

			return marco_disponible();

		} else {

			char * algoritmo = config_get_string_value(memoriaConfig,
					"ALGORITMO_REMPLAZO");

			if (es_fifo(algoritmo)) {
				return fifo();
			}

			if (es_lru(algoritmo)) {
				return lru();
			}

			return clock_m();

		}

	}
}

bool es_fifo(char * algoritmo) {
	return !strcmp(algoritmo, "FIFO");
}

bool es_lru(char * algoritmo) {
	return !strcmp(algoritmo, "LRU");
}

bool es_clock_modificado(char * algoritmo) {
	return !strcmp(algoritmo, "CLOCK-M");
}

