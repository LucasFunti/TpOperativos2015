/*
 * pedidos.c
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */
#include "pedidos.h"

/**
 * Principales
 */
int iniciar_n(int pid, int cantidad_paginas) {
	return 1;
}

char * leer_n(int pid, int pagina) {

	return "Hola";
}

void escribir_n(char * texto_para_escribir) {

}

void finalizar(int pid, t_dictionary * diccionario) {

	if (tlb_habilitada(diccionario)) {
		remover_tlb(tlb, pid);

	}

	remover_tabla_paginas(tabla_paginas, pid);
	remover_swap(pid);

}

/**
 * Secundarios
 */

int marco_pagina(t_tlb * tabla_tlb, t_tabla_paginas * tabla_pags,
		t_dictionary * diccionario, int pid, int pagina) {

	if (tlb_habilitada(diccionario)) {
		return buscar_tlb(tabla_tlb, tabla_pags, pid, pagina);
	}
	return buscar_tabla(tabla_pags, pid, pagina);
}

int buscar_tlb(t_tlb * tabla, t_tabla_paginas * tabla_pags, int pid, int pagina) {

	encontrado = tlb_buscar(tabla, pid, pagina);

	if (encontrado == HIT) {

		return tlb_match->marco;

	}
	return buscar_tabla(tabla_pags, pid, pagina);
}

int buscar_tabla(t_tabla_paginas * tabla_pags, int pid, int pagina) {

	encontrado = tabla_paginas_buscar(tabla_pags, pid, pagina);
	if (encontrado == HIT) {
		return tabla_paginas_match->marco;
	}
	return buscar_swap(pid, pagina);
}

//TODO: implementar
int buscar_swap(int pid, int pagina) {

	return -1;
}

bool tlb_habilitada(t_dictionary * diccionario) {

	char * valor = malloc(sizeof(5));
	strcpy(valor, dictionary_get(diccionario, "TLB_HABILITADA"));

	return (!strcmp(valor, "SI")); //Si son iguales devuelve 0
}

void remover(int pid) {

}

void remover_tlb(t_tlb * tabla, int pid) {
	tlb_remover_entradas_para(tabla, pid);
}

void remover_tabla_paginas(t_tabla_paginas * tabla, int pid) {
	tabla_paginas_remover(tabla, pid);
}

void remover_swap(int pid) {

}
