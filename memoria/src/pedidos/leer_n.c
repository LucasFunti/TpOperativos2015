/*
 * leer_n.c
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */
#include "leer_n.h"

char * leer_n(int pid, int pagina, t_config * configuraciones) {

	int tamanio_marco = config_get_int_value(configuraciones, "TAMANIO_MARCO");

	char * contenido = malloc(tamanio_marco);

	marco marco_encontrado = tlb_buscar(pid, pagina, configuraciones, false);

	//Lee memoria principal, va con retardo
	retardo(configuraciones);

	memcpy(contenido, memoria + marco_encontrado * tamanio_marco,
			tamanio_marco);

	return contenido;

}

