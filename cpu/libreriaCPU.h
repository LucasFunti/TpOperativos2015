/*
 * libreriaCPU.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef LIBRERIACPU_H_
#define LIBRERIACPU_H_
#include <signiorCodigo/planificadorFunctions.h>
#include <pthread.h>

int porcentajeDeUso[50], instruccionesEjecutadas[50];

typedef struct {
	int codigo_operacion;
	int tamanio_data;

} t_header;

typedef struct {

	t_header * header;
	void * data;

} t_data;

typedef struct instruccion{
	char *instruccion;
	char *cantidadDePaginas;
} t_instruccion;

typedef struct instruccionEscritura{
	int idProceso;
	int paginas;
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
	int idProceso;
	int idCpu;
	int tiempoIO;
	int m; 				// slots del array usados hasta ahora
	int estado_ultima_instruccion; 		// 1 si la última instrucción ejecutó correctamente; 0 si hubo un error
	int maximo;
	int contador;		// posición actualizada del contador de programa
	int *data;			// el array de resultados de las ejecuciones de cada instrucción
	int usoDeCpu;		// porcentaje de CPU utilizada en el ultimo minuto;
	int causa_finalizacion; // valor que indica el motivo del fin de ejecucion (I/O = 20,error = 21, Quantum = 22, finalizar = 23)
} t_resultadoOperacion;

typedef struct {
	int codigo_operacion;
	int tamanio_data;
} t_headersCpu;


int reconocerInstruccion(char*);
t_instruccion empaquetar(char *, char *);
t_instruccionEscritura empaquetarEscritura(char *, char *, char *);
char *serializarEmpaquetado(t_instruccion instruccionEmpaquetada);
char *serializarEmpaquetadoEscritura(t_instruccionEscritura instruccionEmpaquetada);
int ejecutar(char *linea, int serverMemoria, int serverPlanificador, char *idProceso, t_hilo *infoHilo);
t_resultadoOperacion correrArchivo(char *ruta, int contadorPrograma, char* idProceso, int serverMemoria, int serverPlanificador,
		t_hilo *infoDelHilo, int quantum);
char *getIpPlanificador();
void *iniciarcpu(void *buffer);
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
char* serializarPaqueteCpu(t_data * unPaquete);
void common_send(int socket, t_data * paquete);
t_resultadoEjecucion empaquetarResultado(char *ruta, int contadorPrograma);
t_data *crearPaquete(int codigoOperacion, int pid, int paginas);
t_data *crearPaqueteEscritura(int codigoOperacion, int pid, int paginas, char *string);
int cantidadElementos(char **lista);
char *eventoDeLogeo(char *mensaje, int id);
char *logeoDeEjecucion(char *mensaje, char *ruta, int contador, char *idProceso);
void logearIniciar(t_hilo *infoHilo, int estadoDeEjecucion, char *idProceso);
void logearLectura(t_hilo *infoHilo, char *idProceso, char *pagina, char *contenidoDePagina);
void logearEscritura(t_hilo *infoHilo, char *idProceso, int estadoDeEjecucion, char *pagina, char *texto);
void logearEntradaSalida(t_hilo *infoHilo, char *idProceso, char *tiempo);
void logearFinalizacion(t_hilo *infoHilo, char *idProceso);
void reiniciarContador(int contador);
void removeChar(char *string, char basura);
void funcionResetearContadores();
void resetearContadores();
t_data * leer_paquete(int socket);
t_data * pedirPaquete(int codigoOp, int tamanio, void * data);
t_data *crearPaqueteEntradaSalida(t_resultadoOperacion resultado);
t_data *crearPaqueteFinQuantum(t_resultadoOperacion resultado);
t_data *crearPaqueteFinalizar(t_resultadoOperacion resultado);
t_data *crearPaqueteConsumo(int id, int consumoActual);

#endif /* LIBRERIACPU_H_ */
