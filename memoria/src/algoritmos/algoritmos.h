/*
 * algoritmos.h
 *
 *  Created on: 11/11/2015
 *      Author: utnso
 */

#ifndef SRC_ALGORITMOS_ALGORITMOS_H_
#define SRC_ALGORITMOS_ALGORITMOS_H_

#include "../globals.h"
#include <commons/collections/list.h>
#include "../swap.h"

int fifo();

int lru();
void encontrar_menor_numero_operacion(void * data);
int posicion_lru, posicion_del_menor_lru;
int minimo_numero_operacion;

int clock_m();
bool no_esta_modificado(void * data);
t_tabla_paginas_item * elemento_del_puntero_clock_m;
int contador_auxiliar_indice_clock_m;
bool analizar;
void actualizar_puntero_clock(int maximo_indice);
bool es_el_del_puntero_o_posterior_y_no_modificado(void * data);
bool es_el_del_puntero(void * data);

#endif /* SRC_ALGORITMOS_ALGORITMOS_H_ */