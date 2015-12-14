/*
 * algoritmos.c
 *
 *  Created on: 11/11/2015
 *      Author: utnso
 */
#include "algoritmos.h"

int fifo() {

	t_list * marcos_proceso = list_filter(cola_llegada,
			coincide_pid_y_esta_presente);

	t_item * item = list_remove(marcos_proceso, 0);

	pagina_matchear = item->pagina;

	list_remove_by_condition(cola_llegada, coincide_pid_y_pagina);

	if (item->modificado) {
		if (!test) {
			swap_escribir(pid_matchear, pagina_matchear, item->marco);
		}
		item->modificado = false;
	}

	item->presencia = false;

	return item->marco;

}

int lru() {

	t_list * marcos_proceso = list_filter(cola_llegada,
			coincide_pid_y_esta_presente);

	list_sort(marcos_proceso, comparador_primero_menor_n_operacion);

	t_item * item = list_get(marcos_proceso, 0);

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

	t_list * marcos_proceso = list_filter(cola_llegada,
			coincide_pid_y_esta_presente);

	int maximo_indice_tabla_paginas = list_size(tabla_paginas) - 1;

//	if (elemento_del_puntero_clock_m == NULL) {
//
//		//Se inicia por primera vez
//
//		contador_auxiliar_indice_clock_m = -1;
//
//		//Busco alguno sin modificar
//
//		t_tabla_paginas_item * elemento_para_swappear = list_find(tabla_paginas,
//				no_esta_modificado_y_esta_presente);
//
//		if (elemento_para_swappear == NULL) {
//
//			//Están todos modificados,entonces agarro el primero
//
//			elemento_para_swappear = list_get(tabla_paginas, 0);
//
//			// El puntero va a ser el siguiente al que removí
//			elemento_del_puntero_clock_m = list_get(tabla_paginas, 1);
//
//			swap_escribir(elemento_para_swappear->pid,
//					elemento_para_swappear->pagina,
//					elemento_para_swappear->marco);
//
//			return 0; //Digo que remueva al que ya escribí
//
//		} else {
//
//			//El elemento está en elemento_para_swappear y no esta modificado, entonces devuelvo
//			//su posicion
//
//			actualizar_puntero_clock(maximo_indice_tabla_paginas);
//
//			return contador_auxiliar_indice_clock_m;
//
//		}
//	} else {
//
//		//No es la primera vez
//
//		contador_auxiliar_indice_clock_m = -1;
//
//		//Busco a alguno que venga después o sea el del puntero y no esté modificado
//
//		t_tabla_paginas_item * elemento_para_swappear = list_find(tabla_paginas,
//				es_el_del_puntero_o_posterior_y_no_modificado);
//
//		if (elemento_para_swappear == NULL) {
//
//			//No hay ninguno sin modificar, tengo que swappear el del puntero
//
//			contador_auxiliar_indice_clock_m = -1;
//
//			elemento_para_swappear = list_find(tabla_paginas,
//					es_el_del_puntero);
//
//			swap_escribir(elemento_para_swappear->pid,
//					elemento_para_swappear->pagina,
//					elemento_para_swappear->marco);
//
//			actualizar_puntero_clock(maximo_indice_tabla_paginas);
//
//			return contador_auxiliar_indice_clock_m;
//
//		} else {
//
//			//Swappeo el que encontró, que no está modificado
//
//			actualizar_puntero_clock(maximo_indice_tabla_paginas);
//
//			return contador_auxiliar_indice_clock_m;
//
//		}
//
//	}

	return -1;

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
