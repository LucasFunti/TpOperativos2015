/*
 * estructuras.h
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <commons/collections/list.h>

typedef struct {
	int codigo_operacion;
	int tamanio_data;

} t_header;

typedef struct {

	t_header * header;
	void * data;

} t_data;

/**
 * @NAME: t_tabla_paginas_item
 * @DESC: La tabla de páginas es una lista
 * de estas
 */
typedef struct {
	int pid;
	t_list * pares_pagina_marco;
} t_tabla_paginas_item;

/**
 * @NAME: t_par_pagina_marco
 * @DESC: Es un par pagina-marco
 */
typedef struct {
	int pagina;
	int marco;
} t_par_pagina_marco;

/**
 * @NAME: t_tlb_item
 * @DESC: El TLB es una lista de estas
 */
typedef struct {
	int pid;
	t_par_pagina_marco * par_pagina_marco;
} t_tlb_item;

/**
 * @DESC: Me ayuda a ver que es un HIT/MISS
 */
typedef int hit_miss;

/**
 * @DESC: Me ayuda para ver poder pedir punteros de tlb o
 * tabla de páginas y olvidarme como están implementadas
 */
typedef t_list t_tlb;
typedef t_list t_tabla_paginas;

#endif /* ESTRUCTURAS_H_ */
