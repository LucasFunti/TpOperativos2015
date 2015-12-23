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
		//Fallo de marco, aborta el proceso
		return false;

	} else {

		item->numero_operacion = get_numero_operacion();

		item->uso = true;

		if (!item->presencia) {

			loggearInfo("El elemento no está presente, se trae a memoria");

			//No esta presente
			page_faults++;

			pid_matchear = item->pid;
			pagina_matchear = item->pagina;

			int marco = marco_libre(pid);

			if (marco == -1) {

				contenido_lectura = NULL;
				return false;

			} else {

				log_info(logger,
						"Se le asigna el marco %d al proceso %d a la página %d",
						marco, pid, pagina);

				char * contenido_Pagina = swap_leer(pid, pagina);

				loggearInfo("Escribiendo contenido del swap");

				retardo();
				memcpy(memoria + tamanio_marco * marco, contenido_Pagina,
						tamanio_marco);

				contenido_lectura = contenido_Pagina;

				item->marco = marco;
				item->presencia = true;

				if (ignorar_proximoAgregar) {
					ignorar_proximoAgregar = false;
				} else {
					list_add(cola_llegada, item);
				}

				return true;
			}
		} else {

			loggearInfo("El elemento está presente");
			//Esta presente

			contenido_lectura = malloc(tamanio_marco);

			loggearInfo("Accediendo al contenido existente");
			retardo();

			memcpy(contenido_lectura, memoria + item->marco * tamanio_marco,
					tamanio_marco);
			return true;
		}

	}
}

