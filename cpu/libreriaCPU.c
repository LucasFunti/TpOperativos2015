/*
 * libreriaCPU.c
 *
 *  Created on: 1/10/2015
 *      Author: utnso
 */
#include <commons/txt.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>

#include <signiorCodigo/libSockets.h>

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

/* empaquetado para todas las instrucciones salvo la escritura. */
t_instruccion empaquetar(char *instruccionRecibida, char *paginas) {
	t_instruccion instruccionEmpaquetada;
	instruccionEmpaquetada.instruccion = instruccionRecibida;
	instruccionEmpaquetada.cantidadDePaginas = paginas;

	return instruccionEmpaquetada;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* empaquetado para la instruccion de escritura. */
t_instruccionEscritura empaquetarEscritura(char *idProceso, char *paginas,
		char *texto) {
	t_instruccionEscritura escrituraEmpaquetada;
	escrituraEmpaquetada.idProceso = atoi(idProceso);
	escrituraEmpaquetada.paginas = atoi(paginas);
	char *textoSinComillas = texto;
	removeChar(textoSinComillas, '"');
	escrituraEmpaquetada.textoAEscribir = textoSinComillas;

	return escrituraEmpaquetada;
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

char *empaquetarEntradaSalida(){
	char *buffer;
	return buffer;
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
	char **array;
	t_data *datos = malloc(sizeof(t_data));
	char *paqueteSerializado, textoAEscribir;
	int clave = reconocerInstruccion(linea);
	int pid = atoi(idProceso);
	int hilo = datosDelHilo->idHilo;

	switch (clave) {

	case 4:									// instrucción iniciar N
		array = string_split(linea, " ");
		int paginas = atoi(array[1]);
		tamanio = sizeof(paginas);
		datos = crearPaquete(clave, pid, paginas);
		tamanio = datos->header->tamanio_data;
		common_send(serverMemoria, datos);

		datos = leer_paquete(serverMemoria);

//		estadoDeEjecucion = 1; // SOLO PARA TESTEAR



		if (datos->header->codigo_operacion == 1) {
//			printf("mProc %s - Iniciado correctamente.\n", idProceso);
			logearIniciar(datosDelHilo, datos->header->codigo_operacion, idProceso);
			resultado = 1;
		} else {
//			printf("mProc %s - Fallo al iniciar\n", idProceso);
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
		if(datos->header == 1){
			logearLectura(datosDelHilo, idProceso, array[1], (char *) datos->data);
//			printf("\n%s\n", datos->data);
			resultado = 1;
		}
		else{
			log_info(infoHilo->logger,string_from_format("CPU%s proceso: %s - error de lectura en la página %s",
					datosDelHilo->idHilo, idProceso, paginas));
			resultado = 0;
			return resultado;
		}

		break;

	case 6:									//instrucción escribir N contenido
		array = string_n_split(linea, 3, " ");
		paginas = atoi(array[1]);
//		removeChar(textoAEscribir, '"');
		datos = crearPaqueteEscritura(clave, pid, paginas, array[2]);
		//tamanio = datos->header->tamanio_data;
		common_send(serverMemoria, datos);
		datos = leer_paquete(serverMemoria);
		logearEscritura(datosDelHilo, idProceso, estadoDeEjecucion, array[1],
				array[2]);
		 if (datos->data == 1) {
		//	printf("mProc %s - página %s escrita: %s\n", idProceso, array[1], array[2]);
			resultado = 1;
			}
		 else {
//			printf("mProc %s - Error de escritura de página\n", idProceso);
			resultado = 0;
			return resultado;
		}

		break;

	case 7:									// instrucción entrada-salida T
		array = string_split(linea, " ");
		pid = atoi(idProceso);
		int tiempo = atoi(array[1]);
		codigo = 20;

		resultado = 1;
		logearEntradaSalida(datosDelHilo, idProceso, array[1]);
//		printf("mProc %s - en entrada/salida de tiempo %s\n", idProceso, array[1]);
		resultado = 1;

		break;

	case 8:								// instrucción finalizar
		pid = atoi(idProceso);
		datos = pedirPaquete(clave, 4, &pid);
		common_send(serverMemoria, datos);
		resultado = 1;
		logearFinalizacion(datosDelHilo, idProceso);
//		printf("mProc %s - Finalizado.\n", idProceso);
		break;

	};

	return resultado;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
t_data * pedirPaquete(int codigoOp, int tamanio, void * data) {

	t_data * paquete = malloc(sizeof(t_data));
	paquete->header = malloc(sizeof(t_header));

	paquete->header->codigo_operacion = codigoOp;
	paquete->header->tamanio_data = tamanio;
	paquete->data = data;

	return paquete;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Funciones para el logeo de resultados de ejecuciones */

void logearIniciar(t_hilo *infoHilo, int estadoDeEjecucion, char *idProceso) {
	char *mensaje = string_new();
	char *id = string_itoa(infoHilo->idHilo);
	string_append(&mensaje, "CPU ");
	string_append(&mensaje, id);
	if (estadoDeEjecucion == 0) {
		string_append(&mensaje, ": error al iniciar el proceso ");
		string_append(&mensaje, idProceso);
		log_error(infoHilo->logger, mensaje);
	} else if (estadoDeEjecucion == 1) {
		string_append(&mensaje, ", proceso ");
		string_append(&mensaje, idProceso);
		string_append(&mensaje, " iniciado correctamente.");
		log_info(infoHilo->logger, mensaje);
	}
}

void logearLectura(t_hilo *infoHilo, char *idProceso, char *pagina,
		char *contenidoDePagina) {

	char *id = string_itoa(infoHilo->idHilo);
	log_info(infoHilo->logger,
			string_from_format(
					"[CPU:%s] El proceso: %s leyo '%s' en la pagina %s", id,
					idProceso, contenidoDePagina, pagina));

}

void logearEscritura(t_hilo *infoHilo, char *idProceso, int estadoDeEjecucion,
		char *pagina, char *texto) {
	char *mensaje = string_new();
	char *id = string_itoa(infoHilo->idHilo);
	string_append(&mensaje, "CPU");
	string_append(&mensaje, id);
	string_append(&mensaje, ", proceso ");
	string_append(&mensaje, idProceso);
	if (estadoDeEjecucion == 0) {
		string_append(&mensaje, ": error de escritura.");
		log_error(infoHilo->logger, mensaje);
	} else if (estadoDeEjecucion == 1) {
		string_append(&mensaje, ": página ");
		string_append(&mensaje, pagina);
		string_append(&mensaje, " escrita: ");
		string_append(&mensaje, texto);
		log_info(infoHilo->logger, mensaje);
	}
}

void logearEntradaSalida(t_hilo *infoHilo, char *idProceso, char *tiempo) {
	char *mensaje = string_new();
	char *id = string_itoa(infoHilo->idHilo);
	string_append(&mensaje, "CPU");
	string_append(&mensaje, id);
	string_append(&mensaje, ", proceso ");
	string_append(&mensaje, idProceso);
	string_append(&mensaje, " en entrada/salida de tiempo ");
	string_append(&mensaje, tiempo);
	log_info(infoHilo->logger, mensaje);
}

void logearFinalizacion(t_hilo *infoHilo, char *idProceso) {
	char *mensaje = string_new();
	char *id = string_itoa(infoHilo->idHilo);
	string_append(&mensaje, "CPU");
	string_append(&mensaje, id);
	string_append(&mensaje, ", proceso ");
	string_append(&mensaje, idProceso);
	string_append(&mensaje, " finalizado.");
	log_info(infoHilo->logger, mensaje);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Función que recibe la ruta y el contador de programa, y
 ejecuta línea por línea las instrucciones desde donde lo indique el contador.*/

t_resultadoOperacion correrArchivo(char *ruta, int contadorPrograma,
		char* idProceso, int serverMemoria, int serverPlanificador,
		t_hilo *infoDelHilo, int quantum) {
	t_hilo *dataDelHilo = (t_hilo *) infoDelHilo;
	t_resultadoOperacion resultado;
	resultado.idProceso = atoi(idProceso);
	resultado.idCpu = dataDelHilo->idHilo;
	int n = contadorPrograma;
	int retardo = getRetardo();

	char* archivoEnStrings = txtAString(ruta);

	char **listaInstrucciones = string_split(archivoEnStrings, ";\n");
	retardo = getRetardo();
	resultado.maximo = cantidadElementos(listaInstrucciones);
	resultado.m = 0;
	resultado.data = malloc(sizeof(int) * resultado.maximo);
	int instruccionesMaximasPorMinuto = 60 / retardo;

	int operacion = reconocerInstruccion(listaInstrucciones[n]);
	if (quantum == 0) { // Caso FIFO
		while ((listaInstrucciones[n] != NULL) && (operacion != 7)) {

			operacion = reconocerInstruccion(listaInstrucciones[n]);
			resultado.data[resultado.m] = ejecutar(listaInstrucciones[n],
					serverMemoria, serverPlanificador, idProceso, dataDelHilo);
			if (resultado.data[resultado.m] == 0) {

				resultado.contador = n;
				resultado.causa_finalizacion = 21;
				resultado.estado_ultima_instruccion = resultado.data[resultado.m];
				t_data *paquete = crearPaqueteFinalizar(resultado);
				common_send(serverPlanificador, paquete);
				return resultado;
			}
			n++;
			resultado.m++;
			instruccionesEjecutadas[infoDelHilo->idHilo]++;
			porcentajeDeUso[infoDelHilo->idHilo] = ((instruccionesEjecutadas[infoDelHilo->idHilo] * 100)
							/ instruccionesMaximasPorMinuto);
			usleep(retardo * 1000000);

		}
		resultado.contador = n;
		if (operacion == 7) {
			resultado.causa_finalizacion = 20;
			char ** array = string_split(listaInstrucciones[n-1], " ");
			resultado.tiempoIO = atoi(array[1]);
			t_data *paquete = crearPaqueteEntradaSalida(resultado);
			common_send(serverPlanificador, paquete);

		} else
			resultado.causa_finalizacion = 23;
			t_data *paquete = crearPaqueteFinalizar(resultado);
			common_send(serverPlanificador, paquete);


	} else if (quantum != 0) { // Caso Round-Robin
		int contador = 0;
		while ((listaInstrucciones[n] != NULL) && (operacion != 7)
				&& contador < quantum) {

			operacion = reconocerInstruccion(listaInstrucciones[n]);
			resultado.data[resultado.m] = ejecutar(listaInstrucciones[n],
					serverMemoria, serverPlanificador, idProceso, dataDelHilo);
			if (resultado.data[resultado.m] == 0) {
				resultado.contador = n;
				resultado.causa_finalizacion = 21;
				t_data *paquete = crearPaqueteFinalizar(resultado);
				common_send(serverPlanificador, paquete);

				return resultado;
			}
			n++;
			resultado.m++;
			instruccionesEjecutadas[infoDelHilo->idHilo]++;
			porcentajeDeUso[infoDelHilo->idHilo] =
					((instruccionesEjecutadas[infoDelHilo->idHilo] * 100)
							/ instruccionesMaximasPorMinuto);
			usleep(retardo * 1000000);
			contador++;
		}

		resultado.m = resultado.m - 1;
		resultado.contador = n;
		if (operacion == 7) {
			resultado.causa_finalizacion = 20;
			resultado.estado_ultima_instruccion = 1;
			char ** array = string_split(listaInstrucciones[n-1], " ");
			resultado.tiempoIO = atoi(array[1]);
			t_data *paquete = crearPaqueteEntradaSalida(resultado);
			common_send(serverPlanificador, paquete);

		} else if(contador < quantum){
			resultado.causa_finalizacion = 23;
			resultado.estado_ultima_instruccion = 1;
			t_data *paquete = crearPaqueteFinalizar(resultado);
			common_send(serverPlanificador, paquete);

		} else {
			resultado.causa_finalizacion = 22;
			resultado.estado_ultima_instruccion = 1;
			t_data *paquete = crearPaqueteFinQuantum(resultado);
			common_send(serverPlanificador, paquete);
		}


	}
	return resultado;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Funciones para la creación de paquetes al finalizar una ejecución (por I/O, error de ejecución,
// fin de quantum, o instrucción finalizar)

t_data *crearPaqueteEntradaSalida(t_resultadoOperacion resultado){
	t_data * paquete = malloc(sizeof(t_data));
	paquete->header = malloc(sizeof(t_header));
	int tamanio = 2 * sizeof(int);
	paquete->data = malloc(tamanio);
	memcpy(paquete->data, &resultado.idCpu, sizeof(int));
	memcpy(paquete->data + sizeof(int), &resultado.tiempoIO, sizeof(int));
	memcpy(paquete->header->codigo_operacion, &resultado.causa_finalizacion, sizeof(int));
	memcpy(paquete->header->tamanio_data, &tamanio, sizeof(int));

	return paquete;
}

t_data *crearPaqueteFinQuantum(t_resultadoOperacion resultado){
	t_data * paquete = malloc(sizeof(t_data));
	paquete->header = malloc(sizeof(t_header));
	int tamanio = (sizeof(int));
	paquete->data =malloc(tamanio);
	memcpy(paquete->data, &resultado.idCpu, sizeof(int));
	memcpy(paquete->header->codigo_operacion, &resultado.causa_finalizacion, sizeof(int));
	memcpy(paquete->header->tamanio_data, &tamanio,sizeof(int));

	return paquete;
}

t_data *crearPaqueteFinalizar(t_resultadoOperacion resultado){
	t_data * paquete = malloc(sizeof(t_data));
	paquete->header = malloc(sizeof(t_header));
	int tamanio = 4 * sizeof(int);
	paquete->data = malloc(tamanio);
	memcpy(paquete->data, &resultado.m, sizeof(int));
	memcpy(paquete->data + sizeof(int), &resultado.idCpu, sizeof(int));
	memcpy(paquete->data + 2 * sizeof(int), &resultado.contador, sizeof(int));
	memcpy(paquete->data + 3 * sizeof(int), &resultado.estado_ultima_instruccion, sizeof(int));
	memcpy(paquete->header->codigo_operacion, &resultado.causa_finalizacion, sizeof(int));
	memcpy(paquete->header->tamanio_data, &tamanio, sizeof(int));

	return paquete;
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
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * ip = config_get_string_value(cpuConfig, "IP_PLANIFICADOR");
	return ip;
}

char *getPuertoPlanificador() {
	t_config *cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * puerto = config_get_string_value(cpuConfig, "PUERTO_PLANIFICADOR");
	return puerto;
}

char *getIpMemoria() {
	t_config *cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * ip = config_get_string_value(cpuConfig, "IP_MEMORIA");
	return ip;
}

char *getPuertoMemoria() {
	t_config *cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	char * puerto = config_get_string_value(cpuConfig, "PUERTO_MEMORIA");
	return puerto;
}
int getHilos() {
	t_config *cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	int hilos = config_get_int_value(cpuConfig, "CANTIDAD_HILOS");
	return hilos;
}

int getRetardo() {
	t_config *cpuConfig;
	cpuConfig = config_create(
			"/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/cpuConfig");
	int retardo = config_get_int_value(cpuConfig, "RETARDO");
	return retardo;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
t_data *crearPaqueteConsumo(int id, int consumoActual){
	t_data * paquete = malloc(sizeof(t_data));
	paquete->header = malloc(sizeof(t_header));
	int tamanio = 2 * sizeof(int);
	paquete->data = malloc(tamanio);
	memcpy(paquete->data, &id, sizeof(int));
	memcpy(paquete->data + sizeof(int), &consumoActual, sizeof(int));
	memcpy(paquete->header->tamanio_data, &tamanio, sizeof(int));

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
	if (serverMemoria == -1) {
		exit(-1);
	}

//	char package[PACKAGESIZE];
	int status = 1;
	char *iniciar = eventoDeLogeo("iniciada CPU ", threadInfo->idHilo);
	printf("\n");
	log_info(threadInfo->logger, iniciar);
	printf("\n");

	t_resultadoOperacion resultado;

	while (status != 0) {


		 int codigoOperacion,programCounter,pid,tamanio,quantum;
		 recv(serverPlanificador,&codigoOperacion, sizeof(int) , MSG_WAITALL);
		 recv(serverPlanificador,&programCounter, sizeof(int) , MSG_WAITALL);
		 recv(serverPlanificador,&pid, sizeof(int) , MSG_WAITALL);
		 recv(serverPlanificador,&quantum, sizeof(int) , MSG_WAITALL);
		 recv(serverPlanificador,&tamanio, sizeof(int) , MSG_WAITALL);

		 char * path = malloc(sizeof(tamanio));
		 recv(serverPlanificador,path, sizeof(int) , MSG_WAITALL);


		 if (status != 0) {
		 char * dataBuffer = malloc(tamanio);
		 recv(serverPlanificador, dataBuffer, tamanio, 0);
		 path = malloc(tamanio);
		 memcpy(path, dataBuffer,tamanio);
		 free(dataBuffer);


		 if(codigoOperacion == 0){		// caso Correr Archivo
		 char *rutaDelArchivo = path;
		 int contadorDePrograma = programCounter;
		 char *idProceso = string_itoa(pid);

		 char *contextoRecibido = logeoDeEjecucion("Contexto de ejecución recibido. Ruta del archivo:| ; Posicion del contador de Programa:| ;Id de proceso:|",
		 rutaDelArchivo, contadorDePrograma, idProceso);
		 log_info(threadInfo->logger, contextoRecibido);
		 printf("%s", rutaDelArchivo);
		 t_resultadoOperacion resultado = correrArchivo(rutaDelArchivo, contadorDePrograma, idProceso, serverMemoria,
		 serverPlanificador,threadInfo, quantum);


/*
		char * rutaDelArchivo = "/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/Debug/memoria";
		int contadorDePrograma = 0;
		char *idProceso = "1";
		int quantum = 0;

		printf("\n");

		resultado = correrArchivo(rutaDelArchivo, contadorDePrograma, idProceso,
				serverMemoria, 456, threadInfo, quantum);

		printf(
				"\noperación exitosa. El resultado de la ejecución fue el siguiente:\nposición actual del contador: %d\n",
				resultado.contador);
		printf("uso de cpu: %d\n", porcentajeDeUso[threadInfo->idHilo]); */

		 }
		else if(codigoOperacion == 1){	// caso Finalizar PID
			int cantidadInstrucciones = cantidadElementos(path);
			programCounter = cantidadInstrucciones - 1;
		 }

		 else if(codigoOperacion == 2){ // caso Consumo de CPU
			 int id = threadInfo->idHilo;
			 int consumoActual = resultado.usoDeCpu;
			 t_data *paqueteConsumo = crearPaqueteConsumo(id, consumoActual);
			 common_send(serverPlanificador, paqueteConsumo);
		 };

		printf("\n");
		char *disponible = eventoDeLogeo("liberada la CPU ",
				threadInfo->idHilo);
		log_info(threadInfo->logger, disponible);
//		printf("PC %d liberada y disponible\n", threadInfo->idHilo);
//		status = 0;
	}

	}

//	close(serverPlanificador);
//	close(serverMemoria);

		printf("\nejecución exitosa. Finalizando...\n");
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
/*función de testeo. */
/*
void testCpuFunction(char *accion) {
//	printf("ingrese la accion a testear, o escriba 'ayuda' para ver comandos disponibles:\n");
//	scanf("%[^\n]%*c", accion);
	if (strcmp(accion, "reconocer instruccion") == 0) {
		int respuesta;
		printf("ingrese la instruccion a reconocer\n");
		char *dataAdicional = malloc(sizeof(char) * 32);
		scanf("%[^\n]%*c", dataAdicional);
		respuesta = reconocerInstruccion(dataAdicional);
		printf("el header correspondiente a la instruccion es: %d\n",
				respuesta);

	} else if (strcmp(accion, "empaquetado normal") == 0) {
		printf(
				"ingrese la instruccion a empaquetar(formato aceptado: instruccion paginas):\n");
		char *dataAdicional = malloc(sizeof(char) * 64);
		scanf("%[^\n]%*c", dataAdicional);
		t_instruccion paquete;
		char **algo;
		algo = string_split(dataAdicional, " ");
		paquete = empaquetar(algo[0], algo[1]);
		printf("la instruccion es: %s\n", paquete.instruccion);
		printf("el número de página/proceso es: %s\n",
				paquete.cantidadDePaginas);

	} else if (strcmp(accion, "empaquetado de escritura") == 0) {
		printf(
				"ingrese la instruccion a empaquetar(formato aceptado: id pagina contenido):\n");
		char *dataAdicional = malloc(sizeof(char) * 64);
		fgets(dataAdicional, 64, stdin);
		t_instruccionEscritura paquete;
		char **algo;
		algo = string_n_split(dataAdicional, 3, " ");
		paquete = empaquetarEscritura(algo[0], algo[1], algo[2]);
		printf("el proceso es: %d\n", paquete.idProceso);
		printf("el número de página es: %d\n", paquete.paginas);
		printf("el contenido a escribir es: %s\n", paquete.textoAEscribir);

	} else if (strcmp(accion, "archivo a texto") == 0) {
		printf("ingrese una ruta de archivo válida:\n");
		char *dataAdicional = malloc(sizeof(char) * 64);
		scanf("%[^\n]%*c", dataAdicional);
		char *texto = txtAString(dataAdicional);
		printf("el contenido del archivo es: %s\n", texto);

	} else if (strcmp(accion, "ejecutar instruccion") == 0) {
		printf("ingrese la instruccion a ejecutar:\n");
		char *dataAdicional = malloc(sizeof(char) * 64);
		char *idProceso = malloc(sizeof(char) * 12);
		char *id;
		t_hilo infoHilo;
		infoHilo.idHilo = 1;
		remove("log_cpu");
		t_log *log_cpu = log_create("log_cpu", "CPU", true, LOG_LEVEL_INFO);
		scanf("%[^\n]%*c", dataAdicional);
		printf("ingrese una id de proceso:\n");
		scanf("%[^\n]%*c", idProceso);
		id = idProceso;
		int resultado = ejecutar(dataAdicional, 123, 456, id, &infoHilo);
		printf("el resultado fue: %d\n", resultado);

	} else if (strcmp(accion, "correr archivo") == 0) {
		char *ruta;
		int contadorPrograma = 0;
		printf("ingrese una ruta de archivo válida:\n");
		char *dataAdicional = malloc(sizeof(char) * 64);
		scanf("%[^\n]%*c", dataAdicional);
		ruta = dataAdicional;
		//		printf("ingrese un valor inicial para el contador de programa:\n");
		//		char *dataAdicionalDos = malloc(sizeof(char) *6);
		//		scanf ("%[^\n]%*c", dataAdicionalDos);
		//		contadorPrograma = atoi(dataAdicionalDos);
		t_hilo infoHilo;
		infoHilo.idHilo = 1;
		remove("log_cpu");
		t_log *log_cpu = log_create("log_cpu", "CPU", true, LOG_LEVEL_INFO);
		infoHilo.logger;
		t_resultadoOperacion resultado = correrArchivo(ruta, contadorPrograma,
				"1", 123, 456, &infoHilo, 0);
		printf("uso de cpu: %d\n", resultado.usoDeCpu);

	} else if (strcmp(accion, "iniciar cpu") == 0) {
		t_log *log_cpu = log_create("log_cpu", "CPU", true, LOG_LEVEL_INFO);
		t_hilo hiloInfo;
		hiloInfo.idHilo = 1;
		hiloInfo.logger = log_cpu;
		t_hilo *puntero = malloc(sizeof(t_hilo));
		memcpy(puntero, &hiloInfo, sizeof(t_hilo));
		iniciarcpu(puntero);

	} else if (strcmp(accion, "ayuda") == 0) {
		printf(
				"lista de comandos disponibles:\nreconocer instruccion\nempaquetado normal\nempaquetado de escritura\n"
						"empaquetar resultado\narchivo a texto\nserializacion normal\nserializacion con escritura\n"
						"serializar resultado\nejecutar instruccion\ncorrer archivo\niniciar cpu\ngenerar hilo\n"
						"generar dos hilos\ngenerar n hilos\n\n");
		printf(
				"para las opciones 'reconocer instruccion' y 'ejecutar instrucción' utilice las siguientes instrucciones:\n"
						"iniciar N\nleer N\nescribir N contenidoAEscribir\nentrada-salida Tiempo\nfinalizar\n");

	} else if (strcmp(accion, "generar hilo") == 0) {
		t_log *log_cpu = log_create("log_cpu", "CPU", true, LOG_LEVEL_INFO);
		t_hilo hiloInfo;
		hiloInfo.idHilo = 1;
		hiloInfo.logger = log_cpu;
		t_hilo *puntero = malloc(sizeof(t_hilo));
		memcpy(puntero, &hiloInfo, sizeof(t_hilo));

		pthread_t hilo1;
		pthread_create(&hilo1, NULL, iniciarcpu, (void *) &hiloInfo);
		pthread_join(hilo1, NULL);

	} else if (strcmp(accion, "generar n hilos") == 0) {
		t_log *log_cpu = log_create("log_cpu", "CPU", true, LOG_LEVEL_INFO);
		t_hilo *infoHilo;
		struct t_hilo *puntero = malloc(sizeof(t_hilo));
		int i;
		int cantHilos = getHilos();
		pthread_t hilos[cantHilos], hiloContador;
		pthread_create(&hiloContador, NULL, funcionResetearContadores, NULL);
		for (i = 0; i < cantHilos; i++) {
			infoHilo = malloc(sizeof(t_hilo));
			infoHilo->idHilo = i;
			infoHilo->logger = log_cpu;
			pthread_create(&hilos[i], NULL, iniciarcpu, infoHilo);
		};
		for (i = 0; i < cantHilos; i++) {
			pthread_join(hilos[i], NULL);
		};
		pthread_join(hiloContador, NULL);
	} else {
		printf("comando desconocido. por favor, intente de nuevo.\n");
	}
} */
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
