/*
 * funciones_listas.h
 *
 *  Created on: 14/12/2015
 *      Author: utnso
 */

#ifndef FUNCIONES_LISTAS_H_
#define FUNCIONES_LISTAS_H_

#include "estructuras.h"
#include "globals.h"
#include "marcos.h"

bool coincide_pid(void * data);
bool coincide_pid_y_pagina(void * data);
bool coincide_pid_y_esta_presente(void * data);
bool coincide_pid_y_libera_marcos(void * data);

bool comparador_primero_menor_n_operacion(void * data1, void* data2);

#endif /* FUNCIONES_LISTAS_H_ */
