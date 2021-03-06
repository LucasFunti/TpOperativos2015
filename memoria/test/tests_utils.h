/*
 * tests_utils.h
 *
 *  Created on: 1/11/2015
 *      Author: utnso
 */

#ifndef TESTS_UTILS_H_
#define TESTS_UTILS_H_

#include <commons/config.h>
#include <cspecs/cspec.h>
#include <stdlib.h>
#include "../src/estructuras.h"
#include "../src/globals.h"
#include "../src/loggear.h"

#include <string.h>

void assert_tabla_paginas_item(t_item * item, int pid, int pagina,
		int marco, bool modificado,bool presencia) {

	should_ptr(item) not be null;
	should_int(item->pid) be equal to(pid);
	should_int(item->pagina) be equal to(pagina);
	should_int(item->marco) be equal to(marco);
	should_bool(item->modificado) be equal to(modificado);
	should_bool(item->presencia) be equal to(presencia);
}


#endif /* TESTS_UTILS_H_ */
