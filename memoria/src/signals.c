/*
 * signals.c
 *
 *  Created on: 4/11/2015
 *      Author: utnso
 */
#include "signals.h"

void registrarSeniales() {

	signal(SIGUSR1, atender_seniales);
	signal(SIGUSR2, atender_seniales);
	signal(SIGPOLL, atender_seniales);
	loggearInfo(
			string_from_format(
					"Ya se atiende la señal SIGUSR1 (%d), SIGUSR2 (%d) y SIGPOLL (%d) ",
					SIGUSR1, SIGUSR2, SIGPOLL));

}

void atender_seniales(int signal) {

	switch (signal) {

	case SIGUSR1:

		pthread_mutex_lock(&semaforo_memoria);
		if (!test) {

			loggearInfo("Se registró la señal para vaciar la TLB");
		}
		vaciar_tlb();
		pthread_mutex_unlock(&semaforo_memoria);

		break;

	case SIGUSR2:
		pthread_mutex_lock(&semaforo_memoria);
		if (!test) {
			loggearInfo("Se registró la señal para vaciar la tabla de páginas");
		}
		vaciar_tabla_paginas();
		pthread_mutex_unlock(&semaforo_memoria);

		break;

	case SIGPOLL:

		pthread_mutex_lock(&semaforo_memoria);
		if (!test) {
			loggearInfo(
					"Se registró la señal para volcar la memoria principal");
		}
		volcar_memoria_principal();
		pthread_mutex_unlock(&semaforo_memoria);

		break;

	}

}

void vaciar_tlb() {

	loggearInfo("Comienza el vaciado de la tlb");
	int cantidad_elementos = list_size(tlb);
	t_item * item;

	while (cantidad_elementos) {

		item = list_remove(tlb, 0);

		cantidad_elementos--;
	}

}

void vaciar_tabla_paginas() {

	loggearInfo("Comienza el vaciado de la tabla_paginas");
	int cantidad_elementos = list_size(tabla_paginas);
	t_item * item;

	while (cantidad_elementos) {

		retardo();
		item = list_remove(tabla_paginas, 0);
		if (item->modificado) {
			swap_escribir(item->pid, item->pagina, item->marco);

		}
		cantidad_elementos--;

	}
}

void volcar_memoria_principal() {

	loggearInfo("Comienza el volcado");

	int tamanio_marco = config_get_int_value(memoriaConfig, "TAMANIO_MARCO");
	int cantidad_marcos = config_get_int_value(memoriaConfig,
			"CANTIDAD_MARCOS");

	loggearInfo("INICIO DEL CONTENIDO");

	int i;
	for (i = 0; i < cantidad_marcos; i++) {

		retardo();

		char * string = malloc(tamanio_marco);
		memcpy(string, memoria + tamanio_marco * i, tamanio_marco);

		loggearInfo(
				string_from_format("En el marco %d se encuentra: %s", i,
						string));

		free(string);
	}

	loggearInfo("FIN DEL CONTENIDO");

}
