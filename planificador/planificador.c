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
#include <libSockets.h>
#include <planificadorFunctions.h>
#include <commons/config.h>
#include <commons/txt.h>

#define BACKLOG 5
#define PACKAGESIZE 32

int p_last_id = 0;

int main(int argc, char **argv) {
	//genero el arhivo config y guardo los datos del mismo en variables
	char *puerto;
	char *algoritmo;
	t_config *archivoConfiguracion;
	archivoConfiguracion =
			config_create(
					"/home/utnso/git/tp-2015-2c-signiorcodigo/planificador/planificadorConfig");
	puerto = config_get_string_value(archivoConfiguracion, "PUERTO_ESCUCHA");
	algoritmo = config_get_string_value(archivoConfiguracion,
			"ALGORITMO_PLANIFICACION");

	int socketCliente;
	socketCliente = conectarServidor("localhost", puerto, BACKLOG);
	char proceso[PACKAGESIZE];

	int enviar = 1;
	while (enviar) {
		int codigoOperacion;

		codigoOperacion = reconocerIdentificador();
		printf("el identificador es: %d \n", codigoOperacion);

		switch (codigoOperacion) {
		case 1:
			//leo el nombre del proceso
			scanf("%s", proceso);

			//busco el path del proceso y calculo el tamaño del path
			char *path = malloc(80);
			realpath(proceso, path);
			printf("el path es: %s", path);
			size_t tamMessage;
			tamMessage = strlen(path);

			//le genero un id al proceso
			int pid;
			pid = generarPID(&p_last_id);
			printf("el pid del proceso es: %d\n", pid);

			//genero el pcb del proceso
			tipo_pcb currentPCB;
			currentPCB = generarPCB(pid, path, listo);
			printf("El estado del proceso %d es: %d \n", currentPCB.id,
					currentPCB.estado);

			//genero el paquete, para enviar a la cpu
			//faltaria el puntero a instruccion
			Paquete paquete;
			paquete = generarPaquete(codigoOperacion, tamMessage, path,currentPCB.programCounter);
			char *buffer = serializar(&paquete);

			send(socketCliente, buffer,
					sizeof(int)+ sizeof(int) + sizeof(int) + paquete.tamanio, 0);

			if (!strcmp(algoritmo, "FIFO")) {

			}

			free(buffer);
			free(path);
			break;
		case 99:
			break;

		}

		/*if (enviar)
		 send(socketCliente, message, sizeof(int), 0);*/

	}
	close(socketCliente);
	free(puerto);

	return 0;

}
