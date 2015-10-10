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

#include "libSockets.h"
#include "swapFunctions.h"

void setupSwap() {

}

t_bitarray *readFile(char *file) {
	FILE *fileptr;
	char *buffer;
	long filelen;

	fileptr = fopen(file, "rb");  // Open the file in binary mode
	fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(fileptr);         // Get the current byte offset in the file
	rewind(fileptr);                   // Jump back to the beginning of the file

	buffer = (char *) malloc((filelen + 1) * sizeof(char)); // Enough memory for file + \0
	fread(buffer, filelen, 1, fileptr); // Read in the entire file
	fclose(fileptr);
	t_bitarray *array = bitarray_create(buffer, filelen);
	return array;
}

/* leer y guardar en una estructura los datos del archivo de configuracion*/
t_config_swap read_config_swap() {
	t_config_swap swap_config;
	t_config *swapConfiguracion;
	swapConfiguracion = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/swap/swapConfig");
	swap_config.puerto = config_get_string_value(swapConfiguracion, "PUERTO_ESCUCHA");
	swap_config.nombreSwap = config_get_string_value(swapConfiguracion,"NOMBRE_SWAP");
	swap_config.cantidadPaginas = config_get_int_value(swapConfiguracion,"CANTIDAD_PAGINAS");
	swap_config.tamanioPagina = config_get_int_value(swapConfiguracion,"TAMANIO_PAGINA");
	swap_config.retardoSwap = config_get_int_value(swapConfiguracion,"RETARDO_SWAP");
	swap_config.retardoCompactacion = config_get_int_value(swapConfiguracion,"RETARDO_COMPACTACION");

	return swap_config;
}
