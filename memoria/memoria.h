/*
 * memoria.h
 *
 *  Created on: 28/10/2015
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>


void levantarConfiguracion();
void loggearInfo(char *);
void loggearError(char * );
void loggearWarning(char *);
void iniciarLogger();
void atenderConexiones();
void atenderSignals();


#endif /* MEMORIA_H_ */
