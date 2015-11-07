/*
 * iniciar_n.h
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */
#include <commons/config.h>
#include <stdbool.h>
#include "../tlb.h"
#include "../tabla_paginas.h"
#include "../marcos.h"

t_config * configuraciones_iniciar_n;

/**
 * Te encuentra un marco
 */
int marco_libre();

/**
 * Iniciar-N
 */
bool iniciar_n(int pid, int cantidad_paginas, t_config * configuraciones,
		bool test);
void crear_estructura_para_proceso(int pid, int cantidad_paginas);

