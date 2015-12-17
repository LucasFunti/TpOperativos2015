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
#include "swapFunctions.h"
#include <signiorCodigo/libSockets.h>

#define INICIAR 4
#define LEER 5
#define ESCRIBIR 6
#define FINALIZAR 8



int main(int argc, char **argv) {
	t_log * log_swap = log_create("log_swap", "SWAP", true, LOG_LEVEL_INFO);

	int socketEscucha, socketMemoria;

	char * puerto = getPort();

	socketEscucha = setup_listen("localhost", puerto);

	socketMemoria = esperarConexionEntrante(socketEscucha, 1024, log_swap);

	t_data * paqueteInicio = leer_paquete(socketMemoria);

	if (paqueteInicio->header->codigo_operacion == 1) {

		int datonulo = 0;

		paqueteInicio = pedirPaquete(2, sizeof(int), &datonulo);

		common_send(socketMemoria, paqueteInicio);

		log_info(log_swap, "Conectado con la memoria en el socket: %d",
				socketMemoria);

	} else {

		log_info(log_swap, "Falla en la conexion con la memoria");
		exit(EXIT_FAILURE);
	}

	setupSwap();
	int pagesAmount = getSwapPagesAmount();
	t_list *pages = setPages(pagesAmount);

	while (1) {
		t_data * paquete = leer_paquete(socketMemoria);

		int cantidad,pid,pagina,tamanio;

		switch (paquete->header->codigo_operacion) {

		case INICIAR:
			memcpy(&cantidad, paquete->data, sizeof(int));
			//TODO faltaria el pid del proceso
			//reserve(process_name,cantidad, pages);

			break;
		case LEER:
			memcpy(&pid,paquete->data,sizeof(int));
			memcpy(&pagina,paquete->data + sizeof(int),sizeof(int));
			//me parece que faltaria algo en esa funcion porque lee una pagina pero no en base a la del proceso
			readPage(pagina);
			break;
		case ESCRIBIR:
			memcpy(&pid,paquete->data,sizeof(int));
			memcpy(&pagina,paquete->data + sizeof(int),sizeof(int));
			memcpy(&tamanio,paquete->data + (sizeof(int) * 2),sizeof(int));
			char * texto = malloc(tamanio);
			memcpy(texto,paquete->data + (sizeof(int)*3),tamanio);

			int hasReservedPages;
			hasReservedPages = checkProcessSpace(pid, pages);
			if (hasReservedPages>=0){
				int start = checkProcessSpace(pid,pages);
				writePage(start,texto);
				markPage(start,pid,pages);
			} else {
				printf("El proceso no posee páginas reservadas\n");
			}
			break;
		case FINALIZAR:
			break;
		}
	}
//	char* file_name = getSwapFileName();
//	int pagesAmount = getSwapPagesAmount();
//	char str[100];
//	strcpy(str,"/tp-2015-2c-signiorcodigo/swap/Debug/");
//	strcpy(str,file_name);
//	int result = doesFileExist( str);
//	if(!result) {
//		setupSwap();
//	} else {
//	    printf("El archivo de Swap ya existe. Continuamos...\n");
//	}
//	t_list *pages = setPages(pagesAmount);
//	printf("Ingrese Accion\n");
//	char *action = malloc(sizeof(char)*15);
//	scanf ("%s",action);
//	while(strcmp(action,"exit")!=0){
//		evaluateAction(action, pages);
//		printf("Ingrese Accion\n");
//		scanf ("%s",action);
//	}

//	t_nodo_swap* item = NULL;
//	for (i = 0; i < top ; i++){
//		item = (t_nodo_swap*) list_get(pages,i);
//		printf("el valor es %d \n",item->numeroPagina);
//		printf("%d\n",i);
//	}

//
//
//	int socketCliente;
//	socketCliente = conectarServidor("localhost", puerto, BACKLOG);
//	char package[PACKAGESIZE];
//	int status = 1;
//
//
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
//	list_destroy(pages);
	return 0;
}

