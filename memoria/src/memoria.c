/*
 * memoria.c
 *
 <<<<<<< HEAD
 *  Created on: 5/9/2015
 =======
 *  Created on: 28/9/2015
 >>>>>>> e5bed079387090f438690c736e7ce6b16c1bb69c
 *      Author: utnso
 */
#include "memoria.h"

//#include "../libreriaSigniorCodigo/libSockets.h"

#define PACKAGESIZE 32
#define BACKLOG 7

#define INICIAR = 4
#define LEER = 5
#define ESCRIBIR = 6
#define ENTRADA-SALIDA = 7
#define FINALIZAR = 8

/*
int main() {

	iniciarLogger();
	levantarConfiguracion();

	//TODO: Conectarse al swap

	lanzarHilos();

	pthread_join(hiloConexiones, NULL);
	loggearInfo("Se cierra el hilo de conexiones");

	pthread_join(hiloSignals, NULL);
	loggearInfo("Se cierra el hilo de signals");

	loggearInfo("Se cierra exitosamente PAM");

	return 1;
}*/

void lanzarHilos() {
	pthread_create(&hiloSignals, NULL, (void*) atenderSignals, NULL);
	loggearInfo("Se levanta el hilo de conexiones");
	pthread_create(&hiloConexiones, NULL, (void*) atenderConexiones, NULL);
	loggearInfo("Se levanta el hilo de signals");
}

void atenderConexiones() {
/*
	socketEscucha = setup_listen_con_log("localhost",
			dictionary_get(diccionarioConfiguraciones, "PUERTO_ESCUCHA"),
			logger);

	while (1) {

		socketCliente = esperarConexionEntrante(socketEscucha, BACKLOG, logger);
		atenderCliente(socketCliente);

	}*/
}

void atenderCliente(int socketConexion) {
	loggearInfo(
			string_from_format("Atendiendo cliente en el socket %d",
					socketConexion));

}
/*
char * leer_n(int pid, int numero_pagina) {

	if (tlb_encendida()) {

		return usar_tlb(pid, numero_pagina);

	} else {

		return usar_tabla(pid, numero_pagina);
	}
}*/
void atenderSignals() {
}

void levantarConfiguracion() {

	memoriaConfig = config_create("./memoriaConfig");

	diccionario_configuraciones = dictionary_create();

	loggearInfo("Se crea el diccionario de variables de configuracion");

	char variablesConfiguracion[9][30] = { "PUERTO_ESCUCHA", "IP_SWAP",
			"PUERTO_SWAP", "MAXIMO_MARCOS_POR_PROCESO", "CANTIDAD_MARCOS",
			"TAMANIO_MARCO", "ENTRADAS_TLB", "TLB_HABILITADA",
			"ALGORITMO_REMPLAZO" };
	int i;
	for (i = 0; i < 9; i++) {

		dictionary_put(diccionario_configuraciones, variablesConfiguracion[i],
				config_get_string_value(memoriaConfig,
						variablesConfiguracion[i]));
		loggearInfo(
				string_from_format("Configurado con %s: %s",
						variablesConfiguracion[i],
						dictionary_get(diccionario_configuraciones,
								variablesConfiguracion[i])));

	}
}

void loggearWarning(char * info) {

	log_warning(logger, info);
}

void loggearError(char * info) {

	log_error(logger, info);
}

void loggearInfo(char * info) {

	log_info(logger, info);
}

void iniciarLogger() {

//Si existe el archivo lo elimina
	remove("logMemoria");

	logger = log_create("logMemoria", "PAM", true, LOG_LEVEL_DEBUG);
	loggearInfo("Iniciado el proceso administrador de memoria");

}

