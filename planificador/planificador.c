/*
 * planificador.c
 *

 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "../libreriaSigniorCodigo/planificadorFunctions.h"
#include <commons/config.h>
#include <commons/collections/queue.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include "../libreriaSigniorCodigo/libSockets.h"

#define BACKLOG 100
#define PACKAGESIZE 32

int p_last_id = 0;

int main(int argc, char **argv) {

	//genero el archivo log
	t_log *log_planificador = log_create("log_planificador", "PLANIFICADOR",
	true, LOG_LEVEL_INFO);

	pthread_mutex_t mutex_readys;
	pthread_mutex_t mutex_ejecucion;
	pthread_mutex_t mutex_bloqueado;
	inicializarMutex(mutex_readys, mutex_ejecucion, mutex_bloqueado);
	//generar estructuras necesarias para el planificador (colas)
	t_queue *colaListos = queue_create();
	t_queue *entrada_salida = queue_create();
	t_list *en_ejecucion = list_create();
//	t_queue *colaFinalizados=queue_create();

//	char* port = getPuerto();
//	char *algoritmo = getAlgoritmo();

//	int socketCliente;
//	socketCliente = conectarServidor("localhost", port,
//	BACKLOG);
	int *pid_a_finalizar = malloc(sizeof(int));
	int enviar = 1;

	while (enviar) {

		int codigoOperacion;

		codigoOperacion = reconocerIdentificador();
		char *nombreProceso = malloc(64 * sizeof(char));
		switch (codigoOperacion) {
		case 1:/* correr */

			//leo el nombre del proceso
			scanf("%s", nombreProceso);

			//busco el path del proceso y calculo el tamaño del path
			char *path = malloc(128);
			realpath(nombreProceso, path);

			//le genero un id al proceso
			int pid;
			pid = generarPID(&p_last_id);

			//genero el pcb del proceso
			tipo_pcb *proceso = malloc(sizeof(tipo_pcb));
			proceso = generarPCB(pid, path, listo, nombreProceso);
			agregarEnColaDeListos(proceso, mutex_readys, colaListos,
					log_planificador, entrada_salida, en_ejecucion);
			cambiarAEstadoDeEjecucion(mutex_readys, colaListos, mutex_ejecucion,
					en_ejecucion, entrada_salida);
			//ver que haya alguna cpu disponible - select()

			//agarro el primer elemento y lo envio a la cpu
			//genero el paquete, para enviar a la cpu
//			Paquete paquete;
//			paquete = generarPaquete(codigoOperacion, tamMessage + 1, path,
//					proceso->programCounter);
//			char *buffer = serializar(&paquete);
//			send(socketCliente, buffer,
//					sizeof(int) + sizeof(int) + sizeof(int) + paquete.tamanio,
//					0);
//			free(buffer);
			free(path);

			break;
		case 99:
			/*finalizar*/

			scanf("%d", pid_a_finalizar);
			bool encontrar_pid(void * nodo) {
				return ((((nodo_en_ejecucion*) nodo)->proceso->id)
						== *pid_a_finalizar);
			}
			nodo_en_ejecucion *procesoEnEjecucion = malloc(
					sizeof(nodo_en_ejecucion));
			procesoEnEjecucion = list_find(en_ejecucion, encontrar_pid);
//			int PC = -1;
//			PC = setProgramCounter(procesoEnEjecucion->proceso.pcb.dirProceso);
			//enviar el proceso a la cpu con el programCounter obtenido

			free(procesoEnEjecucion);

			break;
		case 2:
			/* ps */

			mostrarEstadoDeLista(en_ejecucion, "Ejecutando");
			mostrarEstadoDeCola(colaListos, "Listo");

			break;
		case 3:
			/* cpu */
			break;
		}

		/*if (enviar)
		 send(socketCliente, message, sizeof(int), 0);*/

	}
//	close(socketCliente);
	free(pid_a_finalizar);
	destruirMutex(mutex_readys, mutex_ejecucion, mutex_bloqueado);

	return 0;

}

