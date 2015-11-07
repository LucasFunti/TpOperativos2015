/*
 * retardo.c
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */
#include "retardo.h"

void retardo(t_config * configuracion_retardo) {

	sleep(config_get_int_value(configuracion_retardo, "RETARDO_MEMORIA"));

}
