/*
 * escribir_n.c
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */
#include "escribir_n.h"

void escribir_n(int pid, int pagina, char * contenido) {

	marco marco = tlb_buscar(pid, pagina, memoriaConfig, true);

	int tamanio_marco = config_get_int_value(memoriaConfig, "TAMANIO_MARCO");

	char * contenido_truncado = malloc(tamanio_marco);

	memcpy(contenido_truncado, contenido, tamanio_marco);

	//Escribe en memoria principal, entonces es con retardo
	retardo(memoriaConfig);
	memcpy(memoria + tamanio_marco * marco, contenido_truncado, tamanio_marco);

}

