/*
 * algoritmos.c
 *
 *  Created on: 11/11/2015
 *      Author: utnso
 */
#include "algoritmos.h".

int posicion, posicion_del_menor;
int minimo_numero_operacion;

int fifo() {

	return 0;
}

int lru() {

	//Así arranca en cero con el primero
	posicion = -1;
	posicion_del_menor = 0;
	//El más grande hasta el momento
	minimo_numero_operacion = numero_operacion;

	list_iterate(tabla_paginas, encontrar_menor_numero_operacion);

	return posicion_del_menor;
}

void encontrar_menor_numero_operacion(void * data) {

	t_tabla_paginas_item * entrada = data;

	posicion++;

	if (entrada->numero_operacion < minimo_numero_operacion) {

		minimo_numero_operacion = entrada->numero_operacion;
		posicion_del_menor = posicion;

	}

}

int clock_m() {

	return 2;
}
