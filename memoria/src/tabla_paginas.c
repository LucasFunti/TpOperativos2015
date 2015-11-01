/*
 * tabla_paginas.c
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */
#include "tabla_paginas.h"

hit_miss tabla_paginas_buscar(t_tabla_paginas * tabla_paginas, int pid_a_buscar,
		int pagina_a_buscar) {

	t_tabla_paginas_item * fila_encontrada_mismo_pid = tabla_paginas_buscar_pid(
			tabla_paginas, pid_a_buscar);

	if (fila_encontrada_mismo_pid == NULL) {
		return MISS;
	}

	pagina_para_buscar_tabla_paginas = pagina_a_buscar;
	t_par_pagina_marco * par_pagina_marco_misma_pagina = list_find(
			fila_encontrada_mismo_pid->pares_pagina_marco,
			funcion_par_pagina_marco_buscar_por_pagina);

	if (par_pagina_marco_misma_pagina != NULL) {
		tabla_paginas_match = par_pagina_marco_misma_pagina;
		return HIT;
	} else {
		return MISS;
	}

}

t_tabla_paginas_item * tabla_paginas_buscar_pid(t_tabla_paginas * tabla_paginas,
		int pid) {

	pid_para_buscar_tabla_paginas = pid;
	return (list_find(tabla_paginas, funcion_tabla_paginas_buscar_por_pid));
}

void tabla_paginas_anadir_par_pagina_marco(t_tabla_paginas * tabla,int pid, int pagina, int marco) {

	pid_para_buscar_tabla_paginas = pid;

	t_tabla_paginas_item * item_tabla_pagina = tabla_paginas_buscar_pid(tabla,pid);

	t_par_pagina_marco * item_para_agregar = new_par_pagina_marco(pagina,
			marco);

	list_add(item_tabla_pagina->pares_pagina_marco, item_para_agregar);

}

void tabla_paginas_remover(t_tabla_paginas * tabla_paginas, int pid) {

	pid_para_remover_tabla_paginas = pid;
	list_remove_and_destroy_by_condition(tabla_paginas,
			funcion_tabla_paginas_coincide_pid_remover,
			funcion_tabla_paginas_liberar_item);

}

bool funcion_tabla_paginas_coincide_pid_remover(void * data) {
	t_tabla_paginas_item * fila = data;
	return (fila->pid == pid_para_remover_tabla_paginas);
}

void funcion_tabla_paginas_liberar_item(void * data) {
	t_tabla_paginas_item * fila = data;
	list_destroy_and_destroy_elements(fila->pares_pagina_marco, free);
	free(fila);
}

bool funcion_par_pagina_marco_buscar_por_pagina(void * data) {

	t_par_pagina_marco * fila = data;

	return (fila->pagina == pagina_para_buscar_tabla_paginas);
}

bool funcion_tabla_paginas_buscar_por_pid(void * data) {

	t_tabla_paginas_item * fila = data;

	return (fila->pid == pid_para_buscar_tabla_paginas);
}

t_tabla_paginas_item * new_tabla_paginas_item(int pid, int pagina_inicial,
		int marco_pagina_inicial) {

	t_tabla_paginas_item * nuevo_item = malloc(sizeof(t_tabla_paginas_item));
	nuevo_item->pid = pid;
	nuevo_item->pares_pagina_marco = list_create();

	t_list * lista_pares = nuevo_item->pares_pagina_marco;

	list_add(lista_pares,
			new_par_pagina_marco(pagina_inicial, marco_pagina_inicial));

	return nuevo_item;

}

void tabla_paginas_add(t_tabla_paginas * tabla, t_tabla_paginas_item * item) {
	list_add(tabla, item);
}

t_par_pagina_marco * new_par_pagina_marco(int pagina, int marco) {

	t_par_pagina_marco * nuevo_item = malloc(sizeof(t_par_pagina_marco));
	nuevo_item->pagina = pagina;
	nuevo_item->marco = marco;

	return nuevo_item;
}

int marco_para_pagina(t_tabla_paginas_item * item_tabla_paginas, int pagina) {

	t_list * pares_pagina_marco = item_tabla_paginas->pares_pagina_marco;

	pagina_para_buscar_tabla_paginas = pagina;
	t_par_pagina_marco * par_encontrado = list_find(pares_pagina_marco,
			funcion_buscar_marco);

	return par_encontrado->marco;

}

bool funcion_buscar_marco(void* data) {

	t_par_pagina_marco* par = data;
	return (par->pagina == pagina_para_buscar_tabla_paginas);

}
