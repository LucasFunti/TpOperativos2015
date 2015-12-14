/*
 * retardo.c
 *
 *  Created on: 6/11/2015
 *      Author: utnso
 */
#include "retardo.h"

void retardo() {

	sleep(config_get_int_value(memoriaConfig, "RETARDO_MEMORIA"));

}
