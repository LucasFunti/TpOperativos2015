#include "tlb.h"

t_config * configs;

void tlb_remover_pid(int pid) {

	pid_matchear = pid;

	int cantidad_restante = list_count_satisfying(tlb, coincide_pid);

	while (cantidad_restante) {

		list_remove_by_condition(tlb, coincide_pid);

		cantidad_restante--;
	}

	tabla_paginas_remover_pid(pid);

}

bool tlb_habilitada() {

	char * valor = malloc(4);

	valor = config_get_string_value(memoriaConfig, "TLB_HABILITADA");

	return (!strcmp(valor, "SI"));

}

t_item * tlb_buscar(int pid, int pagina) {

	if (tlb_habilitada(memoriaConfig)) {

		pid_matchear = pid;
		pagina_matchear = pagina;

		pedidos_totales++;
		registroTlb[pid].pedidos++;

		t_item * item_encontrado = list_find(tlb, coincide_pid_y_pagina);

		//No lo encuentra - Lo busca en la tabla de páginas y lo agrega a la tlb
		if (item_encontrado == NULL) {

			item_encontrado = tabla_paginas_buscar(pid, pagina);

			if (item_encontrado != NULL) {
				tlb_agregar_item(item_encontrado);
			}

			return item_encontrado;

		} else {

			// Lo encontró

			aciertos_totales++;

			registroTlb[pid].aciertos++;

			return item_encontrado;
		}

	} else {

		return tabla_paginas_buscar(pid, pagina);

	}

}

void tlb_agregar_item(t_item * item) {

	int cantidad_entradas = config_get_int_value(memoriaConfig, "ENTRADAS_TLB");

	if (list_size(tlb) <= cantidad_entradas) {

		list_add(tlb, item);

	} else {

		//Swapea
		list_remove(tlb, 0);

		//Agrego el nuevo
		list_add(tlb, item);

	}
}
