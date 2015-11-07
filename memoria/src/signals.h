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

void vaciar_tabla_paginas(t_config * configuracion_vaciar_tabla_paginas);
void vaciar_tlb(t_config * configuracion_vaciar_tlb);
void volcar_memoria_principal(t_config * configuracion_volcar);

void atender_seniales(int signal);

void registrarSeniales();



#endif /* SRC_SIGNALS_H_ */
