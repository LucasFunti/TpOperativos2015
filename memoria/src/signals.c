/*
 * signals.c
 *
 *  Created on: 4/11/2015
 *      Author: utnso
 */
#include "signals.h"

void registrarSeniales() {

	signal(SIGUSR1, atender_vaciar_tlb);
	signal(SIGUSR2, atender_vaciar_tabla_paginas);
	//Falta la tercer señal
	loggearInfo(string_from_format("Ya se atiende la señal SIGUSR1 (%d), SIGUSR2 (%d) y **** ",SIGUSR1,SIGUSR2));

}

void atender_vaciar_tlb(int signal_usr1) {
	loggearInfo("Se registró la señal para vaciar la TLB");

}

void atender_vaciar_tabla_paginas(int signal_usr2) {
	loggearInfo("Se registró la señal para vaciar la tabla de páginas");
}
