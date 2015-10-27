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
#include "planificadorFunctions.h"
#include <stdlib.h>
#include <string.h>
#include "libSockets.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

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
	} else if (!strcmp(palabraClave, "finalizar")) {
		codigoInstruccion = 8;
	}
	return codigoInstruccion;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* empaquetado para todas las instrucciones salvo la escritura. */
t_instruccion empaquetar(char *instruccionRecibida, char *paginas) {
	t_instruccion instruccionEmpaquetada;
	instruccionEmpaquetada.instruccion = instruccionRecibida;
	instruccionEmpaquetada.cantidadDePaginas = paginas;


	return instruccionEmpaquetada;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* empaquetado para la instruccion de escritura. */
t_instruccionEscritura empaquetarEscritura(char *instruccionRecibida,
		char *paginas, char *texto) {
	t_instruccionEscritura escrituraEmpaquetada;
	escrituraEmpaquetada.instruccion = instruccionRecibida;
	escrituraEmpaquetada.cantidadDePaginas = paginas;
	escrituraEmpaquetada.textoAEscribir = texto;

	return escrituraEmpaquetada;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* serializacion para todas las instrucciones salvo la de escritura.*/
char *serializarEmpaquetado(t_instruccion instruccionEmpaquetada) {
	size_t numeroDePaginas;
	char *paginas = instruccionEmpaquetada.cantidadDePaginas;
	string_append(&paginas, "\0");
	numeroDePaginas = strlen(paginas);
	void *buffer = malloc((sizeof(char) * numeroDePaginas));
	memcpy(buffer, paginas, numeroDePaginas);
	return buffer;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* serializacion para la escritura */
char *serializarEmpaquetadoEscritura(t_instruccionEscritura instruccionEmpaquetada) {
	size_t numeroDePaginas, tamanioTexto;
	char *paginas = instruccionEmpaquetada.cantidadDePaginas;
	char *texto = instruccionEmpaquetada.textoAEscribir;
	string_append(&paginas, "\0");
	string_append(&texto, "\0");
	numeroDePaginas = strlen(paginas);
	tamanioTexto = strlen(texto);
	void *buffer = malloc(
			(sizeof(char) * numeroDePaginas) + (sizeof(char) * tamanioTexto));
	memcpy(buffer, paginas, numeroDePaginas);
	memcpy(buffer + numeroDePaginas, texto, tamanioTexto);
	return buffer;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Función para pasar un archivo .txt a un string. */
char *txtAString(char *rutaDelArchivo) {
	char * buffer = 0;
	long length;
	FILE * f = fopen(rutaDelArchivo, "rb");
	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = malloc(length);
		if (buffer) {
			fread(buffer, 1, length, f);
		}
		fclose(f);
	}
	return buffer;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* funcion para ejecutar las instrucciones */
int ejecutar(char *linea, int serverMemoria, int serverPlanificador,
		int idProceso) {

	char *estadoDeEjecucion;
	char *contenidoDePagina = malloc(256); /* ver tamañano real mas adelante. */
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
		send(serverMemoria, &clave, sizeof(int), 0);
		send(serverMemoria, paqueteSerializado, sizeof(paqueteSerializado), 0);
//		recv(serverMemoria, &estadoDeEjecucion, sizeof(int), 0);

		printf("ingrese el estado de la ejecucion:\n"); /* unicamente para testear */
		estadoDeEjecucion = malloc (sizeof(char) * 3);	/* unicamente para testear */
		scanf("%s", estadoDeEjecucion);					/* unicamente para testear */

		if (strcmp(estadoDeEjecucion, "0") == 0) {
			printf("error al iniciar el proceso %d\n", idProceso);
		} else if (strcmp(estadoDeEjecucion, "1") == 0){
			printf("proceso %d iniciado correctamente!\n", idProceso);
		};
		break;

	case 5:
		array = string_split(linea, " ");
		paquete = empaquetar(array[0], array[1]);
		paqueteSerializado = serializarEmpaquetado(paquete);
		send(serverMemoria, &clave, sizeof(int), 0);
		send(serverMemoria, paqueteSerializado, sizeof(paqueteSerializado), 0);
//		recv(serverMemoria, contenidoDePagina, 256, 0); /*que el administrador sólo envíe el contenido!!! */

		contenidoDePagina = "texto de prueba"; /* únicamente para testear */

		printf("proceso: %d, pagina: %s, leida: %s\n", idProceso, array[1],
				contenidoDePagina);
		break;

	case 6:
		array = string_n_split(linea, 3, " ");
		paqueteEscritura = empaquetarEscritura(array[0], array[1], array[2]);
		paqueteSerializado = serializarEmpaquetadoEscritura(paqueteEscritura);
		send(serverMemoria, &clave, sizeof(int), 0);
		send(serverMemoria, paqueteSerializado, sizeof(paqueteSerializado), 0);
//		recv(serverMemoria, contenidoDePagina, sizeof(int), 0);

		contenidoDePagina = array[2]; /* unicamente para testear */

		printf("proceso %d, Página %s, escrita: %s\n", idProceso, array[1],
				contenidoDePagina);
		break;

	case 7:
		array = string_split(linea, " ");
		paquete = empaquetar(array[0], array[1]);
		paqueteSerializado = serializarEmpaquetado(paquete);
		send(serverPlanificador, &clave, sizeof(int), 0);
		send(serverPlanificador, paqueteSerializado, sizeof(paqueteSerializado),
				0);

		printf("proceso %d en entrada-salida de tiempo %s\n", idProceso, array[1]);

		break;

	case 8:
		send(serverMemoria, &clave, sizeof(int), 0);
		send(serverMemoria, &idProceso, sizeof(int), 0);
		printf("proceso %d finalizado.\n", idProceso);
		break;

	};
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* The real deal. Esta función va a ser la que reciba la ruta y el contador de programa, y
 * la que ejecute línea por línea las instrucciones desde donde lo indique el contador.*/

void correrArchivo(tipo_pcb pcb, int serverMemoria, int serverPlanificador) {
	t_resultadoEjecucion resultado;
	char **listaInstrucciones;
	int n, retardo;
	char *archivoEnStrings;
	archivoEnStrings = txtAString(pcb.nombrePrograma);
	listaInstrucciones = string_split(archivoEnStrings, ";\n");
	n = pcb.programCounter;
	retardo = getRetardo();
	while (listaInstrucciones[n] != NULL) {
		ejecutar(listaInstrucciones[n], serverMemoria, serverPlanificador,
				pcb.id);
		n++;
		sleep(retardo);
		}
	resultado.nombrePrograma = pcb.nombrePrograma;
	resultado.programCounter = n;
	enviarResultado(resultado, serverPlanificador);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*getters para cada campo del archivo de configuración. */
char *getIpPlanificador() {
	t_config *cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * ip = config_get_string_value(cpuConfig, "IP_PLANIFICADOR");
	return ip;
}

char *getPuertoPlanificador() {
	t_config *cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * puerto = config_get_string_value(cpuConfig, "PUERTO_PLANIFICADOR");
	return puerto;
}

char *getIpMemoria() {
	t_config *cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * ip = config_get_string_value(cpuConfig, "IP_MEMORIA");
	return ip;
}

char *getPuertoMemoria() {
	t_config *cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * puerto = config_get_string_value(cpuConfig, "PUERTO_MEMORIA");
	return puerto;
}
int getHilos() {
	t_config *cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	int hilos = config_get_int_value(cpuConfig, "CANTIDAD_HILOS");
	return hilos;
}

int getRetardo() {
	t_config *cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	int retardo = config_get_int_value(cpuConfig, "RETARDO");
	return retardo;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* funcion para pasarle al pthread_create. */
void *iniciarcpu() {
	char *ip_Planificador, ip_Memoria, puerto_Planificador, puerto_Memoria;
	int serverPlanificador, serverMemoria;
	ip_Planificador = getIpPlanificador();
	ip_Memoria = getIpMemoria();
	puerto_Planificador = getPuertoPlanificador();
	puerto_Memoria = getPuertoMemoria();

	serverPlanificador = conectarCliente(ip_Planificador, puerto_Planificador);
	serverMemoria = conectarCliente(ip_Memoria, puerto_Memoria);

	/*char package[PACKAGESIZE];*/
	int status = 1;

	while (status != 0) {
		char * buffer = malloc(sizeof(int) * 3);
		status = recv(serverPlanificador, buffer, (sizeof(int) * 3), 0);
		Paquete *contexto_ejecucion;
		contexto_ejecucion = deserializar_header(buffer);
		free(buffer);
		//estos printf son probar que ande nada mas
//		printf("El codigo de la operacion es:%d \n",
//				contexto_ejecucion->codigoOperacion);
//		printf("El PC es:%d \n", contexto_ejecucion->programCounter);
//		printf("El tamanio a recivir es:%d\n", contexto_ejecucion->tamanio);
		if (status != 0) {
			char * dataBuffer = malloc(contexto_ejecucion->tamanio);
			recv(serverPlanificador, dataBuffer, contexto_ejecucion->tamanio,
					0);
			contexto_ejecucion->mensaje = malloc(contexto_ejecucion->tamanio);
			memcpy(contexto_ejecucion->mensaje, dataBuffer,
					contexto_ejecucion->tamanio);
			free(dataBuffer);
			printf("el mensaje es:%s \n", contexto_ejecucion->mensaje);
			destruirPaquete(contexto_ejecucion);

			char *rutaDelArchivo;
			int contadorDePrograma;
			int idProceso;
			char **array = string_split(contexto_ejecucion->mensaje, " ");
			rutaDelArchivo = array[0];
			contadorDePrograma = atoi(array[1]);
			idProceso = atoi(array[2]);

//			correrArchivo(pcb, serverMemoria,
//					serverPlanificador, idProceso); /* la dejo en stand-by hasta que definamos si mandamos un contexto, la pcb o
//														ambas cosas. */

		}

	}
	/*send(serverMemoria, package, sizeof(package), 0);*/

	close(serverPlanificador);
	close(serverMemoria);

	return NULL;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void enviarResultado(t_resultadoEjecucion resultado, int serverMemoria){
	char *resultadoSerializado = serializarResultado(resultado);
	send(serverMemoria, resultadoSerializado, sizeof(resultadoSerializado), 0);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *serializarResultado(t_resultadoEjecucion resultado){
	size_t tamanioContador, tamanioTexto;
	char* contadorDePrograma = string_itoa(resultado.programCounter);
	char* programa = resultado.nombrePrograma;
	string_append(contadorDePrograma, "\0");
	string_append(programa, "\0");
	tamanioContador = strlen(contadorDePrograma);
	tamanioTexto = strlen(programa);
	void *buffer = malloc((sizeof(char) * tamanioContador) + (sizeof(char) * tamanioTexto));
	memcpy(buffer, programa, tamanioTexto);
	memcpy(buffer + tamanioTexto, contadorDePrograma, tamanioContador);
	return buffer;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
