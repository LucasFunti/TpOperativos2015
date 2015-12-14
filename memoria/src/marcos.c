#include "marcos.h"

void iniciar_marcos() {

	cola_llegada = list_create();
	tlb = list_create();
	tabla_paginas = list_create();

	aciertos_totales = 0;
	pedidos_totales = 0;

	int contadorAuxiliar;
	for(contadorAuxiliar=0;contadorAuxiliar<100;contadorAuxiliar++) {

		registroTlb[contadorAuxiliar].aciertos = 0;
		registroTlb[contadorAuxiliar].pedidos = 0;

	}

	numero_operacion = 0;

	int cantidad_marcos = config_get_int_value(memoriaConfig,
			"CANTIDAD_MARCOS");

	int tamanio_marcos = config_get_int_value(memoriaConfig, "TAMANIO_MARCO");

	memoria = malloc(tamanio_marcos * cantidad_marcos);

	if (memoria == NULL) {
		exit(EXIT_FAILURE);
	}

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

int cantidad_marcos_libres() {

	return list_count_satisfying(marcos_disponibles, esta_libre);

}

bool esta_libre(void * data) {

	t_marco * marco = data;

	return (marco->disponible == true);
}
