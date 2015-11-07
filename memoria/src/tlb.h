/*
 * tlb.h
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */

#ifndef TLB_H_
#define TLB_H_

#include <commons/config.h>
#include <stdlib.h>
#include "tabla_paginas.h"
#include "estructuras.h"

int pid_matchear_tlb, pagina_matchear_tlb;

/**
 * tlb_remover_pid
 */
void tlb_remover_pid(int pid, t_config * configuraciones,bool es_test);

bool coincide_pid_tlb(void * data);

bool coincide_pid_tlb_y_actualiza_tabla_paginas_modificado(void * data);

/**
 * tlb_sacar_entrada
 */

void tlb_sacar_entrada(int pid, int pagina, t_config * configuraciones);

bool tlb_habilitada(t_config * configuraciones);

bool coincide_pid_y_pagina(void * data);

/**
 * tlb_buscar
 */

marco tlb_buscar(int pid, int pagina, t_config * configuraciones,
bool es_escritura);

/**
 * tlb_agregar_entrada
 */

void tlb_agregar_entrada(int pid, int pagina, int marco,
		t_config * configuraciones, bool es_escritura);

#endif /* TLB_H_ */
