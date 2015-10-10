/*
 * libreriaCPU.c
 *
 *  Created on: 1/10/2015
 *      Author: utnso
 */
#include <commons/txt.h>
#include <commons/string.h>
#include <commons/config.h>
#include <sys/socket.h>
#include "libreriaCPU.h"
#include <stdlib.h>
#include <string.h>




/* recibe la instrucción y devuelve un identificador para poder usar en el switch. */

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

/* empaquetado para todas las instrucciones salvo la escritura. */
t_instruccion empaquetar(char *instruccionRecibida, int paginas){
	t_instruccion instruccionEmpaquetada;

	return instruccionEmpaquetada;
}

t_instruccionEscritura empaquetarEscritura(char *instruccionRecibida, int paginas, char *texto){
	t_instruccionEscritura escrituraEmpaquetada;

	return escrituraEmpaquetada;
}

/* serializacion para todas las instrucciones salvo la de escritura.*/
char *serializarEmpaquetado(t_instruccion instruccionEmpaquetada) {
	size_t tamanio;
	tamanio = strlen(instruccionEmpaquetada.instruccion);
	void *buffer = malloc((sizeof(char) * tamanio) + sizeof(int));
	memcpy(buffer, instruccionEmpaquetada.instruccion, tamanio);
	memcpy(buffer + tamanio, &instruccionEmpaquetada.cantidadDePaginas,
			sizeof(int));
	return buffer;
}

int ejecutar(char *linea, int serverMemoria, int serverPlanificador,
		int idProceso) {

	int estadoDeEjecucion;
	char *contenidoDePagina;
	char **array;
	t_instruccion paquete;
	t_instruccionEscritura paqueteEscritura;
	char *paqueteSerializado;
	int clave = reconocerInstruccion(linea);

	switch (clave) {

	case 4:
		array = string_split(linea, " ");
		paquete = empaquetar(array[0], array[1]);
		paqueteSerializado = serializarEmpaquetado(paquete);

		send(serverMemoria, paqueteSerializado, sizeof(paqueteSerializado), 0);
		recv(serverMemoria, &estadoDeEjecucion, sizeof(int), 0);
		if (estadoDeEjecucion == 0) {
			printf("error al iniciar el proceso %d\n", idProceso);
		} else {
			printf("proceso %d iniciado correctamente!\n", idProceso);
		};
		break;

	case 5:
		array = string_split(linea, " ");
		paquete = empaquetar(array[0], array[1]);
		paqueteSerializado = serializarEmpaquetado(paquete);

		send(serverMemoria, paqueteSerializado, sizeof(paqueteSerializado), 0);
		recv(serverMemoria, &contenidoDePagina, sizeof(int), 0);
		printf("proceso: %d, pagina: %d, leida: %s", idProceso, array[1],
				contenidoDePagina);
		break;

	case 6:
		array = string_split(linea, " ");
		paqueteEscritura = empaquetar_escritura(array[0], array[1], array[2]);

		send(serverMemoria, paqueteSerializado, sizeof(paqueteSerializado), 0);
		recv(serverMemoria, &contenidoDePagina, sizeof(int), 0);
		printf("proceso %d, -Página %d, escrita: %s ", idProceso, array[1],
				contenidoDePagina);
		break;

	case 7:

		send(serverPlanificador, paqueteSerializado, sizeof(paqueteSerializado),
				0);
		break;

	case 8:
		array = string_split(linea, " ");
		t_instruccion paquete = empaquetar(array[0], array[1]);
		char *paqueteSerializado = serializarEmpaquetado(paquete);

		send(serverMemoria, paqueteSerializado, sizeof(paqueteSerializado), 0);
		printf("proceso %d finalizado.", idProceso);
		break;

	};
	return 0;
}

/* genera una estructura para guardar todos los datos del archivo de config */
t_config_cpu read_config_cpu_file() {
	t_config_cpu config_cpu;
	t_config * cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	config_cpu.ipPlanificador = config_get_string_value(cpuConfig,
			"IP_PLANIFICADOR");
	config_cpu.puerto_planificador = config_get_string_value(cpuConfig,
			"PUERTO_PLANIFICADOR");
	config_cpu.ipMemoria = config_get_string_value(cpuConfig, "IP_MEMORIA");
	config_cpu.puerto_memoria = config_get_string_value(cpuConfig,
			"PUERTO_MEMORIA");
//	config_cpu.cantidadHilos = config_get_int_value(cpuConfig,"CANTIDAD_HILOS");
//	config_cpu.retardo = config_get_int_value(cpuConfig,"RETARDO");
	return config_cpu;
}
