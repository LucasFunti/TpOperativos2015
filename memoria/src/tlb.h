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

/**
 * @DESC: Acá se guardan las variables que va
 * usar la función que le a las funciones de
 * find o remove sobre listas
 */

int pid_para_matchear_tlb, pagina_para_matchear_tlb;

/**
 * Principales
 */

void tlb_agregar_entrada(t_tlb * tlb, int pid, int pagina, int marco);
hit_miss tlb_buscar(t_tlb *, int, int);
void tlb_remover_entrada(t_tlb * tlb, int pid, int pagina);
void tlb_remover_entradas_para(t_tlb * tlb, int pid);

/**
 * Secundarias
 */
t_tlb_item * new_tlb_item(int, int, int);
void tlb_add(t_tlb *, t_tlb_item *);

/**
 * Auxiliares para operar con listas
 */

bool funcion_tlb_coincide_pid(void * data);
bool funcion_tlb_coincide_pagina(void * data);
bool funcion_tlb_buscar_por_pid_y_pagina(void * data);
void funcion_tlb_liberar_item(void *);

/**
 * @NAME: tlb_buscar
 * @DESC: Busca en la tlb, devuelve HIT/MISS
 * y almacena el par pagina-marco en tlb_match
 */

/**
 * @NAME: new_tlb_item
 * @DESC: constructor de una fila de la tlb
 * Se puede agregar el resultado a la tlb directamente
 */

/**
 * @NAME: tlb_add
 * @DESC: Agrega un elemento al tlb
 */

#endif /* TLB_H_ */
