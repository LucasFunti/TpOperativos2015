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
	char *hilo;
} t_tcb;

typedef struct{
	char *nombrePrograma;
	int programCounter;
} t_resultadoEjecucion;


int reconocerInstruccion(char*);
t_instruccion empaquetar(char *, char *);
t_instruccionEscritura empaquetarEscritura(char *, char *, char *);
char *serializarEmpaquetado(t_instruccion instruccionEmpaquetada);
char *serializarEmpaquetadoEscritura(t_instruccionEscritura instruccionEmpaquetada);
int ejecutar(char *linea, int serverMemoria,int serverPlanificador, int idProceso);
void correrArchivo(tipo_pcb pcb, int serverMemoria, int serverPlanificador);
char *getIpPlanificador();
void *iniciarcpu();
char *getIpPlanificador();
char *getPuertoPlanificador();
char *getIpMemoria();
char *getPuertoMemoria();
int getHilos();
int getRetardo();
void testearFuncion(char *accion);
char *txtAString(char *rutaDelArchivo);
void enviarResultado(t_resultadoEjecucion resultado, int serverMemoria);
char *serializarResultado(t_resultadoEjecucion resultado);

#endif /* LIBRERIACPU_H_ */
