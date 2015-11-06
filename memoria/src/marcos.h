/*
 * marcos.h
 *
 *  Created on: 5/11/2015
 *      Author: utnso
 */

#ifndef MARCOS_H_
#define MARCOS_H_
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include "globals.h"

t_queue * marcos_asignados;

typedef struct {

	int numero_marco;
	bool disponible;

} t_marco;

t_list * marcos_disponibles;
int cantidad_marcos,tamanio_marcos;

bool esta_libre(void * data);
int cantidad_marcos_libres();

#endif /* MARCOS_H_ */
