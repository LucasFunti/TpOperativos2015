/*
 * memoria.h
 *
 *  Created on: 28/10/2015
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <commons/config.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <signiorCodigo/libSockets.h>
#include "globals.h"




char * puertoCpu;
char * ipSwap;
char * puertoSwap;

t_log * logger;
t_config * memoriaConfig;



pthread_t hiloConexiones, hiloSignals;

int socketEscucha, socketCliente;

void levantarConfiguracion();
void loggearInfo(char *);
void loggearError(char *);
void loggearWarning(char *);
void iniciarLogger();
void atenderConexiones();
void atenderSignals();
void lanzarHilos();
void atenderCliente(int);

#endif /* MEMORIA_H_ */
