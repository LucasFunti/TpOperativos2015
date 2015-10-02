/*
 * swap.c
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <libSockets.h>
#include <commons/config.h>

#define PUERTO "6668"
#define BACKLOG 5
#define PACKAGESIZE 32

typedef struct {
	char *instruccion;
	int cantidadPaginas;
} t_instruccion;

int reconocerInstruccion() {
	return 0;
}

int main(int argc, char **argv) {
	int socketCliente;
	socketCliente = conectarServidor("localhost", PUERTO, BACKLOG);
	char package[PACKAGESIZE];
	int status = 1;

	/*llenar swap con \0 (funcion) */
	FILE *swap = fopen("swap.data", "w");
	fseek(swap, 512 * 256, SEEK_SET);
	fputc('\0', swap);
	fclose(swap);
	printf("Cliente conectado. Esperando mensajes:\n");

	while (status != 0) {
		status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
		char*instruccion = malloc(10);
		int paginas;
		scanf("%s %d",instruccion,paginas);
		if (status != 0) {
			int instruccion;
			instruccion = reconocerInstruccion();
			switch (instruccion) {
			case 4:/*iniciar*/
				/* corroborar espacio disponible en el archivo-(funcion)*/
				break;
			case 5:/*leer*/
				/*Buscar proceso en el archivo (funcion) */
				/* Si existe - devolver contenido de la pagina n solicitada (funcion) */
				break;
			case 6:/*escribir*/
				break;
			case 7:/*entrada salida*/
				break;
			case 8:/*finalizar*/
				/*Buscar proceso en el archivo (funcion) */
				/* si existe liberarlo de memoria */
				break;
			}
			printf("Mensaje Recibido\n %s", package);
		}
	}

	close(socketCliente);
	return 0;
}
