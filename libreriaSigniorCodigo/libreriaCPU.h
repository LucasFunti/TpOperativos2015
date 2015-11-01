/*
 * libreriaCPU.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef LIBRERIACPU_H_
#define LIBRERIACPU_H_
#include "planificadorFunctions.h"

typedef struct instruccion{
	char *instruccion;
	char *cantidadDePaginas;
} t_instruccion;

typedef struct instruccionEscritura{
	char *instruccion;
	char *cantidadDePaginas;
	char *textoAEscribir;
} t_instruccionEscritura;

typedef struct {
	int codigoOperacion;
	int punteroAInstruccion;
	int tamanio;
	char *mensaje;
} t_tablaDeInstruccion;

typedef struct{
	char * ipPlanificador;
	char * puerto_planificador;
	char * ipMemoria;
	char * puerto_memoria;
	int cantidadHilos;
	int retardo;
} t_config_cpu;

typedef struct{
	int idHilo;
	t_log *logger;
} t_hilo;

typedef struct{
	char *nombrePrograma;
	int programCounter;
	char *resultadosSerializados;
} t_resultadoEjecucion;

typedef struct{
	int header;
	int tamanio;
	char * mensaje;
} t_mensaje;

typedef struct{
	int m; 				// slots del array usados hasta ahora
	int maximo; 		// capacidad máxima del array
	int contador;		// posición actualizada del contador de programa
	int *data;			// el array de resultados de las ejecuciones de cada instrucción
} t_resultadoOperacion;


int reconocerInstruccion(char*);
t_instruccion empaquetar(char *, char *);
t_instruccionEscritura empaquetarEscritura(char *, char *, char *);
char *serializarEmpaquetado(t_instruccion instruccionEmpaquetada);
char *serializarEmpaquetadoEscritura(t_instruccionEscritura instruccionEmpaquetada);
int ejecutar(char *linea, int serverMemoria,int serverPlanificador, char *idProceso);
t_resultadoOperacion correrArchivo(char *ruta, int contadorPrograma, char* idProceso, int serverMemoria, int serverPlanificador);
char *getIpPlanificador();
void *iniciarcpu(t_hilo);
char *getIpPlanificador();
char *getPuertoPlanificador();
char *getIpMemoria();
char *getPuertoMemoria();
int getHilos();
int getRetardo();
void testCpuFunction(char *accion);
char *txtAString(char *rutaDelArchivo);
void enviarResultado(t_resultadoEjecucion resultado, int serverPlanificador);
char *serializarResultado(t_resultadoEjecucion resultado);
t_resultadoEjecucion empaquetarResultado(char *ruta, int contadorPrograma);
char *generarHeader(int clave, int tamanio);
int concatenar(int valorA,int valorB);
int cantidadElementos(char **lista);
char *eventoDeLogeo(char *mensaje, int id);
char *logeoDeEjecucion(char *mensaje, char *ruta, int contador, char *idProceso);

#endif /* LIBRERIACPU_H_ */
