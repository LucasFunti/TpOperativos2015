/*
 * globals.h
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/dictionary.h>
#include <stdbool.h>
#include "estructuras.h"

void * memoria;

t_log * logger;

/**
 * @DESC: Archivo de configuración
 * Lo pongo acá para poder usar desde el test
 */
t_config * memoriaConfig;

/**
 * @NAME: diccionario de configuraciones
 * @DESC: Tiene todos los valores del
 * archivo de configuración
 */
t_dictionary * diccionario_configuraciones;

/**
 * @NAME: variables principales
 * @DESC: son la TLB y la Tabla de Páginas
 * que usa la memoria
 */
t_tlb * tlb;
t_tabla_paginas * tabla_paginas;

/**
 * @NAME: variables de match
 * @DESC: Acá van a devolver las búsquedas el par
 * página-marco en caso de hacer HIT
 */
t_par_pagina_marco * tabla_paginas_match;
t_par_pagina_marco * tlb_match;

#endif /* GLOBALS_H_ */
