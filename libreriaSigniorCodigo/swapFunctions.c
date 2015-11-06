/*
 * swapFunctions.c

 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include "swapFunctions.h"
#include "libSockets.h"

void swapInit(t_list *pages){
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
	pages = setPages(pagesAmount);
}

void deSerialize(void* element,void* buffer, size_t size){
	memcpy(element,buffer,size);
}

void serialize(void* element, void* buffer, size_t size){
	memcpy(buffer, element, size);
}


int doesFileExist(const char *filename) {
	FILE *fp = fopen (filename, "r");
   if (fp!=NULL) fclose (fp);
   return (fp!=NULL);
}

int getSwapPagesAmount(){
	t_config *swapConfiguracion;
	swapConfiguracion = config_create("/home/utnso/git/tp-2015-2c-signiorcodigo/swap/swapConfig");
	int pages_amount = config_get_int_value(swapConfiguracion,"CANTIDAD_PAGINAS");
	return pages_amount;
}

int getSwapPagesSize(){
	t_config *swapConfiguracion;
	swapConfiguracion = config_create("/home/utnso/git/tp-2015-2c-signiorcodigo/swap/swapConfig");
	int page_size = config_get_int_value(swapConfiguracion,"TAMANIO_PAGINA");
	return page_size;
}

int getSwapListenPort(){
	t_config *swapConfiguracion;
	swapConfiguracion = config_create("/home/utnso/git/tp-2015-2c-signiorcodigo/swap/swapConfig");
	int port = config_get_int_value(swapConfiguracion,"PUERTO_ESCUCHA");
	return port;
}

char* getSwapFileName(){
	t_config *swapConfiguracion;
	swapConfiguracion = config_create("/home/utnso/git/tp-2015-2c-signiorcodigo/swap/swapConfig");
	char* file_name = config_get_string_value(swapConfiguracion,"NOMBRE_SWAP");
	return file_name;
}

void setupSwap(){
	//Traigo datos del archivo de configuracion de Swap y creo un archivo
	//con las especificaciones de configuracion con /0
	int pages_amount = getSwapPagesAmount();
	int page_size = getSwapPagesSize();
	char *file_name = getSwapFileName();
	char command[100];
	int total = pages_amount * page_size;
	sprintf(command, "dd if=/dev/zero of=%s bs=%d count=1",file_name,total );
	system(command);
	printf("Archivo de swap creado bajo el nombre %s\n",file_name );
}

t_list* setPages(int pagesAmount){
	t_list *paginas = list_create();
	int i;
	int top = pagesAmount;
	for (i = 0; i < top ; i++){
		t_nodo_swap *newItemPtr = malloc(sizeof(t_nodo_swap));
		newItemPtr->numeroPagina = i;
		newItemPtr->nombreProceso = "";
	  	list_add(paginas,newItemPtr);
	}
	return paginas;
}

void markPage(int pageNumber,char *processName, t_list *pages){
	t_nodo_swap* item = (t_nodo_swap*) list_get(pages,pageNumber);
	item->nombreProceso = processName;
}

void writePage(int pageNumber,char *content){
	int page_size = getSwapPagesSize();
	char *file_name = getSwapFileName();
	FILE *fp=fopen(file_name, "rb+");
	int position = pageNumber * page_size;
	fseek( fp, position, SEEK_SET);
	fwrite(content,sizeof(char),strlen(content),fp);
	fclose(fp);
}

char* readPage(int pageNumber){
	int page_size = getSwapPagesSize();
	char *file_name = getSwapFileName();
	FILE *fp=fopen(file_name, "r");
	char* page = malloc(sizeof(char)*page_size);
	int position = pageNumber * page_size;
	fseek( fp, position, SEEK_SET);
	fgets(page,page_size+1,fp);
	return page;
}

void evaluateAction(char* action, t_list *pages){
	if(strcmp(action,"reservar")==0){
		printf("Ingrese nombre de Proceso\n");
		char *process_name = malloc(sizeof(char)*15);
		scanf ("%s",process_name);
		printf("Ingrese cantidad de paginas a reservar\n");
		int amount_to_reserve;
		scanf ("%d",&amount_to_reserve);
		reserve(process_name,amount_to_reserve, pages);
	}else if(strcmp(action,"escribir")==0){
		printf("Ingrese nombre de Proceso\n");
		char *process_name = malloc(sizeof(char)*15);
		scanf ("%s",process_name);
		int hasReservedPages;
		hasReservedPages = checkProcessSpace(process_name, pages);
		if (hasReservedPages>=0){
			char *text = "texto de prueba para signior funti";
			int start = checkProcessSpace(process_name,pages);
			writePage(start,text);
			markPage(start,process_name,pages);
		} else {
			printf("El proceso no posee páginas reservadas\n");
		}
	}else if(strcmp(action,"leer")==0){
		printf("Ingrese pagina de lectura\n");
		int pagina_lectura;
		scanf ("%d",&pagina_lectura);
		char *retorno = readPage(pagina_lectura);
		printf("%s\n",retorno);
	}else if(strcmp(action,"compactar")==0){
		compact(pages);
	}else if(strcmp(action,"liberar")==0){
		printf("Ingrese nombre de Proceso\n");
		char *process_name = malloc(sizeof(char)*15);
		scanf ("%s",process_name);
		freeSpace(process_name,pages);
	}else if(strcmp(action,"imprimir")==0){
		printf("Paginas\n");
		imprimir(pages);
	}else{
		printf("Accion desconocida\n");
	}
}

int checkSpaceAvailability (int amount, t_list *pages){
	int top = list_size(pages);
	int count = 0;
	int i;
	if (amount<=list_size(pages)){
		for (i = 0; i < top ; i++){
			t_nodo_swap *newItemPtr = (t_nodo_swap*)list_get(pages,i);
			if (strcmp(newItemPtr->nombreProceso,"")==0){
				count++;
			} else {
				count = 0;
			}
			if (count == amount){
				return (i-amount+1);
			}
		}
	}
	return -1;
}

void freeSpace(char *name,t_list *pages){
	int top = list_size(pages);
	int i;
	for (i = 0; i < top ; i++){
		t_nodo_swap *newItemPtr = (t_nodo_swap*)list_get(pages,i);
		if (strcmp(newItemPtr->nombreProceso,name)==0){
			newItemPtr->nombreProceso = "";
		}
	}
}

void reserve(char* name, int amount, t_list *pages){
	int result = checkSpaceAvailability(amount,pages);
	if (result>=0){
		int i;
		int top = result + amount;
		for (i = result; i < top ; i++){
			markPage(i,name,pages);
		}
		printf("Paginas reservadas\n");
	} else {
		printf("No existe espacio suficiente para el Proceso\n");
	}
}

//Retorna la primer pagina reservada para
//el proceso o -1 si no tiene espacio reservado
int checkProcessSpace(char* name, t_list *pages){
	int i;
	int top = getSwapPagesAmount();
	t_nodo_swap *newItemPtr;
	for (i = 0; i < top ; i++){
		newItemPtr = (t_nodo_swap*)list_get(pages,i);
		if (strcmp(newItemPtr->nombreProceso,name)==0){
			return newItemPtr->numeroPagina;
		}
	}
	return -1;
}

//Usando 2 listas voy copiando las posiciones en las listas

void compact(t_list *pages){
	int i;
	int top = getSwapPagesAmount();
	t_nodo_swap *newItemPtr;
	int index = 0;
	t_list *sortedPages = list_create();
	printf("1\n" );
	for (i = 0; i < top ; i++){
		newItemPtr = (t_nodo_swap*)list_get(pages,i);
		if (strcmp(newItemPtr->nombreProceso,"")!=0){
			t_nodo_swap *newListItemPtr = malloc(sizeof(t_nodo_swap));
			newListItemPtr->numeroPagina = index;
			newListItemPtr->nombreProceso = newItemPtr->nombreProceso;
			list_add(sortedPages,newListItemPtr);
			copyPage(i,index);
			index++;
		}
	}
	printf("ACA\n" );
	t_nodo_swap *anotherItemPtr;
	top = list_size(sortedPages);
	for (i = 0; i < top ; i++){
		newItemPtr = (t_nodo_swap*)list_get(sortedPages,i);
		anotherItemPtr = (t_nodo_swap*)list_get(pages,i);
		anotherItemPtr->numeroPagina = newItemPtr->numeroPagina;
		anotherItemPtr->nombreProceso = newItemPtr->nombreProceso;
	}
	list_destroy(sortedPages);
	fillRemainingSpace(pages, top);
}

//Leo una pagina y luego la muevo a la posicion deseada

void copyPage(int from, int to){
	char *page = readPage(from);
	writePage(to,page);
}

void fillRemainingSpace(t_list *list, int from){
	int top = getSwapPagesAmount();
	int i;
	for (i = from; i < top ; i++){
		t_nodo_swap *newItemPtr = list_get(list,i);
		newItemPtr->numeroPagina = i;
		newItemPtr->nombreProceso = "";
	}
}

//Imprime todas las paginas de la lista Indice,
//Mostrando numero de pagina y nombre de proceso

void imprimir(t_list *list){
	int size = list_size(list);
	int i;
	for (i = 0; i < size ; i++){
		t_nodo_swap *newItemPtr = list_get(list,i);
		printf("%d ",newItemPtr->numeroPagina);
		printf("%s\n",newItemPtr->nombreProceso);
	}
}
