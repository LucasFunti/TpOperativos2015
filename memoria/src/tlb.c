#include "tlb.h"

hit_miss tlb_buscar(t_tlb * tlb_memoria, int pid_a_buscar, int pagina_a_buscar) {

	//A estas variables las accede la función que le damos a list_find
	pid_para_buscar_tlb = pid_a_buscar;
	pagina_para_buscar_tlb = pagina_a_buscar;

	t_tlb_item * fila_encontrada = list_find(tlb_memoria,
			funcion_tlb_buscar_por_pid_y_pagina);

	if (fila_encontrada != NULL) {
		tlb_match = fila_encontrada->par_pagina_marco;
		return HIT;
	} else {
		return MISS;
	}
}

void tlb_remover(t_tlb * tlb, int pid) {

	pid_para_remover_tlb = pid;
	list_remove_and_destroy_by_condition(tlb, funcion_tlb_coincide_pid_remover,
			funcion_tlb_liberar_item);

}

bool funcion_tlb_coincide_pid_remover(void * data) {
	t_tlb_item * fila = data;
	return (fila->pid == pid_para_remover_tlb);
}

void funcion_tlb_liberar_item(void * data) {
	t_tlb_item * fila = data;
	free(fila->par_pagina_marco);
	free(fila);
}

bool funcion_tlb_buscar_por_pid_y_pagina(void * elemento_a_verificar) {

	t_tlb_item * fila_tlb = elemento_a_verificar;
	return (fila_tlb->pid == pid_para_buscar_tlb
			&& fila_tlb->par_pagina_marco->pagina == pagina_para_buscar_tlb);
}

void tlb_add(t_tlb * tlb_agregar, t_tlb_item * item_para_agregar) {
	list_add(tlb_agregar, item_para_agregar);
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
