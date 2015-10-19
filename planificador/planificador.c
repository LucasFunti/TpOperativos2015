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
	//genero el arhivo config y guardo los datos del mismo en estructura
	t_config_planificador config_planificador;
	config_planificador = read_config_planificador();

	//genero el archivo log
	t_log* log_planificador = log_create("log_planificador", "PLANIFICADOR",
	true, LOG_LEVEL_INFO);

	//generar estructuras necesarias para el planificador (colas)
//	t_list *listaNuevos;
	t_queue *colaListos;
//	t_list *entrada_salida;
	t_list *en_ejecucion;
//	t_queue *colaFinalizados;
//	t_queue *cpu_libres;
	colaListos = queue_create();
	en_ejecucion = list_create();

//	inicializarColecciones(listaNuevos, colaListos, colaFinalizados,cpu_libres,
//			en_ejecucion, entrada_salida);

	int socketCliente;
	socketCliente = conectarServidor("localhost", config_planificador.puerto,
	BACKLOG);
	char proceso[PACKAGESIZE];
	int *pid_a_finalizar = malloc(sizeof(int));
	int enviar = 1;

	while (enviar) {
		int codigoOperacion;

		codigoOperacion = reconocerIdentificador();

		switch (codigoOperacion) {
		case 1:/* correr */

			//leo el nombre del proceso
			scanf("%s", proceso);

			//busco el path del proceso y calculo el tamaño del path
			char *path = malloc(128);
			realpath(proceso, path);
			size_t tamMessage;
			tamMessage = strlen(path);

			//le genero un id al proceso
			int pid;
			pid = generarPID(&p_last_id);

			//genero el pcb del proceso
			tipo_pcb currentPCB;
			currentPCB = generarPCB(pid, path, listo, proceso);

			nodo_proceso proceso;
			proceso.pcb = currentPCB;
			//colocarlo en la cola de readys
			queue_push(colaListos, &proceso);
			log_info(log_planificador,
					"Colocado el proceso: %s con pid = %d en la cola de listos",
					proceso, currentPCB.id);

			if (!strcmp(config_planificador.algoritmo, "FIFO")) {
				//me fijo si tengo algun proceso en la cola de listos

				if (queue_is_empty(colaListos)) {
					printf("No hay procesos en la cola de listos");

				} else {
					//ver que haya alguna cpu disponible - select()

					//agarro el primer elemento y lo envio a la cpu
					//genero el paquete, para enviar a la cpu
					Paquete paquete;
					paquete = generarPaquete(codigoOperacion, tamMessage + 1,
							path, currentPCB.programCounter);
					char *buffer = serializar(&paquete);
					send(socketCliente, buffer,
							sizeof(int) + sizeof(int) + sizeof(int)
									+ paquete.tamanio, 0);
					free(buffer);
					free(path);
				}

			}

			break;
		case 99:/*finalizar*/

			scanf("%d", pid_a_finalizar);

			bool encontrar_pid(void * nodo) {
				return ((((nodo_en_ejecucion*) nodo)->proceso.pcb.id)
						== *pid_a_finalizar);
			}
			nodo_en_ejecucion *procesoEnEjecucion = malloc(
					sizeof(nodo_en_ejecucion));
			procesoEnEjecucion = list_find(en_ejecucion, encontrar_pid);

			free(procesoEnEjecucion);

			break;
		case 2: /* ps */

			mostrarEstadoDeLista(en_ejecucion, "Ejecutando");
			mostrarEstadoDeCola(colaListos, "Listo");

			break;
		case 3: /* cpu */
			break;
		}

		/*if (enviar)
		 send(socketCliente, message, sizeof(int), 0);*/

	}
	close(socketCliente);
	free(config_planificador.puerto);
	free(config_planificador.algoritmo);
	free(pid_a_finalizar);
	log_destroy(log_planificador);

	return 0;

}

