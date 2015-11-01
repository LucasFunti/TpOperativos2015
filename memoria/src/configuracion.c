/*
 * configuracion.c
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */
#include "configuracion.h"

void levantar_configuracion() {

	memoriaConfig = config_create("./memoriaConfig");

	diccionario_configuraciones = dictionary_create();

	char variablesConfiguracion[9][30] = { "PUERTO_ESCUCHA", "IP_SWAP",
			"PUERTO_SWAP", "MAXIMO_MARCOS_POR_PROCESO", "CANTIDAD_MARCOS",
			"TAMANIO_MARCO", "ENTRADAS_TLB", "TLB_HABILITADA",
			"ALGORITMO_REMPLAZO" };
	int i;
	for (i = 0; i < 9; i++) {

		dictionary_put(diccionario_configuraciones, variablesConfiguracion[i],
				config_get_string_value(memoriaConfig,
						variablesConfiguracion[i]));

	}
}

