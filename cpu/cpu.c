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


#define IP "127.0.0.1"
#define PUERTO_PLANIFICADOR "6666"
#define PUERTO_MEMORIA "6667"
#define BACKLOG 5
#define PACKAGESIZE 32

typedef struct {
	int codigoOperacion;
	int tamanio;
	char *mensaje;
	int punteroAInstruccion;
} t_tablaDeInstruccion;

int main(int argc, char **argv) {

	int serverPlanificador,serverMemoria;
	serverPlanificador = conectarCliente(IP,PUERTO_PLANIFICADOR);
	serverMemoria = conectarCliente(IP,PUERTO_MEMORIA);

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

			//Esto hay que delegarlo. <3
			void correrArchivo(mensaje) {
				FILE *archivo;
				archivo = fopen(mensaje, "r");
				char *linea;
				while(!eof(archivo)){
						linea = readline(archivo);
						ejecutar(linea, serverMemoria, serverPlanificador);

					}
				};

			};

		};
	/*send(serverMemoria, package, sizeof(package), 0);*/

		close(serverPlanificador);
		close(serverMemoria);

		return 0;
	}
