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

	int tamanio_string = strlen(contenido)+1;

	char * contenido_a_escribir = malloc(tamanio_marco);

	if(tamanio_marco > tamanio_string){

		int cantidad_completar = tamanio_marco - tamanio_string;

		memcpy(contenido_a_escribir, contenido, tamanio_string);

		memcpy(contenido_a_escribir + tamanio_string, string_repeat('\0', cantidad_completar), cantidad_completar);


	}else{

		memcpy(contenido_a_escribir, contenido, tamanio_marco-1);
		contenido_a_escribir[14]='\0';

	}



	//Escribe en memoria principal, entonces es con retardo
	retardo(memoriaConfig);
	memcpy(memoria + tamanio_marco * marco, contenido_a_escribir, tamanio_marco);

}

