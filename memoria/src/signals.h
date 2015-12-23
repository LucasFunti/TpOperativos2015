/*
 * signals.h
 *
 *  Created on: 4/11/2015
 *      Author: utnso
 */

#ifndef SRC_SIGNALS_H_
#define SRC_SIGNALS_H_
#include <signal.h>
#include "loggear.h"
#include "conexiones.h"
#include <pthread.h>

void * vaciar_memoria_principal(void * data);
void * vaciar_tlb(void * data);
void * volcar_memoria_principal(void * data);

void atender_seniales(int signal);

void registrarSeniales();

#endif /* SRC_SIGNALS_H_ */
