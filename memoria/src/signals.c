/*
 * signals.c
 *
 * Created on: 4/11/2015
 * Author: utnso
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

	pthread_t hilo;

	switch (signal) {

	case SIGUSR1:

		if (!test) {
			loggearInfo("Se registró la señal para vaciar la TLB");
		}

		pthread_create(&hilo, NULL, vaciar_tlb, NULL);

		break;

	case SIGUSR2:

		if (!test) {
			loggearInfo(
					"Se registró la señal para vaciar la memoria principal");
		}

		pthread_create(&hilo, NULL, vaciar_memoria_principal, NULL);

		break;

	case SIGPOLL:

		if (!test) {
			loggearInfo("Se registró la señal para volcar la tabla de páginas");
		}

		pthread_create(&hilo, NULL, volcar_memoria_principal, NULL);

		break;

	}

}

void * vaciar_tlb(void * data) {
	pthread_mutex_lock(&semaforo_memoria);
	loggearInfo("Comienza el vaciado de la tlb");
	int cantidad_elementos = list_size(tlb);

	list_clean(tlb);

	loggearInfo("Se termina el vaciado de la tlb");
	pthread_mutex_unlock(&semaforo_memoria);
	return NULL;
}

void * vaciar_memoria_principal(void * data) {
	pthread_mutex_lock(&semaforo_memoria);
	loggearInfo("Comienza el vaciado de la memoria principal");
	int cantidad_elementos = list_size(tabla_paginas);

	void iterador_para_vaciar(void * data) {

		retardo();
		loggearInfo("Se vaciando un item");

		t_item * item = data;
		item->presencia = false;
		if (item->modificado) {
			loggearInfo("Estaba modificado");
			swap_escribir(item->pid, item->pagina, item->marco);
			item->modificado = false;
		} else {
			loggearInfo("No estaba modificado");
		}
		item->numero_operacion = get_numero_operacion();

	}

	list_iterate(tabla_paginas, iterador_para_vaciar);
	pthread_mutex_unlock(&semaforo_memoria);
	return NULL;
}

void * volcar_memoria_principal(void * data) {
	pthread_mutex_lock(&semaforo_memoria);
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
				string_from_format("En el marco %d se encuentra %s", i,
						string));

		free(string);
	}

	loggearInfo("FIN DEL CONTENIDO");
	pthread_mutex_unlock(&semaforo_memoria);
	return NULL;
}
