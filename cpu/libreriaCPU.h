/*
 * libreriaCPU.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef LIBRERIACPU_H_
#define LIBRERIACPU_H_
#include <commons/txt.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include <signiorCodigo/libSockets.h>
#include <pthread.h>
#include "../planificador/planificadorFunctions.h"
#include <stdbool.h>

#define FINALIZAR 8
#define ENTRADASALIDA 7

int porcentajeDeUso[50], instruccionesEjecutadas[50];

bool hayQueFinalizar[50];

typedef struct{
	time_t tiempoInicial;
	time_t tiempoFinal;
	double tiempoOcupado;

}t_tiempoDeUso;

t_list *uso_cpu[50];

typedef struct {
	int codigo_operacion;
	int tamanio_data;

} t_header;

typedef struct {

	t_header * header;
	void * data;

} t_data;

typedef struct {
	int codigoOperacion;
	int punteroAInstruccion;
	int tamanio;
	char *mensaje;
} t_tablaDeInstruccion;

typedef struct {
	char * ipPlanificador;
	char * puerto_planificador;
	char * ipMemoria;
	char * puerto_memoria;
	int cantidadHilos;
	int retardo;
} t_config_cpu;

typedef struct{
	int instruccion;
	bool resultado;
} t_resultado_ejecucion;

typedef struct {
	int idHilo;
	t_log *logger;
} t_hilo;

typedef struct {
	char * path;
	int contadorPrograma;
	char * id;
	int serverMemoria;
	int serverPlanificador;
	t_hilo * threadInfo;
	int quantum;
} t_correr_info;



typedef struct {
	int header;
	int tamanio;
	char * mensaje;
} t_mensaje;

typedef struct {
	int idProceso;
	int idCpu;
	int tiempoIO;
	int m; 				// slots del array usados hasta ahora
	int estado_ultima_instruccion; // 1 si la última instrucción ejecutó correctamente; 0 si hubo un error
	int maximo;
	int contador;		// posición actualizada del contador de programa
	int *data;	// el array de resultados de las ejecuciones de cada instrucción
	int usoDeCpu;		// porcentaje de CPU utilizada en el ultimo minuto;
	int causa_finalizacion; // valor que indica el motivo del fin de ejecucion, o bien que se está enviando un resultado de ejecución (I/O = 20, error = 21, Quantum = 22, finalizar = 23)
} t_resultadoOperacion;

typedef struct {
	int codigo_operacion;
	int tamanio_data;
} t_headersCpu;

int getNumeroHilo();
int reconocerInstruccion(char*);

int ejecutar(char *linea, int serverMemoria, int serverPlanificador, char *idProceso, t_hilo *infoHilo);
int obtenerTiempoIO(char *instruccion);
void correrArchivo(void * dataCorrer);
char *getIpPlanificador();
void *iniciarcpu(void * buffer);
char *getIpPlanificador();
char *getPuertoPlanificador();
char *getIpMemoria();
char *getPuertoMemoria();
int getHilos();
int getRetardo();
void testCpuFunction(char *accion);
char *txtAString(char *rutaDelArchivo);
char* serializarPaqueteCpu(t_data * unPaquete);
void common_send(int socket, t_data * paquete);
t_data *crearPaquete(int codigoOperacion, int pid, int paginas);
t_data *crearPaqueteEscritura(int codigoOperacion, int pid, int paginas,
		char *string);
int cantidadElementos(char **lista);
char *eventoDeLogeo(char *mensaje, int id);
char *logeoDeEjecucion(char *mensaje, char *ruta, int contador, char *idProceso);
void logearIniciar(t_hilo *infoHilo, int estadoDeEjecucion, char *idProceso);
void logearLectura(t_hilo *infoHilo, char *idProceso, char *pagina,
		char *contenidoDePagina);
void logearEscritura(t_hilo *infoHilo, char *idProceso, int estadoDeEjecucion,
		char *pagina, char *texto);
void logearEntradaSalida(t_hilo *infoHilo, char *idProceso, char *tiempo);
void logearFinalizacion(t_hilo *infoHilo, char *idProceso);
void reiniciarContador(int contador);
void removeChar(char *string, char basura);
void funcionResetearContadores();
void resetearContadores();
t_data * leer_paquete(int socket);
t_data * pedirPaquete(int codigoOp, int tamanio, void * data);
void enviarPaqueteEntradaSalida(int pidCpu, int tiempoIO,
		int contadorPrograma, int socket, t_list * resultados);
void enviarPaqueteFinQuantum(int idCpu, int contadorPrograma, int socket, t_list * resultados);
void enviarPaqueteError(int idCpu, int socket);
void enviarPaqueteFinalizar(int pidCpu, int socket, t_list * resultados);
t_data *crearPaqueteConsumo(int id, int consumoActual);

#endif /* LIBRERIACPU_H_ */
