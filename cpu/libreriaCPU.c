/*
 * libreriaCPU.c
 *
 *  Created on: 1/10/2015
 *      Author: utnso
 */

#include "libreriaCPU.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Función que recibe una instrucción y devuelve un identificador para poder usar en el switch. */

int reconocerInstruccion(char *linea) {
	int codigoInstruccion;
	char **palabrasSeparadas;
	palabrasSeparadas = string_split(linea, " ");
	char *palabraClave;
	palabraClave = palabrasSeparadas[0];

	if (!strcmp(palabraClave, "iniciar")) {
		codigoInstruccion = 4;
	} else if (!strcmp(palabraClave, "leer")) {
		codigoInstruccion = 5;
	} else if (!strcmp(palabraClave, "escribir")) {
		codigoInstruccion = 6;
	} else if (!strcmp(palabraClave, "entrada-salida")) {
		codigoInstruccion = 7;
	} else if (!strcmp(palabraClave, "finalizar")) {
		codigoInstruccion = 8;
	}
	return codigoInstruccion;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Función para remover un char en concreto de un string (usado para sacar las comillas de la instruccion escribir) */
void removeChar(char *string, char basura) {
	char *src, *dst;
	for (src = dst = string; *src != '\0'; src++) {
		*dst = *src;
		if (*dst != basura)
			dst++;
	}
	*dst = '\0';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Genera un paquete que contiene el código de operación, y el tamaño a transmitir. */
t_data *crearPaquete(int codigoOperacion, int pid, int paginas) {
	t_data * paquete = malloc(sizeof(t_data));
	paquete->header = malloc(sizeof(t_header));
	paquete->data = malloc(2 * sizeof(int));
	paquete->header->codigo_operacion = codigoOperacion;
	paquete->header->tamanio_data = 8;
	memcpy(paquete->data, &pid, sizeof(int));
	memcpy(paquete->data + 4, &paginas, sizeof(int));
	return paquete;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

t_data *crearPaqueteEscritura(int codigoOperacion, int pid, int paginas,
		char *string) {
	int largoString = strlen(string) + 1;
	t_data * paquete = malloc(sizeof(t_data));
	paquete->header = malloc(sizeof(t_header));
	int tamanio = 3 * sizeof(int) + largoString;
	paquete->data = malloc(tamanio);
	paquete->header->codigo_operacion = codigoOperacion;
	paquete->header->tamanio_data = tamanio;
	memcpy(paquete->data, &pid, sizeof(int));
	memcpy(paquete->data + sizeof(int), &paginas, sizeof(int));
	memcpy(paquete->data + 2 * sizeof(int), &largoString, sizeof(int));
	memcpy(paquete->data + 3 * sizeof(int), string, largoString);
	return paquete;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Función para pasar un archivo .txt a un string. */
char *txtAString(char *rutaDelArchivo) {
	char * buffer = 0;
	long length;
	FILE * f = fopen(rutaDelArchivo, "rb");
	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = malloc(length);
		if (buffer) {
			fread(buffer, 1, length, f);
		}
		fclose(f);
	}
	return buffer;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char* serializarPaqueteCpu(t_data * unPaquete) {
	void * stream = malloc(sizeof(t_header) + unPaquete->header->tamanio_data);

	memcpy(stream, unPaquete->header, sizeof(t_header));
	memcpy(stream + sizeof(t_header), unPaquete->data,
			unPaquete->header->tamanio_data);
	return stream;
}

void common_send(int socket, t_data * paquete) {
	char* buffer;
	int tamanio_total;
	buffer = serializarPaqueteCpu(paquete);
	tamanio_total = paquete->header->tamanio_data + sizeof(t_header);

	send(socket, buffer, tamanio_total, MSG_WAITALL);

	free(buffer);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

t_data * leer_paquete(int socket) {

	t_data * paquete_entrante = malloc(sizeof(t_data));
	paquete_entrante->header = malloc(sizeof(t_header));

	recv(socket, &paquete_entrante->header->codigo_operacion, sizeof(int),
	MSG_WAITALL);
	recv(socket, &paquete_entrante->header->tamanio_data, sizeof(int),
	MSG_WAITALL);

	paquete_entrante->data = malloc(paquete_entrante->header->tamanio_data);

	recv(socket, paquete_entrante->data, paquete_entrante->header->tamanio_data,
	MSG_WAITALL);

	return paquete_entrante;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* funcion para ejecutar las instrucciones */
int ejecutar(char *linea, int serverMemoria, int serverPlanificador,
		char *idProceso, t_hilo *infoHilo) {
	t_hilo *datosDelHilo = (t_hilo *) infoHilo;
	int tamanio, tamanioARecibir, tamanioContenido, resultado,
			estadoDeEjecucion, paginas, header, largoString, codigo,
			codigoRecibido;
	char *contenidoDePagina;
	char *resultadoEjecucion = malloc(128);
	char **array;
	t_data *datos = malloc(sizeof(t_data));
	char *paqueteSerializado, textoAEscribir;
	int clave = reconocerInstruccion(linea);
	int pid = atoi(idProceso);
	int hilo = datosDelHilo->idHilo;
	estadoDeEjecucion = 1; // SOLO PARA TESTEAR
	switch (clave) {

	case 4:									// instrucción iniciar N
		array = string_split(linea, " ");
		int paginas = atoi(array[1]);
		tamanio = sizeof(paginas);
		datos = crearPaquete(clave, pid, paginas);
		tamanio = datos->header->tamanio_data;
		common_send(serverMemoria, datos);

		datos = leer_paquete(serverMemoria);


		if (datos->header->codigo_operacion == 1) {

			log_info(datosDelHilo->logger, string_from_format("[CPU%d] mProc %s - Iniciado correctamente.",datosDelHilo->idHilo, idProceso));
//			logearIniciar(datosDelHilo, datos->header->codigo_operacion, idProceso);
			printf("\n");
			resultado = 1;
		} else {
			log_info(datosDelHilo->logger, string_from_format("[CPU%d] mProc %s - Fallo al iniciar",datosDelHilo->idHilo, idProceso));
			resultado = 0;
			return resultado;
		}

		break;

	case 5:									// instrucción leer N
		array = string_split(linea, " ");
		paginas = atoi(array[1]);
		tamanio = sizeof(paginas);
		datos = crearPaquete(clave, pid, paginas);
		tamanio = datos->header->tamanio_data;
		common_send(serverMemoria, datos);

//		contenidoDePagina = "texto de prueba"; // SOLO PARA TESTEAR
//		estadoDeEjecucion = 1; // SOLO PARA TESTEAR

		datos = leer_paquete(serverMemoria);
		if (datos->header->codigo_operacion == 1) {
//			logearLectura(datosDelHilo, idProceso, array[1], (char *) datos->data);
			contenidoDePagina = datos->data;
			log_info(datosDelHilo->logger, string_from_format("[CPU%d] mProc %s, página %d leída: %s", datosDelHilo->idHilo, idProceso,paginas, contenidoDePagina));
			printf("\n");
			//    		resultadoEjecucion = string_from_format("mProc %s, página %d leída: %s", idProceso, paginas, contenidoDePagina);
			resultado = 1;
		} else {
			log_info(infoHilo->logger,
					string_from_format(
							"CPU%s proceso: %s - error de lectura en la página %s",
							datosDelHilo->idHilo, idProceso, paginas));
//			resultadoEjecucion = string_from_format("mProc %s - error de lectura en la página %s", idProceso, paginas);
			printf("\n");
			resultado = 0;
			return resultado;
		}

		break;

	case 6:							//instrucción escribir N contenido
		array = string_n_split(linea, 3, " ");
		paginas = atoi(array[1]);
		removeChar(array[2], '"');
		datos = crearPaqueteEscritura(clave, pid, paginas, array[2]);
		common_send(serverMemoria, datos);
		datos = leer_paquete(serverMemoria);

		log_info(datosDelHilo->logger, string_from_format("[CPU%d] mProc %s - página %d escrita: %s",
				datosDelHilo->idHilo, idProceso, paginas, array[2]));
//		logearEscritura(datosDelHilo, idProceso, estadoDeEjecucion, array[1], array[2]);
		printf("\n");
		if (datos->header->codigo_operacion == 1) {
//		    resultadoEjecucion = string_from_format("mProc %s - página %s escrita: %s\n", idProceso, array[1], array[2]);
			resultado = 1;
		} else {
//			resultadoEjecucion = string_from_format("mProc %s - Error de escritura de página\n", idProceso);
			resultado = 0;
			return resultado;
		}

		break;

	case 7:								// instrucción entrada-salida T
		array = string_split(linea, " ");
		pid = atoi(idProceso);
		int tiempo = atoi(array[1]);
		codigo = 20;

		resultado = 1;
//		logearEntradaSalida(datosDelHilo, idProceso, array[1]);
		log_info(datosDelHilo->logger, string_from_format("[CPU%d] mProc %s en entrada-salida de tiempo %s",datosDelHilo->idHilo, idProceso, array[1]));
//		resultadoEjecucion = string_from_format("mProc %s - en entrada/salida de tiempo %s\n", idProceso, array[1]);
		printf("\n");
		resultado = 1;

		break;

	case 8:								// instrucción finalizar
		pid = atoi(idProceso);
		datos = pedirPaquete(clave, 4, &pid);
		common_send(serverMemoria, datos);
		resultado = 1;
		log_info(datosDelHilo->logger, string_from_format("[CPU%d] mProc %s - Finalizado.\n", datosDelHilo->idHilo, idProceso));
//		resultadoEjecucion = string_from_format("mProc %s - Finalizado.\n", idProceso);
		printf("\n");
		break;

	};

	return resultado;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
t_data * pedirPaquete(int codigoOp, int tamanio, void * data) {

	t_data * paquete = malloc(sizeof(t_data));
	paquete->header = malloc(sizeof(t_header));

	paquete->header->codigo_operacion = codigoOp;
	paquete->header->tamanio_data = tamanio;
	paquete->data = data;

	return paquete;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Función que recibe la ruta y el contador de programa, y
 ejecuta línea por línea las instrucciones desde donde lo indique el contador.*/

void correrArchivo(void * infoHilo) {

	t_list * instrucciones = list_create();
	t_list * resultados = list_create();

	t_correr_info * infoCorrer = infoHilo;
	t_hilo * dataDelHilo = infoCorrer->threadInfo;
	t_resultadoOperacion resultado;
	resultado.idProceso = atoi(infoCorrer->id);
	resultado.idCpu = dataDelHilo->idHilo;
	int n = infoCorrer->contadorPrograma;
	int retardo = getRetardo();
	int instruccionesMaximasPorMinuto = 60 / (retardo / 1000000);
	int tiempoIO;
	int quantum = infoCorrer->quantum;
	char* archivoEnStrings = txtAString(infoCorrer->path);
	int contadorEjecutadas = 0;

	char **listaInstrucciones = string_split(archivoEnStrings, ";\n");
	int posicionAuxiliar;

	for (posicionAuxiliar = 0;
			posicionAuxiliar < cantidadElementos(listaInstrucciones);
			posicionAuxiliar++) {

		if (listaInstrucciones[posicionAuxiliar] != NULL) {
			list_add(instrucciones, listaInstrucciones[posicionAuxiliar]);
		}

	}

	int nInstruccion = infoCorrer->contadorPrograma;
	bool ultimaNoFueIO = true;
	if (quantum == 0) { //fifo

		while (nInstruccion <= list_size(instrucciones) && ultimaNoFueIO) {

			if(hayQueFinalizar[dataDelHilo->idHilo] == true){
				nInstruccion = list_size(instrucciones) - 1;
				hayQueFinalizar[dataDelHilo->idHilo] = false;
			}
			int operacion = reconocerInstruccion(listaInstrucciones[nInstruccion]);
			bool fueCorrecta = ejecutar(listaInstrucciones[nInstruccion],
					infoCorrer->serverMemoria, infoCorrer->serverPlanificador,
					infoCorrer->id, dataDelHilo);

			t_resultado_ejecucion * resultadoEjecucion = malloc(
					sizeof(t_resultado_ejecucion));

			resultadoEjecucion->instruccion = operacion;
			resultadoEjecucion->resultado = fueCorrecta;

			list_add(resultados,resultadoEjecucion);
			instruccionesEjecutadas[dataDelHilo->idHilo]++;
			porcentajeDeUso[dataDelHilo->idHilo] =
					((instruccionesEjecutadas[dataDelHilo->idHilo] * 100)
							/ instruccionesMaximasPorMinuto);

			if (fueCorrecta) {

				nInstruccion++;

				switch (operacion) {

				case FINALIZAR:

					enviarPaqueteFinalizar(infoCorrer->threadInfo->idHilo,
							infoCorrer->serverPlanificador, resultados);

					log_info(dataDelHilo->logger, string_from_format("[CPU%d] liberada y disponible!", dataDelHilo->idHilo));
					printf("\n");
					return;

				case ENTRADASALIDA:

					ultimaNoFueIO = false;
					tiempoIO = obtenerTiempoIO(listaInstrucciones[nInstruccion - 1]);
					enviarPaqueteEntradaSalida(infoCorrer->threadInfo->idHilo,
							tiempoIO, nInstruccion,
							infoCorrer->serverPlanificador, resultados);

					log_info(dataDelHilo->logger, string_from_format("[CPU%d] liberada y disponible!", dataDelHilo->idHilo));
					printf("\n");
					return;

				}

			} else if (!fueCorrecta) {
				enviarPaqueteError(infoCorrer->threadInfo->idHilo,
						infoCorrer->serverPlanificador);
				log_info(dataDelHilo->logger, string_from_format("[CPU%d] liberada y disponible!", dataDelHilo->idHilo));
				printf("\n");
				return;
			}
			usleep(retardo);
		}

	} else if (quantum != 0) { // Round-Robin

		if(hayQueFinalizar[dataDelHilo->idHilo] == true){
				nInstruccion = list_size(instrucciones) - 1;
				hayQueFinalizar[dataDelHilo->idHilo] = false;
			}

		while (nInstruccion <= list_size(instrucciones) && ultimaNoFueIO
				&& contadorEjecutadas < quantum) {
			int operacion = reconocerInstruccion(listaInstrucciones[nInstruccion]);
			bool fueCorrecta = ejecutar(listaInstrucciones[nInstruccion],
					infoCorrer->serverMemoria, infoCorrer->serverPlanificador,
					infoCorrer->id, dataDelHilo);

			t_resultado_ejecucion * resultadoEjecucion = malloc(
					sizeof(t_resultado_ejecucion));

			resultadoEjecucion->instruccion = operacion;
			resultadoEjecucion->resultado = fueCorrecta;

			list_add(resultados,resultadoEjecucion);


			instruccionesEjecutadas[dataDelHilo->idHilo]++;
			porcentajeDeUso[dataDelHilo->idHilo] =
					((instruccionesEjecutadas[dataDelHilo->idHilo] * 100)
							/ instruccionesMaximasPorMinuto);

			if (fueCorrecta) {

				nInstruccion++;

				switch (operacion) {

				case FINALIZAR:

					enviarPaqueteFinalizar(infoCorrer->threadInfo->idHilo,
							infoCorrer->serverPlanificador, resultados);

					log_info(dataDelHilo->logger, string_from_format("[CPU%d] liberada y disponible!", dataDelHilo->idHilo));
					printf("\n");
					return;

				case ENTRADASALIDA:

					ultimaNoFueIO = false;
					tiempoIO = obtenerTiempoIO(listaInstrucciones[nInstruccion - 1]);
					enviarPaqueteEntradaSalida(infoCorrer->threadInfo->idHilo,
							tiempoIO, nInstruccion,
							infoCorrer->serverPlanificador, resultados);

					log_info(dataDelHilo->logger, string_from_format("[CPU%d] liberada y disponible!", dataDelHilo->idHilo));
					printf("\n");
					return;

				}

			} else if (!fueCorrecta) {
				enviarPaqueteError(infoCorrer->threadInfo->idHilo,
						infoCorrer->serverPlanificador);
				log_info(dataDelHilo->logger, string_from_format("[CPU%d] liberada y disponible!", dataDelHilo->idHilo));
				printf("\n");
				return;

			}
			contadorEjecutadas ++;
			usleep(retardo);
		}
		enviarPaqueteFinQuantum(infoCorrer->threadInfo->idHilo, nInstruccion,
				infoCorrer->serverPlanificador, resultados);
		log_info(dataDelHilo->logger, string_from_format("[CPU%d] liberada y disponible!", dataDelHilo->idHilo));
		printf("\n");
		return;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int obtenerTiempoIO(char *instruccion) {
	int tiempo;
	char ** array = string_split(instruccion, " ");
	tiempo = atoi(array[1]);

	return tiempo;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones para la creación de paquetes al finalizar una ejecución (por I/O, error de ejecución,
// fin de quantum, o instrucción finalizar)

void enviarPaqueteEntradaSalida(int pidCpu, int tiempoIO, int contadorPrograma,
		int socket, t_list * resultados) {

	int cantidadResultados = list_size(resultados);

	void * buffer = malloc(
			4 * sizeof(int)
					+ cantidadResultados * sizeof(t_resultado_ejecucion));

	memcpy(buffer, &pidCpu, sizeof(int));
	memcpy(buffer + sizeof(int), &tiempoIO, sizeof(int));
	memcpy(buffer + 2 * sizeof(int), &contadorPrograma, sizeof(int));
	memcpy(buffer + 3 * sizeof(int), &cantidadResultados, sizeof(int));

	int n = 0;

	void copiarResultado(void * data) {

		t_resultado_ejecucion * resultado = data;

		memcpy(buffer + 4 * sizeof(int) + n * sizeof(int),
				&resultado->instruccion, sizeof(int));
		n++;

		memcpy(buffer + 16 + n * 4, &resultado->resultado, sizeof(int));
		n++;

	}

	list_iterate(resultados, copiarResultado);

	int tamanio = 4 * sizeof(int) + list_size(resultados) * 2 * sizeof(int);

	t_data * paquete = pedirPaquete(20, tamanio, buffer);

	common_send(socket, paquete);
}

void enviarPaqueteError(int idCpu, int socket) {
	void *buffer = malloc(sizeof(int));
	int tamanio = sizeof(int);
	memcpy(buffer, &idCpu, sizeof(int));
	t_data * paquete = pedirPaquete(21, tamanio, buffer);
	common_send(socket, paquete);
}

void enviarPaqueteFinQuantum(int idCpu, int contadorPrograma, int socket,
		t_list * resultados) {
	int cantidadResultados = list_size(resultados);

	void * buffer = malloc(
			3 * sizeof(int)
					+ cantidadResultados * sizeof(t_resultado_ejecucion));

	memcpy(buffer, &idCpu, sizeof(int));

	memcpy(buffer + sizeof(int), &contadorPrograma, sizeof(int));

	memcpy(buffer + 2 * sizeof(int), &cantidadResultados, sizeof(int));

	int n = 0;

	void copiarResultado(void * data) {

		t_resultado_ejecucion * resultado = data;

		memcpy(buffer + 12 + n * 4, &resultado->instruccion, sizeof(int));
		n++;

		memcpy(buffer + 12 + n * 4, &resultado->resultado, sizeof(bool));
		n++;

	}

	list_iterate(resultados, copiarResultado);

	int tamanio = 3 * sizeof(int) +  list_size(resultados) * 2 * sizeof(int);

	t_data * paquete = pedirPaquete(22, tamanio, buffer);

	common_send(socket, paquete);
}

void enviarPaqueteFinalizar(int pidCpu, int socket,
		t_list * resultados) {

	int cantidadResultados = list_size(resultados);

	void * buffer = malloc(
			2 * sizeof(int)
					+ cantidadResultados * sizeof(t_resultado_ejecucion));

	memcpy(buffer, &pidCpu, sizeof(int));
	memcpy(buffer + sizeof(int), &cantidadResultados, sizeof(int));
	int n = 0;

	void copiarResultado(void * data) {

		t_resultado_ejecucion * resultado = data;

		memcpy(buffer + 2 * sizeof(int) + n * sizeof(int),
				&resultado->instruccion, sizeof(int));
		n++;

		memcpy(buffer + 8 + n * 4, &resultado->resultado,sizeof(int));
		n++;

	}
	int tamanio;

	list_iterate(resultados, copiarResultado);

	tamanio = 2 * sizeof(int) +  list_size(resultados)* 2 * sizeof(int);

	t_data * paquete = pedirPaquete(23, tamanio, buffer);

	common_send(socket, paquete);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Función para obtener la cantidad de elementos de un array cuyo último elemento es NULL (para la lista de instrucciones).*/
int cantidadElementos(char **lista) {
	int cantidad = 0;
	int n = 0;
	while (lista[n] != NULL) {
		cantidad++;
		n++;
	}
	return cantidad;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*getters para cada campo del archivo de configuración. */
char *getIpPlanificador() {
	t_config *cpuConfig;
	cpuConfig = config_create("/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * ip = config_get_string_value(cpuConfig, "IP_PLANIFICADOR");
	return ip;
}

char *getPuertoPlanificador() {
	t_config *cpuConfig;
	cpuConfig = config_create("/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * puerto = config_get_string_value(cpuConfig, "PUERTO_PLANIFICADOR");
	return puerto;
}

char *getIpMemoria() {
	t_config *cpuConfig;
	cpuConfig = config_create("/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * ip = config_get_string_value(cpuConfig, "IP_MEMORIA");
	return ip;
}

char *getPuertoMemoria() {
	t_config *cpuConfig;
	cpuConfig = config_create("/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * puerto = config_get_string_value(cpuConfig, "PUERTO_MEMORIA");
	return puerto;
}
int getHilos() {
	t_config *cpuConfig;
	cpuConfig = config_create("/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	int hilos = config_get_int_value(cpuConfig, "CANTIDAD_HILOS");
	return hilos;
}

int getRetardo() {
	t_config *cpuConfig;
	cpuConfig = config_create("/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	int retardo = config_get_int_value(cpuConfig, "RETARDO");
	retardo = retardo * 1000000;
	return retardo;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
t_data *crearPaqueteConsumo(int id, int consumoActual) {
	t_data * paquete = malloc(sizeof(t_data));
	paquete->header = malloc(sizeof(t_header));
	int tamanio = 2 * sizeof(int);
	paquete->data = malloc(tamanio);
	paquete->header->codigo_operacion = 43;
	paquete->header->tamanio_data = tamanio;
	memcpy(paquete->data, &id, sizeof(int));
	memcpy(paquete->data + sizeof(int), &consumoActual, sizeof(int));
	return paquete;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* funcion  que simula una instancia de CPU. */
void *iniciarcpu(void *punteroALaInfo) {

	t_hilo *threadInfo = (t_hilo *) punteroALaInfo;

	char *ip_Planificador, *ip_Memoria, *puerto_Planificador, *puerto_Memoria;
	int serverPlanificador, serverMemoria;
	ip_Planificador = getIpPlanificador();
	ip_Memoria = getIpMemoria();
	puerto_Planificador = getPuertoPlanificador();
	puerto_Memoria = getPuertoMemoria();

	serverPlanificador = conectarCliente(ip_Planificador, puerto_Planificador);
	serverMemoria = conectarCliente(ip_Memoria, puerto_Memoria);
	send(serverPlanificador, &threadInfo->idHilo, sizeof(int), MSG_WAITALL);

	int status = 1;
	printf("\n");
//	log_info(threadInfo->logger, iniciar);
	log_info(threadInfo->logger, string_from_format
			("[CPU%d] iniciada y conectada! Aguardando instrucciones...", threadInfo->idHilo));
	printf("\n");

	t_resultadoOperacion resultado;

	int codigoOperacion, programCounter, pid, tamanio, quantum;

	while (status != 0) {

		recv(serverPlanificador, &codigoOperacion, sizeof(int),MSG_WAITALL);

		if (codigoOperacion == 1) {		// caso Correr Archivo
			recv(serverPlanificador, &programCounter, sizeof(int),
			MSG_WAITALL);
			recv(serverPlanificador, &pid, sizeof(int),
			MSG_WAITALL);
			recv(serverPlanificador, &quantum, sizeof(int),
			MSG_WAITALL);
			recv(serverPlanificador, &tamanio, sizeof(int),
			MSG_WAITALL);
			char * path = malloc(tamanio);
			recv(serverPlanificador, path, tamanio, MSG_WAITALL);
			char *rutaDelArchivo = path;
			int contadorDePrograma = programCounter;
			char *idProceso = string_itoa(pid);

			log_info(threadInfo->logger, string_from_format("[CPU%d] Contexto de ejecución recibido! "
					"Ruta del archivo:%s ; Posicion del contador de Programa:%d ;Id de proceso:%s",
					threadInfo->idHilo,rutaDelArchivo, contadorDePrograma, idProceso));
			printf("\n");

			pthread_t hiloEjecucion;

			t_correr_info * infoCorrer = malloc(sizeof(t_correr_info));

			infoCorrer->contadorPrograma = contadorDePrograma;
			infoCorrer->id = idProceso;
			infoCorrer->path = path;
			infoCorrer->quantum = quantum;
			infoCorrer->serverMemoria = serverMemoria;
			infoCorrer->serverPlanificador = serverPlanificador;
			infoCorrer->threadInfo = threadInfo;

			pthread_create(&hiloEjecucion, NULL, correrArchivo, infoCorrer);

		} else if (codigoOperacion == 3) { // caso Consumo de CPU
			int id = threadInfo->idHilo;
			int consumoActual = porcentajeDeUso[id];
			t_data *paqueteConsumo = crearPaqueteConsumo(id, consumoActual);
			common_send(serverPlanificador, paqueteConsumo);

		} else if(codigoOperacion == 99){ // caso Finalizar

			hayQueFinalizar[threadInfo->idHilo] = true;
		}

		printf("\n");

//		status = 0;

	}

//	close(serverPlanificador);
//	close(serverMemoria);

	return NULL;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char *eventoDeLogeo(char * mensajeALogear, int id) {
	size_t largomensaje, largoid;
	char *mensaje = mensajeALogear;
	char *idHilo = string_itoa(id);
	largomensaje = strlen(mensaje);
	largoid = strlen(idHilo);
	void *buffer = malloc(
			(sizeof(char) * largomensaje) + sizeof(char) * largoid);
	memcpy(buffer, mensaje, largomensaje);
	memcpy(buffer + largomensaje, idHilo, largoid);
	int longReal = largomensaje + largoid;
	char *retorno = string_substring_until(buffer, longReal);
	free(buffer);
	return retorno;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *logeoDeEjecucion(char *mensaje, char *ruta, int contador, char *idProceso) {
	size_t largoMensaje1, largoMensaje2, largoMensaje3, largoRuta,
			largoContador, largoId;
	char *contadorPrograma = string_itoa(contador);
	char **mensajeDesarmado = string_split(mensaje, "|");
	largoMensaje1 = strlen(mensajeDesarmado[0]);
	largoMensaje2 = strlen(mensajeDesarmado[1]);
	largoMensaje3 = strlen(mensajeDesarmado[2]);
	largoRuta = strlen(ruta);
	largoId = strlen(idProceso);
	largoContador = strlen(contadorPrograma);
	void *buffer = malloc(
			(sizeof(char) * largoMensaje1) + (sizeof(char) * largoMensaje2)
					+ (sizeof(char) * largoMensaje3)
					+ (sizeof(char) * largoRuta)
					+ (sizeof(char) * largoContador) + sizeof(char) * largoId);
	memcpy(buffer, mensajeDesarmado[0], largoMensaje1);
	memcpy(buffer + largoMensaje1, ruta, largoRuta);
	memcpy(buffer + largoMensaje1 + largoRuta, mensajeDesarmado[1],
			largoMensaje2);
	memcpy(buffer + largoMensaje1 + largoRuta + largoMensaje2, contadorPrograma,
			largoContador);
	memcpy(buffer + largoMensaje1 + largoRuta + largoMensaje2 + largoContador,
			mensajeDesarmado[2], largoMensaje3);
	memcpy(
			buffer + largoMensaje1 + largoRuta + largoMensaje2 + largoContador
					+ largoMensaje3, idProceso, largoId);
	return (buffer);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void funcionResetearContadores() {
	while (1) {
		sleep(60);
		resetearContadores();
	}
}

void resetearContadores() {
	int i;
	for (i = 0; i <= 50; i++) {
		porcentajeDeUso[i] = 0;
		instruccionesEjecutadas[i] = 0;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
