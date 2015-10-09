/*
 * swapFunctions.h
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#ifndef SWAPFUNCTIONS_H_
#define SWAPFUNCTIONS_H_

typedef struct configuracionSwap{
	char * puerto;
	char * nombreSwap;
	int cantidadPaginas;
	int tamanioPagina;
	int retardoSwap;
	int retardoCompactacion;
} t_config_swap;

void setupSwap();
t_bitarray *readFile(char *file);

t_config_swap read_config_swap();

#endif /* SWAPFUNCTIONS_H_ */
