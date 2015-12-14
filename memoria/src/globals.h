/*
 * globals.h
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/dictionary.h>
#include <stdbool.h>
#include "estructuras.h"

int numero_operacion;

bool test;

int pedidos_totales, aciertos_totales;

t_registro_tlb registroTlb[100];

void * memoria;

t_log * logger;

t_config * memoriaConfig;

t_list * cola_llegada;

/**
 * @NAME: variables principales
 * @DESC: son la TLB y la Tabla de Páginas
 * que usa la memoria
 */
t_tlb * tlb;
t_tabla_paginas * tabla_paginas;

#endif /* GLOBALS_H_ */
