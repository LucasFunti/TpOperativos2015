/*
 * pedidos.h
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */

#ifndef PEDIDOS_H_
#define PEDIDOS_H_

#include <stdlib.h>
#include <string.h>
#include <commons/collections/dictionary.h>
#include "constantes.h"
#include "globals.h"
#include "tlb.h"
#include "tabla_paginas.h"
#include "marcos.h"
#include "spawn.h"


/**
 * Variables
 */
int encontrado;

void crear_estructura_para_proceso(int pid, int cantidad_paginas);

void remover(int);

void remover_tlb(t_tlb * tabla, int pid);
void remover_tabla_paginas(t_tabla_paginas * tabla_paginas,int pid);
void remover_swap(int);

/**
 * Principales
 */

bool iniciar_n(int pid,int cantidad_paginas);
char * leer_n(int pid, int pagina);
void escribir_n(char * texto_para_escribir);
void finalizar(int pid,t_dictionary * diccionario);

/**
 * Secundarios
 */
int buscar_tlb(t_tlb * tabla,t_tabla_paginas * tabla_pags,int pid, int pagina);

/**
 * @NAME: buscar_tlb
 * @DESC: Delega en tlb_buscar
 * Busca en la tlb, la tabla y
 * por último en swap. Devuelve
 * el numero de marco
 */

/**
 * @NAME: buscar_tabla
 * @DESC: Delega en tabla_paginas_buscar
 * Busca en la tlb, la tabla y
 * por último en swap. Devuelve
 * el numero de marco
 */
int buscar_tabla(t_tabla_paginas * tabla_pags,int pid, int pagina);

/**
 * @NAME: buscar_swap
 * @DESC: Deberia pedirlo al swap, con todo lo que eso implica
 */
int buscar_swap(int pid, int pagina);

/**
 * @NAME: marco_pagina
 * @DESC: Devuelve el numero de marco donde está
 * la página del proceso pedido
 */
int marco_pagina(t_tlb * tabla_tlb ,t_tabla_paginas * tabla_pags,t_dictionary * diccionario,int pid, int pagina);

/**
 * @NAME: tlb_habilitada
 * @DESC: devuelve true si la el archivo de configuración
 * dice algo distinto a NO
 */
bool tlb_habilitada(t_dictionary * diccionario);


#endif /* PEDIDOS_H_ */
