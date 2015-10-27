/*
 * cpu.c
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
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

	/*	int cantHilos, i;
	cantHilos = getHilos();
	pthread_t hilos[cantHilos];
	for (i = 0; i > cantHilos; i++) {
		pthread_create(&hilos[i], NULL, iniciarcpu, NULL);
	} */

	char *accion = malloc(sizeof(char) * 32);

	while(strcmp(accion,"exit")!=0){
		printf("ingrese la accion a testear, o escriba 'ayuda' para ver comandos disponibles:\n");
		scanf ("%[^\n]%*c", accion);
	if (strcmp(accion, "reconocer instruccion") == 0) {
				int respuesta;
				printf("ingrese la instruccion a reconocer\n");
				char *dataAdicional = malloc(sizeof(char) * 32);
				scanf ("%[^\n]%*c", dataAdicional);
				respuesta = reconocerInstruccion(dataAdicional);
				printf("el header correspondiente a la instruccion es: %d\n", respuesta);
			}
	else if(strcmp(accion, "empaquetado normal") == 0){
				printf("ingrese la instruccion a empaquetar(formato aceptado: instruccion paginas):\n");
				char *dataAdicional = malloc(sizeof(char) * 64);
				scanf ("%[^\n]%*c", dataAdicional);
				t_instruccion paquete;
				char **algo;
				algo = string_split(dataAdicional," ");
				paquete = empaquetar(algo[0], algo[1]);
				printf("la instruccion es: %s\n", paquete.instruccion);
				printf("el número de página/proceso es: %s\n", paquete.cantidadDePaginas);
			}
	else if(strcmp(accion, "empaquetado de escritura")== 0){
				printf("ingrese la instruccion a empaquetar(formato aceptado: instruccion pagina contenido):\n");
				char *dataAdicional = malloc(sizeof(char) * 64);
				fgets(dataAdicional, 64, stdin);
				t_instruccionEscritura paquete;
				char **algo;
				algo = string_n_split(dataAdicional,3, " ");
				paquete = empaquetarEscritura(algo[0], algo[1], algo[2]);
				printf("la instruccion es: %s\n", paquete.instruccion);
				printf("el número de página es: %s\n", paquete.cantidadDePaginas);
				printf("el contenido a escribir es: %s\n", paquete.textoAEscribir);
			}
	else if (strcmp(accion, "archivo a texto")== 0){
				printf("ingrese una ruta de archivo válida:\n");
				char *dataAdicional = malloc(sizeof(char) * 64);
				scanf ("%[^\n]%*c", dataAdicional);
				char *texto = txtAString(dataAdicional);
				printf("el contenido del archivo es: %s\n", texto);
			}
	else if (strcmp(accion, "serializacion normal") == 0){
		printf("ingrese una linea de código que desee serializar (formato válido: instruccion página):\n");
				char *paqueteSerializado;
				char *dataAdicional = malloc(sizeof(char) * 64);
				t_instruccion paquete;
				scanf ("%[^\n]%*c", dataAdicional);
				char **algo;
				algo = string_split(dataAdicional," ");
				paquete = empaquetar(algo[0], algo[1]);
				paqueteSerializado = serializarEmpaquetado(paquete);
				printf("serializacion exitosa. contenido a enviar: %s\n", paqueteSerializado);

	}
	else if(strcmp(accion, "serializacion con escritura") == 0){
				printf("ingrese una linea de código que desee serializar (formato válido: "
						"instruccion página contenidoAEscribir):\n");
				char *paqueteSerializado;
				char *dataAdicional = malloc(sizeof(char) * 64);
				t_instruccionEscritura paquete;
				scanf ("%[^\n]%*c", dataAdicional);
				char **algo;
				algo = string_n_split(dataAdicional,3, " ");
				paquete = empaquetarEscritura(algo[0], algo[1], algo[2]);
				paqueteSerializado = serializarEmpaquetadoEscritura(paquete);
				printf("serializacion exitosa. contenido a enviar: %s\n", paqueteSerializado);
	}
	else if(strcmp(accion, "ejecutar instruccion") == 0){
				printf("ingrese la instruccion a ejecutar:\n");
				char *dataAdicional = malloc(sizeof(char) * 64);
				char *idProceso = malloc(sizeof(int));
				int  id;
				scanf ("%[^\n]%*c", dataAdicional);
				printf("ingrese una id de proceso:\n");
				scanf ("%[^\n]%*c", idProceso);
				id = atoi(idProceso);
				ejecutar(dataAdicional, 123, 456, id);

	}
	else if(strcmp(accion, "correr archivo") == 0){
		printf("ingrese una ruta de archivo válida:\n");
		char *dataAdicional = malloc(sizeof(char) * 64);
		scanf ("%[^\n]%*c", dataAdicional);
		int contadorDePrograma = 0;
		int  id = 1;
		correrArchivo(dataAdicional, contadorDePrograma, 123, 456, id);
	}
	else if(strcmp(accion, "ayuda") == 0){
				printf("lista de comandos disponibles:\nreconocer instruccion\nempaquetado normal\nempaquetado de escritura\n"
						"archivo a texto\nserializacion normal\nserializacion con escritura\nejecutar instruccion\n"
						"correr archivo\n\n");
				printf("para las opciones 'reconocer instruccion' y 'ejecutar instrucción' utilice las siguientes instrucciones:\n"
						"iniciar N\nleer N\n\escribir N contenidoAEscribir\nentrada-salida Tiempo\nfinalizar\n");
	}
	else printf("comando desconocido. por favor, intente de nuevo.\n");
	}
	return 0;
}
