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
#include <stdio.h>
#include "tabla_paginas.h"
#include "estructuras.h"
#include "globals.h"
#include "funciones_listas.h"

/**
 * tlb_remover_pid
 */
void tlb_remover_pid(int pid);

bool tlb_habilitada();

/**
 * tlb_buscar
 */

t_item * tlb_buscar(int pid, int pagina);

/**
 * tlb_agregar_entrada
 */

void tlb_agregar_item(t_item * item);

void * mostrarTasaTlb(void * data);

#endif /* TLB_H_ */
