/*
 * swap.c
 *
 *  Created on: 5/11/2015
 *      Author: utnso
 */

#include "swap.h"



bool swap_iniciar(int pid, int cantidad_paginas) {

	int cantidad = cantidad_paginas;

	t_data * paquete = pedirPaquete(INICIAR, sizeof(int), &cantidad);
	common_send(socketSwap, paquete);

	paquete = leer_paquete(socketSwap);

	return (paquete->header->codigo_operacion == ALOCADO);

}

void swap_escribir(int pid, int pagina,int marco){

	common_send(socketSwap,pedirPaquete(ESCRIBIR,tamanio_marcos,memoria+marco*tamanio_marcos));

}

