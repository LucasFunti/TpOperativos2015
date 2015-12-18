/*
 * algoritmos.c
 *
 *  Created on: 11/11/2015
 *      Author: utnso
 */
#include "algoritmos.h"

int fifo() {

	log_info(logger, "Se utiliza el algoritmo fifo");

	t_list * marcos_proceso = list_filter(cola_llegada,
			coincide_pid_y_esta_presente);

	t_item * item = list_remove(marcos_proceso, 0);

	log_info(logger,
			string_from_format(
					"Se va a remplazar la página %d del proceso %d que ocupa el marco %d",
					item->pid, item->pagina, item->marco));

	pagina_matchear = item->pagina;

	list_remove_by_condition(cola_llegada, coincide_pid_y_pagina);

	if (item->modificado) {

		swap_escribir(pid_matchear, pagina_matchear, item->marco);

		item->modificado = false;

	}

	item->presencia = false;
	item->numero_operacion = get_numero_operacion();

	return item->marco;

}

int lru() {

	log_info(logger, "Se utiliza el algoritmo lru");

	t_list * marcos_proceso = list_filter(cola_llegada,
			coincide_pid_y_esta_presente);

	list_sort(marcos_proceso, comparador_primero_menor_n_operacion);

	t_item * item = list_get(marcos_proceso, 0);

	log_info(logger,
			string_from_format(
					"La victima es la pagina %d con el marco %d con el numero de operacion %d",
					item->pagina, item->marco, item->numero_operacion));

	pagina_matchear = item->pagina;

	if (item->modificado) {
		swap_escribir(pid_matchear, pagina_matchear, item->marco);
		item->modificado = false;
	}

	item->presencia = false;
	item->numero_operacion = get_numero_operacion();

	return item->marco;

}

int get_numero_operacion() {
	int n = numero_operacion;
	numero_operacion++;
	return n;
}

int clock_m() {

	log_info(logger, "Se utiliza el algoritmo clock_modificado");

	ignorar_proximoAgregar = true;

	t_list * marcos_proceso = list_filter(cola_llegada, coincide_pid);

	int posicion = -1, posicionPuntero;

	void conseguirPosicionPuntero(void * data) {

		t_item * item = data;
		posicion++;
		if (item->puntero) {
			posicionPuntero = posicion;
			item->puntero = false;
		}
	}

	// Tengo la posición del puntero
	list_iterate(marcos_proceso, conseguirPosicionPuntero);

	t_list * listaAuxiliar = list_create();

	//Muevo elementos que vienen después del puntero
	for (posicion = posicionPuntero; posicion < list_size(marcos_proceso);
			posicion++) {

		t_item * elementoACopiar = list_get(marcos_proceso, posicion);

		list_add(listaAuxiliar, elementoACopiar);

	}

	//Muevo elementos que vienen antes del puntero
	for (posicion = 0; posicion < posicionPuntero; posicion++) {

		t_item * elementoACopiar = list_get(marcos_proceso, posicion);

		list_add(listaAuxiliar, elementoACopiar);

	}

	t_item * puntero = list_get(listaAuxiliar, 0);

	loggearInfo(
			string_from_format("El puntero es la página %d con el marco %d",
					puntero->pagina, puntero->marco));

	//Ya tengo la lista auxiliar armada

	posicion = -1;
	bool buscar_uso_cero_modificado_cero(void * data) {

		posicion++;
		t_item * item = data;
		return item->modificado == false && item->uso == false;

	}

	t_item * victima;

	loggearInfo("Se busca victima con USO falso y MODIFICADO falso");

	comenzarDeNuevo: victima = list_find(listaAuxiliar,
			buscar_uso_cero_modificado_cero);

	if (victima != NULL) {

		loggearInfo(
				string_from_format("Se encontro victima, pagina %d, marco %d",
						victima->pagina, victima->marco));

		victima->presencia = false;

		t_item * proximoPuntero;

		if (posicion == list_size(listaAuxiliar) - 1) {

			proximoPuntero = list_get(listaAuxiliar, 0);

		} else {

			proximoPuntero = list_get(listaAuxiliar, posicion + 1);
		}

		proximoPuntero->puntero = true;

		pid_matchear = victima->pid;
		pagina_matchear = victima->pagina;

		posicion = -1;

		void conseguirPosicionVictima(void * data) {

			t_item * item = data;
			posicion++;
			if (item->pid == victima->pid && item->pagina == victima->pagina) {
				posicionVictima = posicion;

			}
		}

		list_iterate(cola_llegada, conseguirPosicionVictima);

		return victima->marco;
	}

	loggearInfo("No se encontró victima");

	posicion = -1;

	bool buscar_uso_cero_modificado_uno_cambiando_uso(void * data) {

		posicion++;
		t_item * item = data;

		if (item->modificado == true && item->uso == false) {
			return true;
		}
		item->uso = false;
		return false;
	}

	loggearInfo(
			"Se busca victima con USO falso y MODIFICADO true, cambiando USO a falso");

	victima = list_find(listaAuxiliar,
			buscar_uso_cero_modificado_uno_cambiando_uso);

	if (victima != NULL) {

		loggearInfo(
				string_from_format("Se encontro victima, pagina %d, marco %d",
						victima->pagina, victima->marco));

		swap_escribir(victima->pid, victima->pagina, victima->marco);

		victima->presencia = false;
		victima->modificado = false;

		t_item * proximoPuntero;

		if (posicion == list_size(listaAuxiliar) - 1) {

			proximoPuntero = list_get(listaAuxiliar, 0);

		} else {

			proximoPuntero = list_get(listaAuxiliar, posicion + 1);
		}

		proximoPuntero->puntero = true;

		posicion = -1;

		void conseguirPosicionVictima(void * data) {

			t_item * item = data;
			posicion++;
			if (item->pid == victima->pid && item->pagina == victima->pagina) {
				posicionVictima = posicion;

			}
		}

		list_iterate(cola_llegada, conseguirPosicionVictima);

		return victima->marco;

	}

	posicion = -1;

	goto comenzarDeNuevo;

}

//void actualizar_puntero_clock(int maximo_indice) {
//	if (contador_auxiliar_indice_clock_m + 1 > maximo_indice) {
//		//Significa que estaba al final, y el proximo es el primero
//		elemento_del_puntero_clock_m = list_get(tabla_paginas, 0);
//	} else {
//		//Puede ser el siguiente
//		elemento_del_puntero_clock_m = list_get(tabla_paginas,
//				contador_auxiliar_indice_clock_m + 1);
//	}
//}
//
//bool es_el_del_puntero(void * data) {
//	t_tabla_paginas_item * item = data;
//
//	contador_auxiliar_indice_clock_m++;
//
//	if (item == elemento_del_puntero_clock_m) {
//		return true;
//	}
//
//	return false;
//}
//
//bool es_el_del_puntero_o_posterior_y_no_modificado(void * data) {
//	t_tabla_paginas_item * item = data;
//
//	contador_auxiliar_indice_clock_m++;
//
//	if (item == elemento_del_puntero_clock_m) {
//		analizar = true;
//	}
//
//	if (analizar) {
//		return !item->modificado && item->presencia;
//	}
//	return false;
//}
//
//bool no_esta_modificado_y_esta_presente(void * data) {
//
//	t_tabla_paginas_item * item = data;
//	contador_auxiliar_indice_clock_m++;
//
//	return (!item->modificado && item->presencia);
//}
