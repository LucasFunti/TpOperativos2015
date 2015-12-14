/*
 * swap.h
 *
 *  Created on: 5/11/2015
 *      Author: utnso
 */

#ifndef SWAP_H_
#define SWAP_H_

#include "conexiones.h"
#include "constantes.h"
#include  "marcos.h"

bool swap_iniciar(int pid, int cantidad_paginas);
void swap_escribir(int pid, int pagina, int marco);
char * swap_leer(int pid, int pagina);
void swap_finalizar(int pid);

#endif /* SWAP_H_ */
