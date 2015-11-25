/*
 * algoritmos.c
 *
 *  Created on: 11/11/2015
 *      Author: utnso
 */
#include "algoritmos.h"

int fifo() {

	elemento_fifo_matchear = list_remove(cola_llegada, 0); //El primero
	posicion_auxiliar = -1;
	list_iterate(tabla_paginas, encontrar_elemento_fifo);
	return posicion_fifo;
}

void encontrar_elemento_fifo(void * data) {

	t_tabla_paginas_item * entrada = data;

	posicion_auxiliar++;

	if (entrada == elemento_fifo_matchear) {

		posicion_fifo = posicion_auxiliar;

	}

}

int lru() {

	//Así arranca en cero con el primero
	posicion_lru = -1;
	posicion_del_menor_lru = 0;
	//El más grande hasta el momento
	minimo_numero_operacion = numero_operacion;

	list_iterate(tabla_paginas, encontrar_menor_numero_operacion);

	return posicion_del_menor_lru;
}

void encontrar_menor_numero_operacion(void * data) {

	t_tabla_paginas_item * entrada = data;

	posicion_lru++;

	if (entrada->numero_operacion < minimo_numero_operacion) {

		minimo_numero_operacion = entrada->numero_operacion;
		posicion_del_menor_lru = posicion_lru;

	}

}

int clock_m() {

	int maximo_indice_tabla_paginas = list_size(tabla_paginas) - 1;

	if (elemento_del_puntero_clock_m == NULL) {

		//Se inicia por primera vez

		contador_auxiliar_indice_clock_m = -1;

		//Busco alguno sin modificar

		t_tabla_paginas_item * elemento_para_swappear = list_find(tabla_paginas,
				no_esta_modificado_y_esta_presente);

		if (elemento_para_swappear == NULL) {

			//Están todos modificados,entonces agarro el primero

			elemento_para_swappear = list_get(tabla_paginas, 0);

			// El puntero va a ser el siguiente al que removí
			elemento_del_puntero_clock_m = list_get(tabla_paginas, 1);

			swap_escribir(elemento_para_swappear->pid,
					elemento_para_swappear->pagina,
					elemento_para_swappear->marco);

			return 0; //Digo que remueva al que ya escribí

		} else {

			//El elemento está en elemento_para_swappear y no esta modificado, entonces devuelvo
			//su posicion

			actualizar_puntero_clock(maximo_indice_tabla_paginas);

			return contador_auxiliar_indice_clock_m;

		}
	} else {

		//No es la primera vez

		contador_auxiliar_indice_clock_m = -1;

		//Busco a alguno que venga después o sea el del puntero y no esté modificado

		t_tabla_paginas_item * elemento_para_swappear = list_find(tabla_paginas,
				es_el_del_puntero_o_posterior_y_no_modificado);

		if (elemento_para_swappear == NULL) {

			//No hay ninguno sin modificar, tengo que swappear el del puntero

			contador_auxiliar_indice_clock_m = -1;

			elemento_para_swappear = list_find(tabla_paginas,
					es_el_del_puntero);

			swap_escribir(elemento_para_swappear->pid,
					elemento_para_swappear->pagina,
					elemento_para_swappear->marco);

			actualizar_puntero_clock(maximo_indice_tabla_paginas);

			return contador_auxiliar_indice_clock_m;

		} else {

			//Swappeo el que encontró, que no está modificado

			actualizar_puntero_clock(maximo_indice_tabla_paginas);

			return contador_auxiliar_indice_clock_m;

		}

	}

}

void actualizar_puntero_clock(int maximo_indice) {
	if (contador_auxiliar_indice_clock_m + 1 > maximo_indice) {
		//Significa que estaba al final, y el proximo es el primero
		elemento_del_puntero_clock_m = list_get(tabla_paginas, 0);
	} else {
		//Puede ser el siguiente
		elemento_del_puntero_clock_m = list_get(tabla_paginas,
				contador_auxiliar_indice_clock_m + 1);
	}
}

bool es_el_del_puntero(void * data) {
	t_tabla_paginas_item * item = data;

	contador_auxiliar_indice_clock_m++;

	if (item == elemento_del_puntero_clock_m) {
		return true;
	}

	return false;
}

bool es_el_del_puntero_o_posterior_y_no_modificado(void * data) {
	t_tabla_paginas_item * item = data;

	contador_auxiliar_indice_clock_m++;

	if (item == elemento_del_puntero_clock_m) {
		analizar = true;
	}

	if (analizar) {
		return !item->modificado && item->presencia;
	}
	return false;
}

bool no_esta_modificado_y_esta_presente(void * data) {

	t_tabla_paginas_item * item = data;
	contador_auxiliar_indice_clock_m++;

	return (!item->modificado && item->presencia);
}
