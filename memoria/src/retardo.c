/*
 * retardo.c
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */
#include "retardo.h"

void retardo() {

	log_info(logger, "Acceso a memoria. Zzz...");
	sleep(config_get_int_value(memoriaConfig, "RETARDO_MEMORIA"));

}
