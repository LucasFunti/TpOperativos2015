/*
 * tabla_paginas.c
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */
#include "tabla_paginas.h"

void tabla_paginas_remover_pid(int pid) {

	pid_matchear = pid;

	int cantidad_restante = list_count_satisfying(cola_llegada, coincide_pid);

	while (cantidad_restante) {

		list_remove_by_condition(cola_llegada, coincide_pid);

		cantidad_restante--;

	}

	cantidad_restante = list_count_satisfying(tabla_paginas, coincide_pid);

	while (cantidad_restante) {

		//Esta accediendo a la tabla de páginas, va retardo
		retardo(memoriaConfig);

		list_remove_and_destroy_by_condition(tabla_paginas,
				coincide_pid_y_libera_marcos, free);

		cantidad_restante--;
	}

	if (!test) {
		swap_finalizar(pid);
	}

}

t_item * tabla_paginas_aniadir_item(int pid, int pagina, int marco) {

	t_item * nuevo = malloc(sizeof(t_item));

	nuevo->pid = pid;
	nuevo->pagina = pagina;
	nuevo->marco = marco;
	nuevo->modificado = false;
	nuevo->numero_operacion = 0;
	nuevo->presencia = false;
	nuevo->uso = 1;
	nuevo->puntero = false;

	list_add(tabla_paginas, nuevo);

	return nuevo;
}

t_item * tabla_paginas_buscar(int pid, int pagina) {

//Busca en la tabla_paginas, usa retardo
	retardo(memoriaConfig);

	pid_matchear = pid;
	pagina_matchear = pagina;

	t_item * item_encontrado = list_find(tabla_paginas, coincide_pid_y_pagina);

	if (item_encontrado == NULL) {

		int tamanio_marco = config_get_int_value(memoriaConfig,
				"TAMANIO_MARCO");

		int marco_a_asignar = marco_libre(pid);

		if (marco_a_asignar == FALLO_MARCO) {

			return NULL;

		} else {

			item_encontrado = tabla_paginas_aniadir_item(pid, pagina,
					marco_a_asignar);

			item_encontrado->presencia = true;

			accesos_swap++;

			if (!test) {

				char * contenido = swap_leer(pid, pagina);

				escribir_n(pid, pagina, contenido);
			}

			return item_encontrado;
		}
	}

	else {

		return item_encontrado;

	}

}
