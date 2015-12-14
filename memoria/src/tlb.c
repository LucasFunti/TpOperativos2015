#include "tlb.h"

t_config * configs;

void tlb_remover_pid(int pid, t_config * configuraciones, bool es_test) {

	configs = configuraciones;

	pid_matchear_tlb = pid;

	int cantidad_restante = list_count_satisfying(tlb, coincide_pid_tlb);

	while (cantidad_restante) {

		list_remove_and_destroy_by_condition(tlb,
				coincide_pid_tlb_y_actualiza_tabla_paginas_modificado, free);

		cantidad_restante--;
	}

	tabla_paginas_remover_pid(pid, configuraciones, es_test);

}

bool coincide_pid_tlb(void * data) {

	t_tlb_item * item = data;

	return (item->pid == pid_matchear_tlb);

}

bool coincide_pid_tlb_y_actualiza_tabla_paginas_modificado(void * data) {

	t_tlb_item * item = data;

	if (item->pid == pid_matchear_tlb && item->modificado) {

		//Si bien no es para esto (ya que me devuelve un marco que no uso), me permite setearle el modificado
		tabla_paginas_buscar(item->pid, item->pagina, configs, true);
	}

	return (item->pid == pid_matchear_tlb);

}

void tlb_sacar_entrada(int pid, int pagina, t_config * configuraciones) {

	if (tlb_habilitada(configuraciones)) {

		pid_matchear_tlb = pid;
		pagina_matchear_tlb = pagina;

		list_remove_and_destroy_by_condition(tlb, coincide_pid_y_pagina, free);

	}

}

void tlb_sacar_presencia(int pid, int pagina, t_config * configuraciones) {

	pid_matchear_tlb = pid;
	pagina_matchear_tlb = pagina;

	list_iterate(tlb, coincide_pid_y_pagina_iterador);

}

void coincide_pid_y_pagina_iterador(void * data) {

	t_tlb_item * item = data;
	if (item->pid == pid_matchear_tlb && item->pagina == pagina_matchear_tlb) {
		item->presencia = 0;
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

marco tlb_buscar(int pid, int pagina, t_config * configuraciones,
bool es_escritura) {

	if (tlb_habilitada(configuraciones)) {

		pid_matchear_tlb = pid;
		pagina_matchear_tlb = pagina;

		pedidos_totales++;
		registroTlb[pid].pedidos++;

		t_tlb_item * item_encontrado = list_find(tlb, coincide_pid_y_pagina);

		//No lo encuentra - Lo busca en la tabla de páginas y lo agrega a la tlb
		if (item_encontrado == NULL) {

			marco marco_encontrado_para_agregar_tlb = tabla_paginas_buscar(pid,
					pagina, configuraciones, es_escritura);

			tlb_agregar_entrada(pid, pagina, marco_encontrado_para_agregar_tlb,
					es_escritura);

			return marco_encontrado_para_agregar_tlb;

		} else { // Lo encontró
			aciertos_totales++;
			registroTlb[pid].aciertos++;
			//Si es operación de escritura, se pone en modificado
			if (es_escritura) {
				item_encontrado->modificado = true;
			}

			return item_encontrado->marco;
		}

	} else {

		return tabla_paginas_buscar(pid, pagina, configuraciones, es_escritura);

	}

}

void tlb_agregar_entrada(int pid, int pagina, int marco, bool es_escritura) {

	int cantidad_entradas = config_get_int_value(memoriaConfig, "ENTRADAS_TLB");

	if (list_size(tlb) <= cantidad_entradas) {

		t_tlb_item * item = malloc(sizeof(t_tlb_item));

		item->pid = pid;
		item->pagina = pagina;
		item->marco = marco;
		item->modificado = es_escritura;
		item->presencia = true;


		list_add(tlb, item);

	} else {

		//Swapea
		t_tlb_item * item = list_get(tlb, 0);
		if (item->modificado) {
			//Actualiza el modificado en la tabla de páginas
			tabla_paginas_buscar(pid, pagina, memoriaConfig, true);
		}
		//Lo saco
		list_remove_and_destroy_element(tlb, 0, free);

		item = malloc(sizeof(t_tlb_item));

		item->pid = pid;
		item->pagina = pagina;
		item->marco = marco;
		item->modificado = es_escritura;
		item->numero_operacion = 0;

		//Agrego el nuevo
		list_add(tlb, item);

	}
}
