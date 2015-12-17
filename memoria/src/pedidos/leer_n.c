/*
 * leer_n.c
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */
#include "leer_n.h"

bool leer_n(int pid, int pagina) {

	int tamanio_marco = config_get_int_value(memoriaConfig, "TAMANIO_MARCO");

	t_item * item = tlb_buscar(pid, pagina);

	if (item == NULL) {

		return false;

	} else {

		item->numero_operacion = get_numero_operacion();

		if (!item->presencia) {

			//No esta presente
			page_faults++;

			pid_matchear = item->pid;
			pagina_matchear = item->pagina;

			int marco = marco_libre(pid);

			if (marco == -1) {

				contenido_lectura = NULL;
				return false;

			} else {

				char * contenido_Pagina = swap_leer(pid, pagina);

				retardo(memoriaConfig);
				memcpy(memoria + tamanio_marco * item->marco, contenido_Pagina,
						tamanio_marco);

				contenido_lectura = contenido_Pagina;

				item->marco = marco;
				item->presencia = true;
				list_add(cola_llegada, item);
				return true;
			}
		} else {

			//Esta presente

			contenido_lectura = malloc(tamanio_marco);
			retardo(memoriaConfig);

			memcpy(contenido_lectura, memoria + item->marco * tamanio_marco,
					tamanio_marco);
			return true;
		}

	}
}

