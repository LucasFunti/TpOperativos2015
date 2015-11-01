/*
 * tlb.h
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */

#ifndef TLB_H_
#define TLB_H_

#include <commons/collections/list.h>
#include <stdlib.h>
#include "constantes.h"
#include "estructuras.h"
#include "globals.h"

void tlb_remover(t_tlb *, int);
bool funcion_tlb_coincide_pid_remover(void *);
void funcion_tlb_liberar_item(void *);

/**
 * @DESC: Acá se guardan las variables que va
 * usar la función que le a las funciones de
 * find o remove sobre listas
 */
int pid_para_buscar_tlb, pagina_para_buscar_tlb;
int pid_para_remover_tlb;

/**
 * @NAME: tlb_buscar
 * @DESC: Busca en la tlb, devuelve HIT/MISS
 * y almacena el par pagina-marco en tlb_match
 */
hit_miss tlb_buscar(t_tlb *, int, int);

/**
 * @NAME: new_tlb_item
 * @DESC: constructor de una fila de la tlb
 * Se puede agregar el resultado a la tlb directamente
 */
t_tlb_item * new_tlb_item(int, int, int);

/**
 * @NAME: funcion_tlb_buscar_por_pid_y_pagina
 * @DESC: se la pasamos al list_find y devuelve true
 * si el elemento de la lista tiene el pid y la página
 * almacenados en las variables pid_para_buscar_tlb y
 * pagina_para_buscar_tlb
 */
bool funcion_tlb_buscar_por_pid_y_pagina(void *);

/**
 * @NAME: tlb_add
 * @DESC: Agrega un elemento al tlb
 */
void tlb_add(t_tlb *, t_tlb_item *);

#endif /* TLB_H_ */
