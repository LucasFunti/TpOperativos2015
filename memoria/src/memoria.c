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

//TODO: Rellenar con /0 cuando escribo escribo una página

int main() {

	iniciarLogger();
	levantarConfiguracion();
	iniciar_marcos(memoriaConfig);
	registrarSeniales();

	conectarseAlSwap();

	//Inicio el semáforo que blockea todas las operaciones de la memoria
	pthread_mutex_init(&semaforo_memoria, NULL);

	atenderConexiones();

	return 1;
}

void atenderConexion(int socket, fd_set sockets_activos) {

	pthread_mutex_lock(&semaforo_memoria);

	t_data * data_entrante = leer_paquete(socket);

	switch (data_entrante->header->codigo_operacion) {

	case INICIAR:

		log_info(logger,
				string_from_format(
						"Realizando una operación INICIAR para el socket %d",
						socket));

		int pid, cant_paginas;

		memcpy(&pid, data_entrante->data, sizeof(int));
		memcpy(&cant_paginas, data_entrante->data + sizeof(int), sizeof(int));

		//El false indica que no es un test
		bool exito = iniciar_n(pid, cant_paginas, memoriaConfig, false);

		t_data * paquete;

		if (exito) {

			log_info(logger,
					string_from_format("Se inicia el programa %d con %d marcos",
							pid, cant_paginas));

			//Data basura le mando, total no tengo nada que mandar
			paquete = pedirPaquete(SE_INICIO, sizeof(int), &pid);

		} else {

			log_info(logger, "No se pudo iniciar el programa pedido");

			//Data basura le mando, total no tengo nada que mandar
			paquete = pedirPaquete(NO_SE_INICIO, sizeof(int), &pid);

		}
		common_send(socket, paquete);

		free(paquete);
		break;

	case LEER:

		log_info(logger,
				string_from_format(
						"Realizando una operación LEER para el socket %d",
						socket));

		int pid_leer, pagina_leer;

		memcpy(&pid_leer, data_entrante->data, sizeof(int));
		memcpy(&pagina_leer, data_entrante->data + sizeof(int), sizeof(int));

		char * contenido = leer_n(pid_leer, pagina_leer, memoriaConfig);

		log_info(logger,
				string_from_format("Leer el pid %d y página %d retorna %s",
						pid_leer, pagina_leer, contenido));

		int tamanio_marco = config_get_int_value(memoriaConfig,
				"TAMANIO_MARCO");

		paquete = pedirPaquete(LEER, tamanio_marco, contenido);
		common_send(socket, paquete);

		free(paquete);
		break;

	case ESCRIBIR:
		log_info(logger,
				string_from_format(
						"Realizando una operación ESCRIBIR para el socket %d",
						socket));

		int pid_escribir, pagina_escribir, tamanio_texto;

		memcpy(&pid_escribir, data_entrante->data, sizeof(int));
		memcpy(&pagina_escribir, data_entrante->data + sizeof(int),
				sizeof(int));
		memcpy(&tamanio_texto, data_entrante->data + 2 * sizeof(int),
				sizeof(int));

		char * texto = malloc(tamanio_texto);
		memcpy(texto, data_entrante + 3 * sizeof(int), tamanio_texto);

		escribir_n(pid_escribir, pagina_escribir, texto, memoriaConfig);

		log_info(logger,
				string_from_format(
						"Se escribío en la página %d del pid %d el texto %s",
						pagina_escribir, pid_escribir, texto));

		break;

	case FINALIZAR:

		log_info(logger,
				string_from_format(
						"Realizando una operación FINALIZAR para el socket %d",
						socket));

		int pid_para_finalizar;

		memcpy(&pid_para_finalizar, data_entrante->data, sizeof(int));

		//False porque no es test
		finalizar(pid, memoriaConfig, false);

		log_info(logger,
				string_from_format("Se finalizó el pid %d",
						pid_para_finalizar));

		break;

	case CERRAR_CONEXION:

		log_info(logger,
				string_from_format("Cerrando conexion en el socket %d",
						socket));

		close(socket);
		FD_CLR(socket, &sockets_activos);
		break;

	default:

		log_info(logger,
				string_from_format(
						"Codigo de operación (%d) no reconocido enviado por el socket %d",
						data_entrante->header->codigo_operacion, socket));
		break;

	}
	free(data_entrante);
	pthread_mutex_unlock(&semaforo_memoria);

}

void levantarConfiguracion() {

	memoriaConfig =
			config_create(
					"/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/src/memoriaConfig");

	loggearInfo("Levantan las configuraciones");

	char variablesConfiguracion[10][50] = { "PUERTO_ESCUCHA", "IP_SWAP",
			"PUERTO_SWAP", "MAXIMO_MARCOS_POR_PROCESO", "CANTIDAD_MARCOS",
			"TAMANIO_MARCO", "ENTRADAS_TLB", "TLB_HABILITADA",
			"RETARDO_MEMORIA", "ALGORITMO_REMPLAZO" };
	int i;
	for (i = 0; i < 10; i++) {

		loggearInfo(
				string_from_format("Configurado con %s: %s",
						variablesConfiguracion[i],
						config_get_string_value(memoriaConfig,
								variablesConfiguracion[i])));

	}
}

