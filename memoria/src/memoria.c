/*
 * memoria.c
 *  Created on: 28/9/2015
 *      Author: utnso
 */
#include "memoria.h"

int main() {

	test = false;

	iniciarLogger();
	levantarConfiguracion();
	iniciar_marcos();
	registrarSeniales();

	conectarseAlSwap();

	//Inicio el semáforo que blockea todas las operaciones de la memoria
	pthread_mutex_init(&semaforo_memoria, NULL);
	pthread_create(&hiloMostrarTasa, NULL, mostrarTasaTlb, NULL);

	atenderConexiones();

	return 1;
}

void atenderConexion(int socket, fd_set sockets_activos) {

	t_data * data_entrante = leer_paquete(socket);

	switch (data_entrante->header->codigo_operacion) {

	case INICIAR:

		loggearInfo(
				string_from_format(
						"Realizando una operación INICIAR para el socket %d",
						socket));

		int pid, cant_paginas;

		memcpy(&pid, data_entrante->data, sizeof(int));
		memcpy(&cant_paginas, data_entrante->data + sizeof(int), sizeof(int));

		log_info(logger,
				string_from_format(
						"Se intenta iniciar el proceso %d con %d páginas", pid,
						cant_paginas));

		bool exito1 = iniciar_n(pid, cant_paginas);

		t_data * paquete;

		if (!exito1) {

			log_warning(logger, "No se pudo iniciar el programa pedido");

		}

		//Le mando basura como data, ya que lo que le importa es el numeroOperacion
		paquete = pedirPaquete(exito1, sizeof(int), &pid);
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

		log_info(logger,
				string_from_format(
						"Se intenta leer la página %d para el proceso %d",
						pagina_leer, pid_leer));

		bool exito = leer_n(pid_leer, pagina_leer);

		if (!exito) {
			log_warning(logger,
					string_from_format(
							"No se disponian marcos para el proceso %d, se finaliza.",
							pid_leer));
			finalizar(pid);

			contenido_lectura = NULL;
		}

		log_info(logger,
				string_from_format("El resultado de la lectura fué %s",
						contenido_lectura));

		int tamanio_marco = config_get_int_value(memoriaConfig,
				"TAMANIO_MARCO");

		paquete = pedirPaquete(exito, tamanio_marco, contenido_lectura);
		common_send(socket, paquete);

		free(paquete);
		break;

	case ESCRIBIR:
		log_info(logger,
				string_from_format(
						"Realizando una operación ESCRIBIR para el socket %d.",
						socket));

		int pid_escribir, pagina_escribir, tamanio_texto;

		memcpy(&pid_escribir, data_entrante->data, sizeof(int));
		memcpy(&pagina_escribir, data_entrante->data + sizeof(int),
				sizeof(int));
		memcpy(&tamanio_texto, data_entrante->data + 2 * sizeof(int),
				sizeof(int));

		char * texto = malloc(tamanio_texto);
		memcpy(texto, data_entrante->data + 3 * sizeof(int), tamanio_texto);

		log_info(logger,
				string_from_format(
						"Se intenta escribir la página %d para el proceso %d con el contenido %s",
						pagina_escribir, pid_escribir, texto));

		exito = escribir_n(pid_escribir, pagina_escribir, texto);

		if (!exito) {
			log_warning(logger,
					string_from_format(
							"No se disponian marcos para el proceso %d, se finaliza",
							pid_leer));
			finalizar(pid);
		}

		paquete = pedirPaquete(exito, 4, &exito);
		common_send(socket, paquete);

		break;

	case FINALIZAR:

		log_info(logger,
				string_from_format(
						"Realizando una operación FINALIZAR para el socket %d",
						socket));

		int pid_para_finalizar;

		memcpy(&pid_para_finalizar, data_entrante->data, sizeof(int));

		finalizar(pid_para_finalizar);

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

}

void levantarConfiguracion() {

	memoriaConfig = config_create(
			"/tp-2015-2c-signiorcodigo/memoria/src/memoriaConfig");

	loggearInfo("Levantan las configuraciones");

	char variablesConfiguracion[10][50] = { "PUERTO_ESCUCHA", "IP_SWAP",
			"PUERTO_SWAP", "MAXIMO_MARCOS_POR_PROCESO", "CANTIDAD_MARCOS",
			"TAMANIO_MARCO", "ENTRADAS_TLB", "TLB_HABILITADA",
			"RETARDO_MEMORIA", "ALGORITMO_REEMPLAZO" };
	int i;
	for (i = 0; i < 10; i++) {

		loggearInfo(
				string_from_format("Configurado con %s: %s",
						variablesConfiguracion[i],
						config_get_string_value(memoriaConfig,
								variablesConfiguracion[i])));

	}
}

