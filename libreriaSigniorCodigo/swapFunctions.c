/*
 * swapFunctions.c

 *
 *  Created on: 28/9/2015
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
#include "libSockets.h"

int doesFileExist(const char *filename) {
	FILE *fp = fopen (filename, "r");
   if (fp!=NULL) fclose (fp);
   return (fp!=NULL);
}

void setupSwap(){
	//Traigo datos del archivo de configuracion de Swap y creo un archivo
	//con las especificaciones de configuracion con /0
	t_config *swapConfiguracion;
	swapConfiguracion = config_create("/home/utnso/git/tp-2015-2c-signiorcodigo/swap/swapConfig");
	int pages_amount = config_get_int_value(swapConfiguracion,"CANTIDAD_PAGINAS");
	int page_size = config_get_int_value(swapConfiguracion,"TAMANIO_PAGINA");
	char* file_name = config_get_string_value(swapConfiguracion,"NOMBRE_SWAP");
	char command[100];
	int total = pages_amount * page_size;
	sprintf(command, "dd if=/dev/zero of=%s bs=%d count=1",file_name,total );
	system(command);
	printf("Archivo de swap creado bajo el nombre %s\n",file_name );
}

t_list* getPages(int pagesAmount){
	t_list *paginas = list_create();
	int i;
	int top = pagesAmount+1;
	for (i = 1; i < top ; i++){
		t_nodo_swap *newItemPtr = malloc(sizeof(t_nodo_swap));
		t_nodo_swap newItem;
		newItemPtr = &newItem;
		newItem.numeroPagina = i;
		newItem.nombreProceso = "";
	  	list_add(paginas,newItemPtr);
	}
	return paginas;
}

