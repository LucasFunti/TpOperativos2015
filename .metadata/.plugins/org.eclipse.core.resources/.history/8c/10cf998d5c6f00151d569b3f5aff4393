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
		char * buffer = malloc(sizeof(int)*3);
		status = recv(serverPlanificador,buffer,(sizeof(int)*3),0);
		Paquete *contexto_ejecucion;
		contexto_ejecucion = deserializar_header(buffer);
		free(buffer);
		//estos printf son probar que ande nada mas
		printf("El codigo de la operacion es:%d \n", contexto_ejecucion->codigoOperacion);
		printf("El PC es:%d \n", contexto_ejecucion->programCounter);
		printf("El tamanio a recivir es:%d\n", contexto_ejecucion->tamanio);
		if (status != 0) {
			char * dataBuffer = malloc(contexto_ejecucion->tamanio);
			recv(serverPlanificador,dataBuffer,contexto_ejecucion->tamanio,0);
			contexto_ejecucion->mensaje = malloc(contexto_ejecucion->tamanio);
			memcpy(contexto_ejecucion->mensaje,dataBuffer,contexto_ejecucion->tamanio);
			free(dataBuffer);
			printf("el mensaje es:%s \n", contexto_ejecucion->mensaje);
			destruirPaquete(contexto_ejecucion);


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
	free(config_cpu.ipMemoria);
	free(config_cpu.ipPlanificador);
	free(config_cpu.puerto_memoria);
	free(config_cpu.puerto_planificador);
	return 0;
}
