/*
 * swap.c
 *
 *  Created on: 5/11/2015
 *      Author: utnso
 */

#include "swap.h"

char * swap_leer(int pid, int pagina) {

	void * data = malloc(2 * sizeof(int));

	memcpy(data, &pid, sizeof(int));
	memcpy(data + sizeof(int), &pagina, sizeof(int));

	t_data * paquete = pedirPaquete(LEER, 2 * sizeof(int), data);

	common_send(socketSwap, paquete);

	paquete = leer_paquete(socketSwap);

	return (char *) paquete->data;

}

bool swap_iniciar(int pid, int cantidad_paginas) {

	int cantidad = cantidad_paginas;

	t_data * paquete = pedirPaquete(INICIAR, sizeof(int), &cantidad);
	common_send(socketSwap, paquete);

	paquete = leer_paquete(socketSwap);

	return (paquete->header->codigo_operacion == ALOCADO);

}

void swap_escribir(int pid, int pagina, int marco) {

	int tamanio_marco = config_get_int_value(memoriaConfig, "TAMANIO_MARCO");

	void * data = malloc(3 * sizeof(int) + tamanio_marco);

	memcpy(data, &pid, sizeof(int));
	memcpy(data + sizeof(int), &pagina, sizeof(int));
	memcpy(data + 2 * sizeof(int), &tamanio_marco, sizeof(int));
	memcpy(data + 3 * sizeof(int), memoria + tamanio_marco * marco,
			tamanio_marco);

	common_send(socketSwap,
			pedirPaquete(ESCRIBIR, 3 * sizeof(int) + tamanio_marco, data));

}

void swap_finalizar(int pid) {

	t_data * paquete = pedirPaquete(FINALIZAR, sizeof(int), &pid);
	common_send(socketSwap, paquete);
	free(paquete);

}

