/*
 * escribir_n.c
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */
#include "escribir_n.h"

void escribir_n(int pid, int pagina, char * contenido,
		t_config * configuraciones) {

	marco marco = tlb_buscar(pid, pagina, configuraciones, true);

	int tamanio_marco = config_get_int_value(configuraciones, "TAMANIO_MARCO");

	char * contenido_truncado = malloc(tamanio_marco);

	memcpy(contenido_truncado, contenido, tamanio_marco);

	//Escribe en memoria principal, entonces es con retardo
	retardo(configuraciones);
	memcpy(memoria + tamanio_marco * marco, contenido_truncado, tamanio_marco);

}

