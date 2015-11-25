/*
 * tabla_paginas.c
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */
#include "tabla_paginas.h"

void tabla_paginas_remover_pid(int pid, t_config * configuraciones,
bool es_test) {

	pid_matchear_tabla_paginas = pid;

	int cantidad_restante = list_count_satisfying(cola_llegada,
			coincide_pid_tabla_paginas);

	while (cantidad_restante) {

		list_remove_by_condition(cola_llegada, coincide_pid_tabla_paginas);

		cantidad_restante--;

	}

	cantidad_restante = list_count_satisfying(tabla_paginas,
			coincide_pid_tabla_paginas);

	while (cantidad_restante) {

		//Esta accediendo a la tabla de páginas, va retardo
		retardo(configuraciones);

		list_remove_and_destroy_by_condition(tabla_paginas,
				coincide_pid_tabla_paginas_y_libera_marcos, free);

		cantidad_restante--;
	}

	if (!es_test) {
		swap_finalizar(pid);
	}

}

bool coincide_pid_tabla_paginas(void * data) {

	t_tabla_paginas_item * item = data;

	return (item->pid == pid_matchear_tabla_paginas);

}

bool coincide_pid_tabla_paginas_y_libera_marcos(void * data) {

	t_tabla_paginas_item * item = data;

	if (item->pid == pid_matchear_tabla_paginas) {

		t_marco * marco = list_get(marcos_disponibles, item->marco);
		marco->disponible = true;

	}

	return (item->pid == pid_matchear_tabla_paginas);

}


void tabla_paginas_aniadir_item(int pid, int pagina, int marco,
bool es_modificado, bool presente) {

	t_tabla_paginas_item * nuevo = malloc(sizeof(t_tabla_paginas_item));

	nuevo->pid = pid;
	nuevo->pagina = pagina;
	nuevo->marco = marco;
	nuevo->modificado = false;
	nuevo->numero_operacion = numero_operacion;
	nuevo->presencia = true;

	numero_operacion++;

	list_add(tabla_paginas, nuevo);
	list_add(cola_llegada,nuevo);

}

marco tabla_paginas_buscar(int pid, int pagina, t_config * configuraciones,
bool es_escritura) {

	//Busca en la tabla_paginas, usa retardo
	retardo(configuraciones);

	pid_matchear_tabla_paginas = pid;
	pagina_matchear_tabla_paginas = pagina;

	t_tabla_paginas_item * item_encontrado = list_find(tabla_paginas,
			coincide_pid_y_pagina_tabla_paginas);

	if (item_encontrado == NULL) {

		int tamanio_marco = config_get_int_value(configuraciones,
				"TAMANIO_MARCO");

		int marco_a_asignar = marco_libre();

		tabla_paginas_aniadir_item(pid, pagina, marco_a_asignar, true, true);

		char * contenido = swap_leer(pid, pagina);

		strcpy(memoria + tamanio_marco * marco_a_asignar, contenido);

		//Deberia rellenar el contenido faltante con /0

		return marco_a_asignar;
	}

	else {

		item_encontrado->numero_operacion = numero_operacion;
		numero_operacion++;

		return item_encontrado->marco;

	}

}

bool coincide_pid_y_pagina_tabla_paginas(void * data) {

	t_tabla_paginas_item * item = data;
	return (item->pid == pid_matchear_tabla_paginas
			&& item->pagina == pagina_matchear_tabla_paginas);
}
