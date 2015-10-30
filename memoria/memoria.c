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
#define BACKLOG 5

#define INICIAR = 4
#define LEER = 5
#define ESCRIBIR = 6
#define ENTRADA-SALIDA = 7
#define FINALIZAR = 8

char * puertoCpu;
char * ipSwap;
char * puertoSwap;

t_log * logger;
t_config * memoriaConfig;

pthread_t hiloConexiones;


void main() {

	iniciarLogger();
	levantarConfiguracion();

	atenderConexiones();
	atenderSignals();

	//pthread_join(hiloConexiones, NULL);

	loggearInfo("Se cierra el hilo de conexiones");
	loggearInfo("Se cierra exitosamente PAM");

}

void atenderConexiones() {

	loggearInfo("Se levanta el hilo de conexiones");

}

void atenderSignals() {
	loggearInfo("Se levanta el hilo que atienden signals");
}

void levantarConfiguracion() {

	memoriaConfig = config_create("./memoriaConfig");

	puertoCpu = config_get_string_value(memoriaConfig, "PUERTO_ESCUCHA");
	ipSwap = config_get_string_value(memoriaConfig, "IP_SWAP");
	puertoSwap = config_get_string_value(memoriaConfig, "PUERTO_SWAP");

	char msg[120];

	strcpy(msg, "Configurado con IP_SWAP: ");
	strcat(msg, ipSwap);
	strcat(msg, ",PUERTO_CPU: ");
	strcat(msg, puertoCpu);
	strcat(msg, ", PUERTO_SWAP: ");
	strcat(msg, puertoSwap);

	loggearInfo(msg);
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

	logger = log_create("logMemoria", "PAM ", true, LOG_LEVEL_DEBUG);
	loggearInfo("Iniciado el proceso administrador de memoria");

}

