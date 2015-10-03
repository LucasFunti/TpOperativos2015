/*
 * libreriaCPU.c
 *
 *  Created on: 1/10/2015
 *      Author: utnso
 */
#include <commons/txt.h>
#include <commons/string.h>
#include <sys/socket.h>
#include "libreriaCPU.h"
#include <stdlib.h>
#include <string.h>

int reconocerInstruccion(char *linea) {
	int codigoInstruccion;
	char **palabrasSeparadas;
	palabrasSeparadas = string_split(linea, " ");
	char *palabraClave;
	palabraClave = palabrasSeparadas[0];

	if (!strcmp(palabraClave, "iniciar")) {
		codigoInstruccion = 4;
	} else if (!strcmp(palabraClave, "leer")) {
		codigoInstruccion = 5;
	} else if (!strcmp(palabraClave, "escribir")) {
		codigoInstruccion = 6;
	} else if (!strcmp(palabraClave, "entrada-salida")) {
		codigoInstruccion = 7;
	} else
		codigoInstruccion = 8;
	return codigoInstruccion;
}

t_instruccion empaquetar(char *instruccion, int cantidadDePaginas) {

	t_instruccion instruccionEmpaquetada;
	instruccionEmpaquetada.instruccion = instruccion;
	instruccionEmpaquetada.cantidadDePaginas = cantidadDePaginas;

	return instruccionEmpaquetada;
}

char *serializarEmpaquetado(t_instruccion instruccionEmpaquetada) {
	size_t tamanio;
	tamanio = strlen(instruccionEmpaquetada.instruccion);
	void *buffer = malloc((sizeof(char) * tamanio) + sizeof(int));
	memcpy(buffer, instruccionEmpaquetada.instruccion, tamanio);
	memcpy(buffer + tamanio, instruccionEmpaquetada.cantidadDePaginas, sizeof(int));
	return buffer;
}

int ejecutar(char *linea, int serverMemoria, int serverPlanificador, int idProceso) {

	int estadoDeEjecucion;
	int contenidoDePagina;

	/* separo la primera palabra de la linea del valor que la acompaña, empaqueto ambas y las serializo. */
	char **array;
	array = string_split(linea, " ");
	char *instruccion = array[0];
	int valor = array[1];
	t_instruccion paquete = empaquetar(instruccion, valor);
	char *paqueteSerializado = serializarEmpaquetado(paquete);

	/* Interpreto la instruccion, la envío a quien corresponda, y dado el caso,notifico si
	 * la ejecución fue correcta. */
	int clave = reconocerInstruccion(linea);

	switch (clave) {

	case 4:
		send(serverMemoria, paqueteSerializado, sizeof(paqueteSerializado), 0);
		recv(serverMemoria, estadoDeEjecucion, sizeof(int), 0);
		if(estadoDeEjecucion == 0){
			printf("error al iniciar el proceso %d\n", idProceso);
		}
		else{
			printf("proceso %d iniciado correctamente!\n", idProceso);
		};
		break;

	case 5:
		send(serverMemoria, paqueteSerializado, sizeof(paqueteSerializado), 0);
		recv(serverMemoria, contenidoDePagina, sizeof(int), 0);
		printf("proceso %d", idProceso, "página %d", valor, "leida: %s", contenidoDePagina);
		break;

	case 6:
		send(serverMemoria, paqueteSerializado, sizeof(paqueteSerializado), 0);
		recv(serverMemoria, contenidoDePagina, sizeof(int), 0);
		printf("proceso %d", idProceso, " -Página %d", valor, "escrita: %s", contenidoDePagina);
		break;

	case 7:
		send(serverPlanificador, paqueteSerializado, sizeof(paqueteSerializado), 0);
		break;

	case 8:
		send(serverMemoria, paqueteSerializado, sizeof(paqueteSerializado), 0);
		printf("proceso %d", idProceso, "finalizado.");
		break;

	};
	return 0;
}
