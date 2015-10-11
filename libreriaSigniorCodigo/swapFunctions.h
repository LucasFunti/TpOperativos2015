/*
 * swapFunctions.h
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#ifndef SWAPFUNCTIONS_H_
#define SWAPFUNCTIONS_H_

void setupSwap();

int doesFileExist(const char *filename);

t_list* getPages(int pagesAmount);

typedef struct {
	int numeroPagina;
	char *nombreProceso;
} t_nodo_swap;

#endif /* SWAPFUNCTIONS_H_ */
