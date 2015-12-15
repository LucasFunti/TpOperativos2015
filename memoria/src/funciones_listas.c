/*
 * funciones_listas.c
 *
 *  Created on: 14/12/2015
 *      Author: utnso
 */
#include "funciones_listas.h"

bool coincide_pid(void * data) {

	t_item * item = data;

	return (item->pid == pid_matchear);

}

bool coincide_pid_y_pagina(void * data) {

	t_item * item = data;
	return (item->pagina == pagina_matchear && coincide_pid(data));
}

bool coincide_pid_y_libera_marcos(void * data) {

	t_item * item = data;

	if (coincide_pid(data)) {

		t_marco * marco = list_get(marcos_disponibles, item->marco);
		marco->disponible = true;

	}

	return coincide_pid(data);

}

bool coincide_pid_y_esta_presente(void * data) {

	t_item * item = data;

	return (coincide_pid(data) && item->presencia);

}

bool comparador_primero_menor_n_operacion(void * data1, void* data2) {

	t_item * item1 = data1;
	t_item * item2 = data2;

	return item1->numero_operacion < item2->numero_operacion;

}
