#include "marcos.h"

void iniciar_marcos() {

	cola_llegada = list_create();
	log_info(logger, "Se inició la cola de llegada");

	tlb = list_create();
	log_info(logger, "Se inició la tlb");

	tabla_paginas = list_create();
	log_info(logger, "Se inició la tabla de páginas");

	ignorar_proximoAgregar = false;

	aciertos_totales = 0;
	pedidos_totales = 0;

	log_info(logger,
			"Se setearon en cero la cantidad de pedidos y aciertos totales");

	accesos_swap = 0;
	page_faults = 0;

	log_info(logger,
			"Se setearon en cero la cantidad de page faults y accesos a swap");

	int contadorAuxiliar;
	for (contadorAuxiliar = 0; contadorAuxiliar < 200; contadorAuxiliar++) {

		registroTlb[contadorAuxiliar].aciertos = 0;
		registroTlb[contadorAuxiliar].pedidos = 0;

	}

	log_info(logger,
			"Se inició la estructura de aciertos por programa de la tlb");

	numero_operacion = 1;

	int cantidad_marcos = config_get_int_value(memoriaConfig,
			"CANTIDAD_MARCOS");

	int tamanio_marcos = config_get_int_value(memoriaConfig, "TAMANIO_MARCO");

	memoria = malloc(tamanio_marcos * cantidad_marcos);

	if (memoria == NULL) {
		exit(EXIT_FAILURE);
	}

	log_info(logger, "Se reservó el espacio de memoria");

	marcos_disponibles = list_create();

	t_marco * marco;
	int i;
	for (i = 0; i < cantidad_marcos; i++) {

		marco = malloc(sizeof(t_marco));

		marco->numero_marco = i;
		marco->disponible = true;

		list_add(marcos_disponibles, marco);
	}
}

int marcos_libres() {

	return list_count_satisfying(marcos_disponibles, esta_libre);

}

int marco_disponible() {

	t_marco * marcoParaDar = list_find(marcos_disponibles, esta_libre);

	marcoParaDar->disponible = false;
	return marcoParaDar->numero_marco;

}

bool tiene_marcos_asignados(int pid) {

	return marcos_asignados(pid) != 0;

}

int marcos_asignados(int pid) {

	pid_matchear = pid;

	return list_count_satisfying(tabla_paginas, coincide_pid_y_esta_presente);

}

bool esta_libre(void * data) {

	t_marco * marco = data;

	return (marco->disponible == true);
}
