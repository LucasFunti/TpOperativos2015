/*
 * tabla_paginas.c
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */
#include "tabla_paginas.h"

void tabla_paginas_aniadir_item(int pid, int pagina, int marco) {

	t_tabla_paginas_item * nuevo = malloc(sizeof(t_tabla_paginas_item));

	nuevo->pid = pid;
	nuevo->pagina = pagina;
	nuevo->marco = marco;
	nuevo->modificado = false;

	list_add(tabla_paginas, nuevo);

}

marco tabla_paginas_buscar(int pid, int pagina, t_config * configuraciones) {

	pid_matchear_tabla_paginas = pid;
	pagina_matchear_tabla_paginas = pagina;

	t_tabla_paginas_item * item_encontrado = list_find(tabla_paginas,
			coincide_pid_y_pagina_tabla_paginas);

	if (item_encontrado == NULL) {

		int tamanio_marco = config_get_int_value(configuraciones,
				"TAMANIO_MARCO");

		int marco_a_asignar = marco_libre();

		tabla_paginas_aniadir_item(pid, pagina, marco_a_asignar);

		char * contenido = swap_leer(pid, pagina);

		strcpy(memoria + tamanio_marco * marco_a_asignar, contenido);

		//Deberia rellenar el contenido faltante con /0

		return marco_a_asignar;
	}

	else {

		return item_encontrado->marco;

	}

}

bool coincide_pid_y_pagina_tabla_paginas(void * data) {

	t_tabla_paginas_item * item = data;
	return (item->pid == pid_matchear_tabla_paginas
			&& item->pagina == pagina_matchear_tabla_paginas);
}
