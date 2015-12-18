/*
 * swap.c
 *
 *  Created on: 5/11/2015
 *      Author: utnso
 */

#include "swap.h"

char * swap_leer(int pid, int pagina) {

	log_info(logger,
			string_from_format(
					"Se intenta leer el contenido del proceso %d pagina %d al swap",
					pid, pagina));

	accesos_swap++;

	if (!test) {

		void * data = malloc(2 * sizeof(int));

		memcpy(data, &pid, sizeof(int));
		memcpy(data + sizeof(int), &pagina, sizeof(int));

		t_data * paquete = pedirPaquete(LEER, 2 * sizeof(int), data);

		common_send(socketSwap, paquete);

		paquete = leer_paquete(socketSwap);

		log_info(logger,
				string_from_format("El contenido es %s", paquete->data));

		return (char *) paquete->data;
	} else {
		char * texto = malloc(2);
		texto = "T";
		return texto;
	}
}

bool swap_iniciar(int pid, int cantidad_paginas) {

	log_info(logger, "Se le pide iniciar al swap");

	int cantidad = cantidad_paginas;

	void * buffer = malloc(sizeof(int) * 2);

	memcpy(buffer, &pid, sizeof(int));
	memcpy(buffer + sizeof(int), &cantidad, sizeof(int));

	t_data * paquete = pedirPaquete(INICIAR, 2 * sizeof(int), buffer);

	common_send(socketSwap, paquete);

	paquete = leer_paquete(socketSwap);

	return (paquete->header->codigo_operacion == ALOCADO);

}

void swap_escribir(int pid, int pagina, int marco) {

	log_info(logger,
			string_from_format(
					"Se envia al swap el contenido de la página %d del proceso %d que estaba en el marco %d",
					pagina, pid, marco));

	if (!test) {

		int tamanio_marco = config_get_int_value(memoriaConfig,
				"TAMANIO_MARCO");

		void * data = malloc(2 * sizeof(int) + tamanio_marco);

		memcpy(data, &pid, sizeof(int));
		memcpy(data + sizeof(int), &pagina, sizeof(int));

		retardo();
		memcpy(data + 2 * sizeof(int), memoria + tamanio_marco * marco,
				tamanio_marco);

		common_send(socketSwap,
				pedirPaquete(ESCRIBIR, 2 * sizeof(int) + tamanio_marco, data));

	}
	accesos_swap++;
}

void swap_finalizar(int pid) {

	t_data * paquete = pedirPaquete(FINALIZAR, sizeof(int), &pid);
	common_send(socketSwap, paquete);
	free(paquete);

}

