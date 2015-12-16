/*
 * planificadorFunctions.h
 *
 *  Created on: 2/10/2015
 *      Author: utnso
 */

#ifndef PLANIFICADORFUNCTIONS_H_
#define PLANIFICADORFUNCTIONS_H_
#include <commons/log.h>
#include <commons/collections/queue.h>

enum estado {
	listo = 98,
	ejecucion,
	bloqueado,
	finalizado = 23,
	finquantum = 22,
	instruccionFinalizada = 21,
	entrada_salida = 20
};

typedef struct {
	char *puerto;
	char *algoritmo;
} t_config_planificador;

typedef struct {
	int id;
	char *dirProceso;
	int estado;
	int programCounter;
	char *nombrePrograma;
	time_t tiempoComienzo;
	time_t tiempoFinalizacion;
	time_t tiempoIngreso;
	double tiempoEnReadys;
} tipo_pcb;

typedef struct {
	tipo_pcb * proceso;
	int socket;
	int pid_cpu;
	int instrucciones_ejecutadas;
} nodo_en_ejecucion;

typedef struct {
	tipo_pcb * proceso;
	int espera;
} nodo_entrada_salida;

typedef struct {
	tipo_pcb *pcb;	int tiempo;pthread_mutex_t mutex_bloqueados;
	t_queue * entradaSalida; pthread_mutex_t mutex_ejecucion;
	t_list * listaEjecutando; pthread_mutex_t mutex_readys;
	t_queue * colaListos; t_log * log_planificador;
} data_hilo;

typedef struct {
	int rafagaEjecutada;
	int pid_cpu;
	int PC;
	int resultado_rafaga;
} rafaga_t;

void inicializarMutex(pthread_mutex_t mutex_readys,
	pthread_mutex_t mutex_ejecucion, pthread_mutex_t mutex_bloqueados);

void destruirMutex(pthread_mutex_t mutex_readys,
	pthread_mutex_t mutex_ejecucion, pthread_mutex_t mutex_bloqueados);

int reconocerIdentificador();

int generarPID(int* pid);

tipo_pcb *generarPCB(int pid, char *path, int estado, char *nombre);

char *getPuerto();

char *getAlgoritmo();

void inicializarColecciones(t_queue * colaListos, t_queue * colaFinalizados,
	t_list * listaEjecutando, t_queue * entradaSalida);

void mostrarEstadoDeLista(t_list *lista, char*estado,t_log * log_planificador);

void mostrarEstadoDeListos(t_queue *cola, char*estado,t_log * log_planificador);

void mostrarEstadoDeBloqueados(t_queue *cola, char*estado,t_log * log_planificador);

void agregarAListaDeEjecucion(pthread_mutex_t mutex_ejecucion,
	t_list *listaEjecutando, nodo_en_ejecucion *proceso, t_queue*colaListos,
	t_queue*entrada_salida, t_log*log_planificador);

tipo_pcb * removerDeColaDeListos(pthread_mutex_t mutex_readys,
	t_queue*colaListos);

void agregarEnColaDeListos(tipo_pcb *proceso, pthread_mutex_t mutex_readys,
	t_queue *colaListos, t_log*log_planificador, t_queue*entrada_salida,
	t_list*en_ejecucion);

void cambiarAEstadoDeEjecucion(pthread_mutex_t mutex_readys, t_queue*colaListos,
	pthread_mutex_t mutex_ejecucion, t_list *listaEjecutando,
	t_queue*entrada_salida, t_log*log_planificador, int socketEnEjecucion,int pid_cpu);

nodo_en_ejecucion * removerDeListaDeEjecucion(tipo_pcb *pcb,
	pthread_mutex_t mutex_ejecucion, t_list*listaEjecutando);

void agregarAColaDeBloqueados(pthread_mutex_t mutex_bloqueados,
	t_queue *entradaSalida, nodo_entrada_salida*io, t_queue*colaListos,
	t_list*listaEjecutando, t_log*log_planificador);

void * cambiarEstadoABloqueado(void * data) ;

void cambiarEstado(tipo_pcb *proceso, int estado, t_queue*colaListos,
	t_queue*entrada_salida, t_list*en_ejecucion,t_log * log_planificador);

int setProgramCounter(char *dirProceso);

void enviarContextoEjecucion(int socketCliente, int codigoOperacion,
		tipo_pcb *proceso, char *path,char *algoritmo, int quantum) ;

void interpretarInstruccion(int instruccion, int socketCliente,
		pthread_mutex_t mutex_readys, t_queue *colaListos,
		t_log*log_planificador, t_queue*entradaSalida, t_list*en_ejecucion,
		t_queue*finalizados, pthread_mutex_t mutex_bloqueados,
		pthread_mutex_t mutex_ejecucion) ;

data_hilo *obtenerDatosHilo(data_hilo * dataHilo, nodo_en_ejecucion *Proceso,
		pthread_mutex_t mutex_readys, t_queue *colaListos,
		t_log*log_planificador, t_queue*entradaSalida, t_list*en_ejecucion,
		t_queue*finalizados, pthread_mutex_t mutex_bloqueados,
		pthread_mutex_t mutex_ejecucion,int tiempo);

void loguearRafaga(rafaga_t *otraInstruccion, nodo_en_ejecucion * unProceso,t_log * log_planificador) ;

void agregarAFinalizados(t_queue *finalizados, nodo_en_ejecucion * proceso,
		t_log *log_planificador) ;

void peticionPorcentajeUsoCpu(t_list * lista,int codigo) ;


#endif /* PLANIFICADORFUNCTIONS_H_ */
