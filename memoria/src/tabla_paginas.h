/*
 * tabla_paginas.h
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */

#ifndef TABLA_PAGINAS_H_
#define TABLA_PAGINAS_H_

#include <stdlib.h>
#include <commons/collections/list.h>
#include "constantes.h"
#include "estructuras.h"
#include "globals.h"


void tabla_paginas_anadir_par_pagina_marco(t_tabla_paginas *, int, int, int);
t_tabla_paginas_item * tabla_paginas_buscar_pid(t_tabla_paginas *, int);
void tabla_paginas_anadir_pagina(int, int, int);
void tabla_paginas_remover(t_tabla_paginas *, int);
bool funcion_tabla_paginas_coincide_pid_remover(void *);
void funcion_tabla_paginas_liberar_item(void *);
/**
 * @DESC: Acá se guardan las variables que va
 * usar la función que le pasamos a list_find
 */
int pagina_para_buscar_tabla_paginas, pid_para_buscar_tabla_paginas;
int pid_para_remover_tabla_paginas;

/**
 * @NAME: tabla_paginas_buscar
 * @DESC: Busca en la tabla_paginas,
 * devuelve HIT/MISS y almacena el
 * par pagina-marco en tabla_paginas_match
 */
hit_miss tabla_paginas_buscar(t_tabla_paginas*, int, int);

/**
 * @NAME: funcion_par_pagina_marco_buscar_por_pagina  *
 * @NAME: funcion_tabla_paginas_buscar_por_pid
 *
 * @DESC: Se las pasamos al list_find. La segunda va a buscar
 * la fila de la tabla que tenga un pid concidente con
 * pid_para_buscar_tabla_paginas
 */
bool funcion_par_pagina_marco_buscar_por_pagina(void *);
bool funcion_tabla_paginas_buscar_por_pid(void *);

/**
 * @NAME: tabla_paginas_add
 * @DESC: Agrega un elemento a la tabla de páginas
 */
void tabla_paginas_add(t_tabla_paginas *, t_tabla_paginas_item *);

/**
 * @NAME: new_par_pagina_marco
 * @DESC: Constructor de un par pagina-marco
 */
t_par_pagina_marco * new_par_pagina_marco(int, int);

/**
 * @NAME: new_tabla_paginas_item
 * @DESC: constructor de una fila de la tabla de páginas.
 */
t_tabla_paginas_item * new_tabla_paginas_item(int, int, int);

/**
 * @NAME: marco_para_pagina
 * @DESC: Devuelve el marco dado una fila de la tlb
 * (esto significa que ya matcheo el pid), y la página
 * (va a buscar en la lista de pares)
 */
int marco_para_pagina(t_tabla_paginas_item *, int);

/**
 * @NAME: funcion_buscar_marco
 * @DESC: Función que le pasamos a list_find
 * para saber que par tiene el mismo numero de página
 * que está almacenado en pagina_para_buscar_tabla_paginas
 */
bool funcion_buscar_marco(void*);

#endif /* TABLA_PAGINAS_H_ */
