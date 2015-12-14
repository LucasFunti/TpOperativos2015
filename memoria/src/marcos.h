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
#include "funciones_listas.h"

typedef struct {

	int numero_marco;
	bool disponible;

} t_marco;

t_list * marcos_disponibles;

void iniciar_marcos();
int marcos_libres();
int marco_disponible();
bool tiene_marcos_asignados(int pid);
int marcos_asignados(int pid);
bool esta_libre(void * data);


#endif /* MARCOS_H_ */
