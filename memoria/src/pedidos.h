/*
 * pedidos.h
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */

#ifndef PEDIDOS_H_
#define PEDIDOS_H_

#include <commons/config.h>
#include <stdbool.h>

/**
 * Te encuentra un marco
 */
int marco_libre();

/**
 * Iniciar-N
 */
bool iniciar_n(int pid, int cantidad_paginas, t_config * configuraciones);
void crear_estructura_para_proceso(int pid, int cantidad_paginas);


#endif /* PEDIDOS_H_ */
