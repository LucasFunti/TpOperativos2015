/*
 * cpu.c
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <commons/string.h>
#include <limits.h>
#include "../libreriaSigniorCodigo/libreriaCPU.h"
#include "../libreriaSigniorCodigo/libSockets.h"

#define BACKLOG 5
#define PACKAGESIZE 32

int main(int argc, char **argv) {
	//invoco la funcion para leer los datos del file de config y los guardo en la estructura
	t_config_cpu config_cpu;
	config_cpu = read_config_cpu_file();

	int serverPlanificador, serverMemoria;
	serverPlanificador = conectarCliente(config_cpu.ipPlanificador, config_cpu.puerto_planificador);
	serverMemoria = conectarCliente(config_cpu.ipMemoria, config_cpu.puerto_memoria);

	/*char package[PACKAGESIZE];*/
	int status = 1;

	while (status != 0) {
		int identificador;
		size_t tamMensaje;
		status = recv(serverPlanificador, &identificador, sizeof(int), 0);
		printf("identificador: %d \n", identificador);

		if (status != 0) {
			recv(serverPlanificador, &tamMensaje, sizeof(int), 0);
			printf("tamaño del mensaje a recibir: %d \n", tamMensaje);
			char *mensaje = malloc(tamMensaje);
			recv(serverPlanificador, mensaje, tamMensaje, 0);
			printf("Mensaje Recibido: %s \n", mensaje);
			free(mensaje);

			//Esto hay que delegarlo. <3
			/*void correrArchivo(char *mensaje) {
			 FILE* file = fopen(mensaje, "r");

			 char* buffer = calloc(1, tamMensaje + 1);
			 fread(buffer, tamMensaje, 1, file);
			 char *listaInstrucciones[] = string_split(file, ";");
			 int n = 0;
			 while (listaInstrucciones[n] != NULL) {
			 ejecutar(listaInstrucciones[n], serverMemoria, serverPlanificador,
			 idProceso);
			 n++;

			 }
			 }*/

		}

	}
	/*send(serverMemoria, package, sizeof(package), 0);*/

	close(serverPlanificador);
	close(serverMemoria);

	return 0;
}
