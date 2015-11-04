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

#define CERRAR_CONEXION -3

#define INICIAR 4
#define LEER 5
#define ESCRIBIR 6
#define FINALIZAR 8

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
}

void lanzarHilos() {
	pthread_create(&hiloSignals, NULL, (void*) atenderSignals, NULL);
	loggearInfo("Se levanta el hilo de conexiones");
	pthread_create(&hiloConexiones, NULL, (void*) atenderConexiones, NULL);
	loggearInfo("Se levanta el hilo de signals");
}

void atenderConexion(int socket, fd_set sockets_activos) {

	t_data * data_entrante = leer_paquete(socket);

	switch (data_entrante->header->codigo_operacion) {

	case INICIAR:
		log_info(logger,
				string_from_format(
						"Realizando una operación INICIAR para el socket %d",
						socket));

		break;

	case LEER:
		log_info(logger,
				string_from_format(
						"Realizando una operación LEER para el socket %d",
						socket));
		break;

	case ESCRIBIR:
		log_info(logger,
				string_from_format(
						"Realizando una operación ESCRIBIR para el socket %d",
						socket));
		break;

	case FINALIZAR:
		log_info(logger,
				string_from_format(
						"Realizando una operación FINALIZAR para el socket %d",
						socket));
		close(socket);
		FD_CLR(socket, &sockets_activos);
		break;

	case CERRAR_CONEXION:
		log_info(logger,
				string_from_format("Cerrando conexion en el socket %d",
						socket));

		break;

	default:
		log_info(logger,
				string_from_format(
						"Codigo de operación (%d) no reconocido enviado por el socket %d",
						data_entrante->header->codigo_operacion, socket));
		break;

	}

}

void atenderSignals() {
}

void levantarConfiguracion() {

	memoriaConfig = config_create(
			"/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/src/memoriaConfig");

	diccionario_configuraciones = dictionary_create();

	loggearInfo("Se crea el diccionario de variables de configuracion");

	char variablesConfiguracion[10][50] = { "PUERTO_ESCUCHA", "IP_SWAP",
			"PUERTO_SWAP", "MAXIMO_MARCOS_POR_PROCESO", "CANTIDAD_MARCOS",
			"TAMANIO_MARCO", "ENTRADAS_TLB", "TLB_HABILITADA",
			"RETARDO_MEMORIA", "ALGORITMO_REMPLAZO" };
	int i;
	for (i = 0; i < 10; i++) {

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

