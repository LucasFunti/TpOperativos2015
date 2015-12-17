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
	int paginaNuevoProceso = pagina_matchear;

	if (marcos_libres() == 0) {
		//Si no hay marcos disponibles

		if (tiene_marcos_asignados(pid)) {

			//Swappea uno que ya tiene

			if (es_fifo()) {
				return fifo();
			}

			if (es_lru()) {
				return lru();
			}

			int marco = clock_m();
			pagina_matchear = paginaNuevoProceso;

			t_item * itemParaMeter = list_find(tabla_paginas,
					coincide_pid_y_pagina);

			list_replace(cola_llegada, posicionVictima, itemParaMeter);

			return marco;

		} else {

			//No le puedo dar ninguno, se aborta

			return -1;
		}

	} else {

		int cantidad_maxima_marcos_proceso = config_get_int_value(memoriaConfig,
				"MAXIMO_MARCOS_POR_PROCESO");

		if (marcos_asignados(pid) < cantidad_maxima_marcos_proceso) {

			//Le doy un marco nuevo

			int marco = marco_disponible();

			if (es_clock_modificado()) {

				//Si es clockModificado me fijo si no tiene presentes,
				//significa que este que tengo acá en pid_matchear y pagina_matchear
				//es el único elemento y pasa a ser puntero

				t_item * elementoParaHacerPuntero;

				bool algunoPresente = list_any_satisfy(tabla_paginas,
						coincide_pid_y_esta_presente);

				if (!algunoPresente) {

					elementoParaHacerPuntero = list_find(tabla_paginas,
							coincide_pid_y_pagina);

					elementoParaHacerPuntero->puntero = true;

				}

			}

			return marco;

		} else {

			//Tiene el maximo y necesita swappear uno existente

			if (es_fifo()) {
				return fifo();
			}

			if (es_lru()) {
				return lru();
			}

			int marco = clock_m();
			pagina_matchear = paginaNuevoProceso;

			t_item * itemParaMeter = list_find(tabla_paginas,
					coincide_pid_y_pagina);

			itemParaMeter->uso = true;

			list_replace(cola_llegada, posicionVictima, itemParaMeter);

			return marco;

		}

	}
}

bool es_fifo() {
	char * algoritmo = config_get_string_value(memoriaConfig,
			"ALGORITMO_REEMPLAZO");
	return !strcmp(algoritmo, "FIFO");
}

bool es_lru() {
	char * algoritmo = config_get_string_value(memoriaConfig,
			"ALGORITMO_REEMPLAZO");
	return !strcmp(algoritmo, "LRU");
}

bool es_clock_modificado() {
	char * algoritmo = config_get_string_value(memoriaConfig,
			"ALGORITMO_REEMPLAZO");
	return !strcmp(algoritmo, "CLOCK-M");
}

