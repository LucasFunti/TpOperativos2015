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
	int cantidadDePaginas;
} t_instruccion;

typedef struct instruccionEscritura{
	char *instruccion;
	int cantidadDePaginas;
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

int reconocerInstruccion(char*);
t_instruccion empaquetar(char *, int);
t_instruccionEscritura empaquetar_escritura(char *instruccionRecibida, int paginas, char *texto);
char *serializarEmpaquetado(t_instruccion instruccionEmpaquetada);
int ejecutar(char *linea, int serverMemoria,int serverPlanificador, int idProceso);
t_config_cpu read_config_cpu_file();
#endif /* LIBRERIACPU_H_ */
