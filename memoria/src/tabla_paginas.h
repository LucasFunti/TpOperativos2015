/*
 *
 * tabla_paginas.h
 *  Created on: 29/10/2015
 *      Author: utnso
 */

#ifndef TABLA_PAGINAS_H_
#define TABLA_PAGINAS_H_

#include <stdlib.h>
#include "estructuras.h"
#include "pedidos/iniciar_n.h"
#include "pedidos/escribir_n.h"
#include "swap.h"
#include "retardo.h"
#include "algoritmos/algoritmos.h"

void tabla_paginas_remover_pid(int pid);

/**
 * tabla_paginas_aniadir_item
 */
t_item * tabla_paginas_aniadir_item(int pid, int pagina, int marco);

/**
 * tabla_paginas_buscar
 */
t_item * tabla_paginas_buscar(int pid, int pagina);

#endif /* TABLA_PAGINAS_H_ */
