/*
 * swapFunctions.h
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#ifndef SWAPFUNCTIONS_H_
#define SWAPFUNCTIONS_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <signiorCodigo/libSockets.h>

//Estructuras

typedef struct {
	int numeroPagina;
	char *nombreProceso;
} t_nodo_swap;

typedef struct {
	int codigo_operacion;
	int tamanio_data;

} t_header;

typedef struct {

	t_header * header;
	void * data;

} t_data;

//Metodos


void setupSwap();

int doesFileExist(const char *filename);

t_list* setPages(int pagesAmount);

int getSwapPagesAmount();

int getSwapPagesSize();

char* getSwapFileName();

void markPage(int pageNumber,char *processName, t_list *pages);

void writePage(int pageNumber,char *content);

char* readPage(int pageNumber);

void evaluateAction(char* action, t_list *pages);

int reserve(char* name, int amount, t_list *pages);

int checkProcessSpace(char* name, t_list *pages);

int checkSpaceAvailability(int amount, t_list *page);

void freeSpace(char *name,t_list *pages);

void compact(t_list *pages);

void fillRemainingSpace(t_list *list, int from);

void copyPage(int from, int to);

void imprimir(t_list *list);

char* getPort();

t_data * pedirPaquete(int codigoOp, int tamanio, void * data);

char * serializarPaquete(t_data * unPaquete);

t_data * leer_paquete(int socket);

void common_send(int socket, t_data * paquete) ;


#endif /* SWAPFUNCTIONS_H_ */
