/*
 * escribir_n.c
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */
#include "escribir_n.h"

bool escribir_n(int pid, int pagina, char * contenido) {

	t_item * item = tlb_buscar(pid, pagina);

	if (item == NULL) {
		//Fallo de marco, aborta el proceso
		return false;

	} else {

		if (!item->presencia) {

			page_faults++;
			swap_leer(pid, pagina);

			int marco = marco_libre(pid);

			if (marco == -1) {

				return false;

			} else {

				item->marco = marco;
				item->presencia = true;

				if (ignorar_proximoAgregar) {
					ignorar_proximoAgregar = false;
				} else {
					list_add(cola_llegada, item);
				}
			}

		}

		//Ya está presente

		int tamanio_marco = config_get_int_value(memoriaConfig,
				"TAMANIO_MARCO");

		int tamanio_string = strlen(contenido) + 1;

		char * contenido_a_escribir = malloc(tamanio_marco);

		if (tamanio_marco > tamanio_string) {

			int cantidad_completar = tamanio_marco - tamanio_string;

			memcpy(contenido_a_escribir, contenido, tamanio_string);

			memcpy(contenido_a_escribir + tamanio_string,
					string_repeat('\0', cantidad_completar),
					cantidad_completar);

		} else {

			memcpy(contenido_a_escribir, contenido, tamanio_marco - 1);
			contenido_a_escribir[14] = '\0';

		}

		//Escribe en memoria principal, entonces es con retardo
		retardo(memoriaConfig);
		memcpy(memoria + tamanio_marco * item->marco, contenido_a_escribir,
				tamanio_marco);

		item->modificado = true;
		item->numero_operacion = get_numero_operacion();

		return true;

	}

}

