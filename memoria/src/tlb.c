#include "tlb.h"

t_config * configs;

void tlb_remover_pid(int pid) {

	loggearInfo("Se remueven los elementos de la tlb");

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

		log_info(logger, "Se busca en la tlb");

		pid_matchear = pid;
		pagina_matchear = pagina;

		pedidos_totales++;
		registroTlb[pid].pedidos++;

		t_item * item_encontrado = list_find(tlb, coincide_pid_y_pagina);

		//No lo encuentra - Lo busca en la tabla de páginas y lo agrega a la tlb
		if (item_encontrado == NULL) {

			log_info(logger, "No se encontró en la tlb el elemento");

			item_encontrado = tabla_paginas_buscar(pid, pagina);

			if (item_encontrado != NULL) {
				log_info(logger, "Se agrega el item a la tabla de páginas");
				tlb_agregar_item(item_encontrado);
			}

			return item_encontrado;

		} else {

			// Lo encontró
			log_info(logger, "No se encontró en la TLB el elemento");

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
		log_info(logger,
				"Se agrega el item nuevo ya que hay entradas disponibles");
		list_add(tlb, item);

	} else {

		//Swapea
		log_info(logger,
				"Es necesario swappear la tlb, se remueve la primer entrada");
		list_remove(tlb, 0);

		//Agrego el nuevo
		log_info(logger, "Se agrega la nueva entrada");
		list_add(tlb, item);

	}
}

void mostrarTasaTlb(void * data) {

	printf("ADMINISTRADOR DE MEMORIA");

	while (1) {

		sleep(60);

		system("clear");
		printf("ADMINISTRADOR DE MEMORIA");

		if (tlb_habilitada()) {

			float tasa = (double) aciertos_totales / (double) pedidos_totales;

			loggearInfo(string_from_format("Tasa aciertos %.2f", tasa * 100));
			loggearInfo(
					string_from_format("Cantidad de pedidos tlb totales: %d",
							pedidos_totales));
			loggearInfo(
					string_from_format("Cantidad de aciertos tlb totales: %d",
							aciertos_totales));

			printf("Tasa aciertos %.2f \% \n\n", tasa * 100);
			printf("Cantidad de aciertos tlb totales: %d \n\n",
					aciertos_totales);
			printf("Cantidad de pedidos tlb totales: %d \n\n", pedidos_totales);
		}

	}

}
