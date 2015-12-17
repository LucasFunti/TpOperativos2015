/*
 * planificador.h
 *
 *  Created on: 16/12/2015
 *      Author: root
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include <pthread.h>
#include <poll.h>
#include <signiorCodigo/libSockets.h>
#include <semaphore.h>
#include "planificadorFunctions.h"

typedef struct {
	int socket;
	bool libre;
	int cpu_id;
} t_cpu;

pthread_mutex_t mutex_readys;
pthread_mutex_t mutex_ejecucion;
pthread_mutex_t mutex_bloqueados;
pthread_mutex_t mutex_cpus;
sem_t procesos_listos;
sem_t cpu_libre;
t_list * listaCpu;
t_queue * colaListos;
t_queue * entradaSalida;
t_list * en_ejecucion;
t_queue * colaFinalizados;
int codigoOperacion;
int p_last_id;
t_log *log_planificador;

#endif /* PLANIFICADOR_H_ */
