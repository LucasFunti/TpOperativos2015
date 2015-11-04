#include "tlb.h"

/**
 * Principales
 */

void tlb_agregar_entrada(t_tlb * tabla, int pid, int pagina, int marco) {

	tlb_add(tabla, new_tlb_item(pid, pagina, marco));
}

void tlb_remover_entrada(t_tlb * tabla, int pid, int pagina) {

	pid_para_matchear_tlb = pid;
	pagina_para_matchear_tlb = pagina;
	list_remove_and_destroy_by_condition(tabla,
			funcion_tlb_buscar_por_pid_y_pagina, funcion_tlb_liberar_item);

}

void tlb_remover_entradas_para(t_tlb * tabla, int pid) {

	pid_para_matchear_tlb = pid;

	int cantidad_entradas;

	while (cantidad_entradas != 0) {

		list_remove_and_destroy_by_condition(tabla, funcion_tlb_coincide_pid,
				funcion_tlb_liberar_item);

		cantidad_entradas = list_count_satisfying(tabla,
				funcion_tlb_coincide_pid);
	}

}

hit_miss tlb_buscar(t_tlb * tabla, int pid_a_buscar, int pagina_a_buscar) {

	pid_para_matchear_tlb = pid_a_buscar;
	pagina_para_matchear_tlb = pagina_a_buscar;

	t_tlb_item * fila_encontrada = list_find(tabla,
			funcion_tlb_buscar_por_pid_y_pagina);

	if (fila_encontrada != NULL) {
		tlb_match = fila_encontrada->par_pagina_marco;
		return HIT;
	} else {
		return MISS;
	}
}

/**
 * Secundarios
 */

void tlb_add(t_tlb * tabla, t_tlb_item * item_para_agregar) {
	list_add(tabla, item_para_agregar);
}

t_tlb_item * new_tlb_item(int pid, int pagina, int marco) {

	t_tlb_item * new_tlb_item = malloc(sizeof(t_tlb_item));
	t_par_pagina_marco * par = malloc(sizeof(t_par_pagina_marco));

	new_tlb_item->pid = pid;

	par->pagina = pagina;
	par->marco = marco;

	new_tlb_item->par_pagina_marco = par;

	return new_tlb_item;
}

/**
 * Auxiliares para operar sobre listas
 */

void funcion_tlb_liberar_item(void * data) {
	t_tlb_item * fila = data;
	free(fila->par_pagina_marco);
	free(fila);
}

bool funcion_tlb_buscar_por_pid_y_pagina(void * data) {

	return (funcion_tlb_coincide_pid(data) && funcion_tlb_coincide_pagina(data));
}

bool funcion_tlb_coincide_pid(void * data) {
	t_tlb_item * fila = data;
	return (fila->pid == pid_para_matchear_tlb);
}

bool funcion_tlb_coincide_pagina(void * data) {
	t_tlb_item * fila = data;
	return (fila->par_pagina_marco->pagina == pagina_para_matchear_tlb);
}

