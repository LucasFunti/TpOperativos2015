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
#include <libSockets.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <limits.h>
#include <libreriaCPU.h>

#define BACKLOG 5
#define PACKAGESIZE 32

typedef struct {
	int codigoOperacion;
	int punteroAInstruccion;
	int tamanio;
	char *mensaje;
} t_tablaDeInstruccion;

int main(int argc, char **argv) {
	char * ipPlanificador;
	char * puerto_planificador;
	char * ipMemoria;
	char * puerto_memoria;
	//int cantidadHilos;
	//int retardo;
	t_config * cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpu/cpuConfig");
	ipPlanificador = config_get_string_value(cpuConfig, "IP_PLANIFICADOR");
	puerto_planificador = config_get_string_value(cpuConfig,"PUERTO_PLANIFICADOR");
	ipMemoria = config_get_string_value(cpuConfig,"IP_MEMORIA");
	puerto_memoria = config_get_string_value(cpuConfig,"PUERTO_MEMORIA");
	//cantidadHilos = config_get_int_value(cpuConfig,"CANTIDAD_HILOS");
	//retardo = config_get_int_value(cpuConfig,"RETARDO");

	int serverPlanificador, serverMemoria;
	serverPlanificador = conectarCliente(ipPlanificador, puerto_planificador);
	serverMemoria = conectarCliente(ipMemoria, puerto_memoria);

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
/*			void correrArchivo(char *mensaje) {
				FILE *archivo;
				archivo = fopen(mensaje, "r");
				char *linea;
				while(!eof(archivo)){
						linea = readline(archivo);
						ejecutar(linea, serverMemoria, serverPlanificador, idProceso);

					}
				};
*/
		};

	};
	/*send(serverMemoria, package, sizeof(package), 0);*/

	close(serverPlanificador);
	close(serverMemoria);

	return 0;
}
