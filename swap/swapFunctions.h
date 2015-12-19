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
#include <signiorCodigo/libSockets.h>
#include "swap.h"

t_config * swapConfig;

//Estructuras

typedef struct {
	int numeroPagina;
	int pid;
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

void setPages();

int getSwapPagesAmount();

int getSwapPagesSize();

char* getSwapFileName();

void markPage(int pageNumber, int pid);

void writePage(int absolutePageNumber, char *content);

int getBlankPages();

int getLargestContiguousSpace();

char* readPage(int pageNumber);

void evaluateAction(char* action);

int reserve(int pid, int amount);

int getProcessFirstPage(int pid);

int getProcessReservedSpace(int pid);

int checkSpaceAvailability(int amount);

void freeSpace(int pid);

void compact();

void fillRemainingSpace();

void copyPage(int from, int to);

void imprimir();

char* getPort();

t_data * pedirPaquete(int codigoOp, int tamanio, void * data);

char * serializarPaquete(t_data * unPaquete);

t_data * leer_paquete(int socket);

void common_send(int socket, t_data * paquete);

char * readProcessPage(int pid, int nPage);

void writeProcessPage(int pid, int nPage, char * content);

void setSwapConfig(char * path);

void retardo();
void retardoCompactacion();

#endif /* SWAPFUNCTIONS_H_ */
