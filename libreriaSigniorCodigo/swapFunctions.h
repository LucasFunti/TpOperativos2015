/*
 * swapFunctions.h
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#ifndef SWAPFUNCTIONS_H_
#define SWAPFUNCTIONS_H_

//Estructuras

typedef struct {
	int numeroPagina;
	char *nombreProceso;
} t_nodo_swap;


//Metodos


void setupSwap();

void listDestroy(t_list *pages);

int doesFileExist(const char *filename);

t_list* setPages(int pagesAmount);

int getSwapPagesAmount();

int getSwapPagesSize();

char* getSwapFileName();

void markPage(int pageNumber,char *processName, t_list *pages);

void writePage(int pageNumber,char *content);

char* readPage(int pageNumber);

void evaluateAction(char* action, t_list *pages);

void reserve(char* name, int amount, t_list *pages);

int checkProcessSpace(char* name, t_list *pages);

int checkSpaceAvailability(int amount, t_list *page);

void freeSpace(char *name,t_list *pages);

void compact(t_list *pages);

void fillRemainingSpace(t_list *list, int from);

void copyPage(int from, int to);

void imprimir(t_list *list);

#endif /* SWAPFUNCTIONS_H_ */
