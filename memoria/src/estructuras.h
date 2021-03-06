/*
 * estructuras.h
 *
 *  Created on: 29/10/2015
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include <commons/collections/list.h>
#include <stddef.h>

typedef struct {
	int codigo_operacion;
	int tamanio_data;

} t_header;

typedef struct {

	t_header * header;
	void * data;

} t_data;

/**
 * @NAME: t_tabla_paginas_item
 * @DESC: La tabla de páginas es una lista
 * de estas
 */
typedef struct {
	int pid;
	int pagina;
	int marco;
	bool modificado;
	int numero_operacion;
	bool presencia;
	bool uso;
	bool puntero;
} t_item;

/**
 * @DESC: Me ayuda para ver poder pedir punteros de tlb o
 * tabla de páginas y olvidarme como están implementadas
 */
typedef t_list t_tlb;
typedef t_list t_tabla_paginas;

typedef struct {
	int pedidos;
	int aciertos;
} t_registro_tlb;

#endif /* ESTRUCTURAS_H_ */
