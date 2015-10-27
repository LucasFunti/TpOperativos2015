/*
 * libreriaCPU.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef LIBRERIACPU_H_
#define LIBRERIACPU_H_

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

int reconocerInstruccion(char*);
t_instruccion empaquetar(char *, char *);
t_instruccionEscritura empaquetarEscritura(char *, char *, char *);
char *serializarEmpaquetado(t_instruccion instruccionEmpaquetada);
char *serializarEmpaquetadoEscritura(t_instruccionEscritura instruccionEmpaquetada);
int ejecutar(char *linea, int serverMemoria,int serverPlanificador, int idProceso);
void correrArchivo(char *rutaDelArchivo, int contadorDePrograma, int serverMemoria, int serverPlanificador, int idProceso);
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

#endif /* LIBRERIACPU_H_ */
