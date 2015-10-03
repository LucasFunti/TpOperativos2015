/*
 * libreriaCPU.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef LIBRERIACPU_H_
#define LIBRERIACPU_H_

typedef struct {
	char *instruccion;
	int cantidadDePaginas;
} t_instruccion;
int reconocerInstruccion(char *linea);
t_instruccion empaquetar(char *instruccion, int cantidadDePaginas);
char *serializarEmpaquetado( t_instruccion instruccionEmpaquetada);
int ejecutar(char *linea, int serverMemoria,int serverPlanificador, int idProceso);

#endif /* LIBRERIACPU_H_ */
