/*
 * iniciar_n.h
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */

#include <stdbool.h>
#include "../tlb.h"
#include "../tabla_paginas.h"
#include "../marcos.h"
#include "../algoritmos/algoritmos.h"

/**
 * Te encuentra un marco
 */
int marco_libre(int pid);

/**
 * Iniciar-N
 */
bool iniciar_n(int pid, int cantidad_paginas);
void crear_estructura_para_proceso(int pid, int cantidad_paginas);

bool es_fifo();
bool es_lru();
bool es_clock_modificado();
