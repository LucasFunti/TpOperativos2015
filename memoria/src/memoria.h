/*
 * memoria.h
 *
 *  Created on: 28/10/2015
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <pthread.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <unistd.h>
#include "signals.h"
#include "conexiones.h"
#include "globals.h"
#include "constantes.h"

char * puertoCpu;
char * ipSwap;
char * puertoSwap;

void atenderConexion(int socket, fd_set sockets_activos);

void funcion_imprimir_elemento(char * key, void * value);
void levantarConfiguracion();

void atenderConexiones();

#endif /* MEMORIA_H_ */
