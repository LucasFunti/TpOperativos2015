/*
 * swap.c
 *
 <<<<<<< HEAD
 *  Created on: 5/9/2015
 =======
 *  Created on: 28/9/2015
 >>>>>>> e5bed079387090f438690c736e7ce6b16c1bb69c
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include "../libreriaSigniorCodigo/swapFunctions.h"
#include "../libreriaSigniorCodigo/libSockets.h"

#define BACKLOG 5
#define PACKAGESIZE 32


int reconocerInstruccion() {
	return 0;
}


int main(int argc, char **argv) {
	char* file_name = getSwapFileName();
	int pagesAmount = getSwapPagesAmount();
	char str[100];
	strcpy(str,"/home/utnso/git/tp-2015-2c-signiorcodigo/swap/Debug/");
	strcpy(str,file_name);
	int result = doesFileExist( str);
	if(!result) {
		setupSwap();
	} else {
	    printf("El archivo de Swap ya existe. Continuamos...\n");
	}
	t_list *pages = setPages(pagesAmount);
	printf("Ingrese Accion\n");
	char *action = malloc(sizeof(char)*15);
	scanf ("%s",action);
	while(strcmp(action,"exit")!=0){
		evaluateAction(action, pages);
		printf("Ingrese Accion\n");
		scanf ("%s",action);
	}


//	t_nodo_swap* item = NULL;
//	for (i = 0; i < top ; i++){
//		item = (t_nodo_swap*) list_get(pages,i);
//		printf("el valor es %d \n",item->numeroPagina);
//		printf("%d\n",i);
//	}
//	char * puerto;
//	char * nombreSwap;
//	int cantidadPaginas;
//	int tamanioPagina;
//	int retardoSwap;
//	int retardoCompactacion;
//	t_config *swapConfiguracion;
//	swapConfiguracion = config_create("/home/utnso/git/tp-2015-2c-signiorcodigo/swap/swapConfig");
//	puerto = config_get_string_value(swapConfiguracion,"PUERTO_ESCUCHA");
//	nombreSwap = config_get_string_value(swapConfiguracion,"NOMBRE_SWAP");
//	cantidadPaginas = config_get_int_value(swapConfiguracion,"CANTIDAD_PAGINAS");
//	tamanioPagina = config_get_int_value(swapConfiguracion,"TAMANIO_PAGINA");
//	retardoSwap = config_get_int_value(swapConfiguracion,"RETARDO_SWAP");
//	retardoCompactacion = config_get_int_value(swapConfiguracion,"RETARDO_COMPACTACION");


//	int socketCliente;
//	socketCliente = conectarServidor("localhost", puerto, BACKLOG);
//	char package[PACKAGESIZE];
//	int status = 1;


//	printf("Cliente conectado. Esperando mensajes:\n");
//
//	while (status != 0) {
//		status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
//				if (status != 0) {
//
//			int instruccion;
//			instruccion = reconocerInstruccion();
//			switch (instruccion) {
//				case 4:/*iniciar*/
//				/* corroborar espacio disponible en el archivo-(funcion)*/
//				break;
//				case 5:/*leer*/
//				/*Buscar proceso en el archivo (funcion) */
//				/* Si existe - devolver contenido de la pagina n solicitada (funcion) */
//				break;
//				case 6:/*escribir*/
//				break;
//				case 7:/*entrada salida*/
//				break;
//				case 8:/*finalizar*/
//				/*Buscar proceso en el archivo (funcion) */
//				/* si existe liberarlo de memoria */
//				break;
//			}
//			printf("Mensaje Recibido\n %s", package);
//		}
//
//	}
//
//	close(socketCliente);
	list_destroy(pages);
	return 0;
}

