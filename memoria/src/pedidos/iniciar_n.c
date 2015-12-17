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

	if (swap_puede) {

		loggearInfo("El swap tiene lugar para iniciarlo");

		crear_estructura_para_proceso(pid, cantidad_paginas);

		loggearInfo("Se añadieron las entradas a la tabla de páginas");

		return true;

	} else {

		loggearWarning("El swap no tenía lugar para iniciarlo");

		return false;

	}

}

void crear_estructura_para_proceso(int pid, int cantidad_paginas) {

	int i;
	for (i = 0; i < cantidad_paginas; i++) {

		//Un número de marco cualquiera, ya que no están presentes y necesito algo :p
		tabla_paginas_aniadir_item(pid, i, 0);
	}

}

//Devuelve el numero de marco para asignar o -1 si no había disponibles
int marco_libre(int pid) {

	log_info(logger, "Se busca un marco libre para asignar");

	pid_matchear = pid;

	int paginaNuevoProceso = pagina_matchear;

	if (marcos_libres() == 0) {
		//Si no hay marcos disponibles
		log_info(logger, "No hay marcos libres para asignar");

		if (tiene_marcos_asignados(pid)) {

			log_info(logger,
					"El proceso ya tiene marcos asignados, se swappea alguno existente");

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

			loggearInfo("Se remplaza la victima con el nuevo elemento");

			return marco;

		} else {

			//No le puedo dar ninguno, se aborta
			return -1;
		}

	} else {

		//Hay marcos libres

		int cantidad_maxima_marcos_proceso = config_get_int_value(memoriaConfig,
				"MAXIMO_MARCOS_POR_PROCESO");

		if (marcos_asignados(pid) < cantidad_maxima_marcos_proceso) {

			//Le doy un marco nuevo

			int marco = marco_disponible();

			if (es_clock_modificado()) {

				//Si es clockModificado me fijo si no tiene presentes,
				//significa que este que tengo acá en pid_matchear y pagina_matchear
				//es el único elemento presente y pasa a ser puntero

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

			loggearInfo("Se remplaza la victima con el nuevo elemento");

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

void compuA() {
	//Cliente
	//localhost:5000

	//Pedir socket pero para pedir un socket necesitas parametros de la dirección
	// Entonces primero cargás los datos de la dirección con el tipo de socket y parecidos
	//conseguis la dirección

	//Los parámetros de la función socket los sacás de la estructura de la dirección que conseguiste
	// int socketCompuB = socket(infoDireccion->sarlanga,infoDireccion->sarlanga2,infoDireccion->sarlanga3)
	int socketCompuB;
	int mensaje = 33;

	send(socketCompuB, &mensaje, sizeof(int), MSG_WAITALL);

}

void compuB() {

	//localhost:5000

	//Cargar la info de la direccion

	int socketCompuA = 3;
	int mensajeRecibido;

	recv(socketCompuA, &mensajeRecibido, sizeof(int), MSG_WAITALL);

}

