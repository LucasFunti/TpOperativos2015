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
	//Falta la tercer señal
	loggearInfo(
			string_from_format(
					"Ya se atiende la señal SIGUSR1 (%d), SIGUSR2 (%d) y **** ",
					SIGUSR1, SIGUSR2));

}

void atender_seniales(int signal) {

	switch (signal) {

	case SIGUSR1:

		pthread_mutex_lock(&semaforo_memoria);
		loggearInfo("Se registró la señal para vaciar la TLB");
		vaciar_tlb(memoriaConfig);
		pthread_mutex_unlock(&semaforo_memoria);

		break;

	case SIGUSR2:
		pthread_mutex_lock(&semaforo_memoria);
		loggearInfo("Se registró la señal para vaciar la tabla de páginas");
		vaciar_tabla_paginas(memoriaConfig);
		pthread_mutex_unlock(&semaforo_memoria);

		break;

	case 99: //Preguntar Gastón D:

		pthread_mutex_lock(&semaforo_memoria);
		loggearInfo("Se registró la señal para vaciar la tabla de páginas");
		volcar_memoria_principal(memoriaConfig);
		pthread_mutex_unlock(&semaforo_memoria);

		break;

	}

}

void vaciar_tlb(t_config * configuracion_vaciar_tlb) {

	int cantidad_elementos = list_size(tlb);
	t_tlb_item * item;

	while (cantidad_elementos) {

		item = list_remove(tlb, 0);
		if (item->modificado) {
			tabla_paginas_buscar(item->pid, item->pagina,
					configuracion_vaciar_tlb, true);
		}
		free(item);

		cantidad_elementos--;
	}

}

void vaciar_tabla_paginas(t_config * configuracion_vaciar_tabla_paginas) {

	int cantidad_elementos = list_size(tabla_paginas);
	t_tabla_paginas_item * item;

	while (cantidad_elementos) {

		item = list_remove(tabla_paginas, 0);
		if (item->modificado) {
			swap_escribir(item->pid, item->pagina, item->marco,
					configuracion_vaciar_tabla_paginas);
		}
		free(item);

		cantidad_elementos--;
	}

}

//Definir D:
void volcar_memoria_principal(t_config * configuracion_volcar) {

	int tamanio_marco = config_get_int_value(configuracion_volcar,
			"TAMANIO_MARCO");
	int cantidad_marcos = config_get_int_value(configuracion_volcar,
			"CANTIDAD_MARCOS");

	loggearInfo("INICIO DEL CONTENIDO");

	int i;
	for (i = 0; i < cantidad_marcos; i++) {

		char * string = malloc(tamanio_marco);
		memcpy(string, memoria + tamanio_marco * i, tamanio_marco);

		loggearInfo(
				string_from_format("En el marco %d se encuentra %s", i,
						string));

		free(string);
	}

	loggearInfo("FIN DEL CONTENIDO");

}
