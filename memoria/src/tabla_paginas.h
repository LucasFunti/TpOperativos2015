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

int pid_matchear_tabla_paginas,pagina_matchear_tabla_paginas;

void tabla_paginas_aniadir_item(int pid, int pagina, int marco);

marco tabla_paginas_buscar(int pid, int pagina,t_config * configuraciones);

bool coincide_pid_y_pagina_tabla_paginas(void * data);


#endif /* TABLA_PAGINAS_H_ */
