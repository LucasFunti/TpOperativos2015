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

void swapInit(t_list *pages);

void setupSwap();

int doesFileExist(const char *filename);

t_list* setPages(int pagesAmount);

int getSwapListenPort();

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

void deSerialize(void* element,void* buffer, size_t size);

void serialize(void* element,void* buffer, size_t size);

#endif /* SWAPFUNCTIONS_H_ */
