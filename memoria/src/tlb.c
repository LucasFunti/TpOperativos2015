#include "tlb.h"

void tlb_sacar_entrada(int pid, int pagina, t_config * configuraciones) {

	if (tlb_habilitada(configuraciones)) {

		pid_matchear_tlb = pid;
		pagina_matchear_tlb = pagina;

		list_remove_and_destroy_by_condition(tlb, coincide_pid_y_pagina, free);

	}

}

bool tlb_habilitada(t_config * configuraciones) {

	char * valor = malloc(4);

	valor = config_get_string_value(configuraciones, "TLB_HABILITADA");

	return (!strcmp(valor, "SI"));

}

bool coincide_pid_y_pagina(void * data) {

	t_tlb_item * item = data;
	return (item->pid == pid_matchear_tlb && item->pagina == pagina_matchear_tlb);
}

marco tlb_buscar(int pid, int pagina, t_config * configuraciones) {

	if (tlb_habilitada(configuraciones)) {

		t_tlb_item * item_encontrado = list_find(tlb, coincide_pid_y_pagina);

		if (item_encontrado == NULL) {

			marco marco_encontrado_para_agregar_tlb = tabla_paginas_buscar(pid, pagina,
					configuraciones);

			tlb_agregar_entrada(pid, pagina, marco_encontrado_para_agregar_tlb,
					configuraciones);

			return marco_encontrado_para_agregar_tlb;

		} else {
			return item_encontrado->marco;
		}

	} else {

		return tabla_paginas_buscar(pid, pagina, configuraciones);

	}

}

void tlb_agregar_entrada(int pid, int pagina, int marco, t_config * configuraciones){

	//TODO

}
