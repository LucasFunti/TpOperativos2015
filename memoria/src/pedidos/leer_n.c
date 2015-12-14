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

		if (!item->presencia) {

			int marco = marco_libre(pid);

			if (marco == -1) {

				contenido_lectura = NULL;
				return false;

			} else {

				if(!test){
					contenido_lectura = swap_leer(pid, pagina);
				}else{
					contenido_lectura = malloc(2);
					contenido_lectura = "T";
				}

				item->marco = marco;
				item->presencia = true;
				return true;
				list_add(cola_llegada,item);
			}
		} else {

			contenido_lectura = malloc(tamanio_marco);
			retardo(memoriaConfig);

			memcpy(contenido_lectura, memoria + item->marco * tamanio_marco,
					tamanio_marco);
			return true;
		}

	}
}

