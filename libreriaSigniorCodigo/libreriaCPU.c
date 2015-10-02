/*
 * libreriaCPU.c
 *
 *  Created on: 1/10/2015
 *      Author: utnso
 */
#include <commons/txt.h>
#include <sys/socket.h>
#include <libSockets.h>

int reconocerInstruccion(char *linea) {
	int codigoInstruccion;
	char *palabrasSeparadas;
	*palabrasSeparadas = string_split(linea, " ");
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

typedef struct {
	char *instruccion;
	int cantidadDePaginas;
				} t_instruccion;

t_instruccion empaquetar(char *instruccion, int cantidadDePaginas){

	t_instruccion instruccionEmpaquetada;
	instruccionEmpaquetada.instruccion = instruccion;
	instruccionEmpaquetada.cantidadDePaginas = cantidadDePaginas;

	return instruccionEmpaquetada;
}

char *serializarEmpaquetado( t_instruccion instruccionEmpaquetada) {
	size_t tamanio = strlen(instruccionEmpaquetada.instruccion);
	void *buffer = malloc((sizeof(char) * instruccionEmpaquetada->instruccion) + sizeof(int));
	memcpy(buffer, &instruccionEmpaquetada->instruccion, tamanio);
	memcpy(buffer + tamanio, instruccionEmpaquetada->cantidadDePaginas, sizeof(int));
	return buffer;
}

int ejecutar(char *linea, serverMemoria, serverPlanificador) {



	switch (reconocerInstruccion(*linea)) {

		case 4:
			char *array[3];
			array = string_split(*linea, " ");
			char *instruccion = array[0];
			int valor = array[1];
			t_instruccion paquete = empaquetar(instruccion, valor);
			send(serverMemoria, serializarEmpaquetado(paquete)), 0);
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;

	};
	return 0;
}
