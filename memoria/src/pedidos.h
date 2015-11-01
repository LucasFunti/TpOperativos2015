/*
 * pedidos.h
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */

#ifndef PEDIDOS_H_
#define PEDIDOS_H_

#include <stdlib.h>
#include <string.h>
#include "constantes.h"
#include "globals.h"
#include "tlb.h"
#include "tabla_paginas.h"

int encontrado;

void remover(int);

void remover_tlb(int);
void remover_tabla_paginas(int);
void remover_swap(int);

/**
 * @NAME: buscar_tlb
 * @DESC: Delega en tlb_buscar
 * Busca en la tlb, la tabla y
 * por último en swap. Devuelve
 * el numero de marco
 */
int buscar_tlb(int, int);

/**
 * @NAME: buscar_tabla
 * @DESC: Delega en tabla_paginas_buscar
 * Busca en la tlb, la tabla y
 * por último en swap. Devuelve
 * el numero de marco
 */
int buscar_tabla(int, int);

/**
 * @NAME: buscar_swap
 * @DESC: Deberia pedirlo al swap, con todo lo que eso implica
 */
int buscar_swap(int pid, int pagina);

/**
 * @NAME: leer_n
 * @DESC: Devuelve el numero de marco donde está
 * la página del proceso pedido
 */
int leer_n(int, int);

/**
 * @NAME: tlb_habilitada
 * @DESC: devuelve true si la el archivo de configuración
 * dice algo distinto a NO
 */
bool tlb_habilitada();

#endif /* PEDIDOS_H_ */
