/*
 * iniciar_n.c
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */
#include "iniciar_n.h"

bool iniciar_n(int pid, int cantidad_paginas, t_config * configuraciones,
bool test) {

	configuraciones_iniciar_n = configuraciones;
	bool swap_puede;

	if (test) {
		swap_puede = true;
	} else {
		swap_puede = swap_iniciar(pid, cantidad_paginas);
	}

	int cantidad_maxima_marcos_proceso = config_get_int_value(configuraciones,
			"MAXIMO_MARCOS_POR_PROCESO");

	if (swap_puede && cantidad_maxima_marcos_proceso >= cantidad_paginas) {

		crear_estructura_para_proceso(pid, cantidad_paginas);

		return true;

	} else {

		return false;

	}

}

void crear_estructura_para_proceso(int pid, int cantidad_paginas) {

	int i;
	for (i = 0; i < cantidad_paginas; i++) {
		//False porque recién se crean y no tienen datos, entonces no están modificados
		tabla_paginas_aniadir_item(pid, i + 1, marco_libre(), false);
	}

}

int marco_libre() {

	if (cantidad_marcos_libres()) {

		t_marco * marco = list_find(marcos_disponibles, esta_libre);

		marco->disponible = false;

		return marco->numero_marco;

	} else {

		//Swapea el primero asignado (FIFO)
		int indice_para_swappear = 0;

		t_tabla_paginas_item * entrada_a_swappear = list_remove(tabla_paginas,
				indice_para_swappear);

		//Si está modificado (el swap está atrasado, le pido que escriba el contenido)
		if (entrada_a_swappear->modificado) {
			swap_escribir(entrada_a_swappear->pid, entrada_a_swappear->pagina,
					entrada_a_swappear->marco, configuraciones_iniciar_n);
		}

		tlb_sacar_entrada(entrada_a_swappear->pid, entrada_a_swappear->pagina,
				configuraciones_iniciar_n);

		return entrada_a_swappear->marco;

	}
}

