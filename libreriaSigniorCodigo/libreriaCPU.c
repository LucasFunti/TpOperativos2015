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
#include "libreriaCPU.h"
#include "planificadorFunctions.h"
#include <stdlib.h>
#include <string.h>
#include "libSockets.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>

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
t_instruccionEscritura empaquetarEscritura(char *idProceso,
		char *paginas, char *texto) {
	t_instruccionEscritura escrituraEmpaquetada;
	escrituraEmpaquetada.idProceso = atoi(idProceso);
	escrituraEmpaquetada.paginas = atoi(paginas);
	char *textoSinComillas = texto;
	removeChar(textoSinComillas, '"');
	escrituraEmpaquetada.textoAEscribir = textoSinComillas;

	return escrituraEmpaquetada;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void removeChar(char *string, char basura) {
	char *src, *dst;
    for (src = dst = string; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != basura) dst++;
    }
    *dst = '\0';
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Empaquetado para los resultados de ejecución de los procesos. */
t_resultadoEjecucion empaquetarResultado(char *ruta, int contadorDePrograma) {
	t_resultadoEjecucion paquete;
	paquete.nombrePrograma = ruta;
	paquete.programCounter = contadorDePrograma;
	return paquete;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* serializacion para todas las instrucciones salvo la de escritura.*/
char *serializarEmpaquetado(t_instruccion instruccionEmpaquetada) {
	size_t numeroDePaginas;
	char *paginas = instruccionEmpaquetada.cantidadDePaginas;
	string_append(&paginas, "\0");
	numeroDePaginas = strlen(paginas);
	void *buffer = malloc((sizeof(char) * numeroDePaginas));
	memcpy(buffer, paginas, numeroDePaginas);
	return buffer;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* serializacion para la escritura */
char *serializarEmpaquetadoEscritura(t_instruccionEscritura instruccionEmpaquetada) {
	size_t tamanioTexto;
	tamanioTexto = strlen(instruccionEmpaquetada.textoAEscribir);
	void *buffer = malloc((2 * sizeof(int)) + sizeof(char) * tamanioTexto);
	memcpy(buffer, &instruccionEmpaquetada.idProceso, sizeof(int));
	memcpy(buffer + sizeof(int), &instruccionEmpaquetada.paginas, sizeof(int));
	memcpy(buffer + (2 * sizeof(int)), instruccionEmpaquetada.textoAEscribir, tamanioTexto);

	return buffer;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Serialización del resultado de ejecución de un proceso. */
char *serializarResultado(t_resultadoEjecucion resultado) {
	size_t tamanioContador, tamanioTexto;
	char* contadorDePrograma = string_itoa(resultado.programCounter);
	char* programa = resultado.nombrePrograma;
	string_append(&contadorDePrograma, "\0");
	string_append(&programa, "\0");
	tamanioContador = strlen(contadorDePrograma);
	tamanioTexto = strlen(programa);
	void *buffer = malloc(
			(sizeof(char) * tamanioTexto) + (sizeof(char) * tamanioContador));
	memcpy(buffer, programa, tamanioTexto);
	memcpy(buffer + tamanioTexto, contadorDePrograma, tamanioContador);
	return buffer;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Genera un header que contiene el código de operación, y el tamaño a transmitir. */
char *generarHeader(int clave, int tamanio){
	size_t tamanioClave, longitudTamanio;
	char* claveString = string_itoa(clave);
	char *tamanioString = string_itoa(tamanio);
	tamanioClave = strlen(claveString);
	longitudTamanio = strlen(tamanioString);
	void *buffer = malloc((sizeof(char) * tamanioClave) + (sizeof(char) * longitudTamanio));
	memcpy(buffer, claveString, tamanioClave);
	memcpy(buffer + tamanioClave, tamanioString, longitudTamanio);
	int largoReal = strlen(buffer) - 1;
	char *header = string_substring_until(buffer, largoReal);
	free(buffer);
	return header;
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

/* funcion para ejecutar las instrucciones */
int ejecutar(char *linea, int serverMemoria, int serverPlanificador, char *idProceso, t_hilo infoHilo) {
	int tamanio, tamanioARecibir, tamanioContenido, resultado, estadoDeEjecucion, paginas, header;
	char *contenidoDePagina = malloc(256); /* ver tamañano real mas adelante. */
	char **array;
	t_instruccion paquete;
	t_instruccionEscritura paqueteEscritura;
	char *paqueteSerializado;
	int clave = reconocerInstruccion(linea);
	int id;

	switch (clave) {

	case 4:									// instrucción iniciar N
		array = string_split(linea, " ");
		id = atoi(idProceso);
		int paginas = atoi(array[1]);
		tamanio = sizeof(paginas);
		send(serverMemoria, &clave, sizeof(int), 0);
		send(serverMemoria, &id, sizeof(int), 0);
		send(serverMemoria, &paginas, tamanio, 0);

//		recv(serverMemoria, &estadoDeEjecucion, sizeof(int), 0);

		estadoDeEjecucion = 1; // SOLO PARA TESTEAR
		logearIniciar(infoHilo, estadoDeEjecucion, idProceso);
		if(estadoDeEjecucion == 0){
			printf("mProc %s - Fallo al iniciar\n", idProceso);
			resultado = 0;
		}else if (estadoDeEjecucion == 1){
			printf("mProc %s - Iniciado correctamente.\n", idProceso);
			resultado = 1;
		}

		break;

	case 5:									// instrucción leer N
		array = string_split(linea, " ");
		paginas = atoi(array[1]);
		tamanio = sizeof(paginas);
		id = atoi(idProceso);
		send(serverMemoria, &id, sizeof(int) + tamanio, 0);
		send(serverMemoria, &paginas, tamanio, 0);
//		recv(serverMemoria, &estadoDeEjecucion, sizeof(int), 0);


		contenidoDePagina = "texto de prueba"; // SOLO PARA TESTEAR
		estadoDeEjecucion = 1; // SOLO PARA TESTEAR
		logearLectura(infoHilo, idProceso, estadoDeEjecucion, array[1], contenidoDePagina);
		if(estadoDeEjecucion == 0){
			printf("mProc %s - Error de lectura de página\n", idProceso);
			resultado = 0;
		}else if(estadoDeEjecucion == 1){
//		    recv(serverMemoria,	&tamanioARecibir, sizeof(int), 0);
//			recv(serverMemoria, &contenidoDePagina, sizeof(char) * tamanioARecibir, o);
			printf("mProc %s, página %s leida: %s\n", idProceso, array[1], contenidoDePagina);
			resultado = 1;
		}


		break;

	case 6:										//instrucción escribir N contenido
		array = string_n_split(linea, 3, " ");
		paqueteEscritura = empaquetarEscritura(idProceso, array[1], array[2]);
		paqueteSerializado = serializarEmpaquetadoEscritura(paqueteEscritura);
		tamanio = strlen(paqueteSerializado);
		send(serverMemoria, &clave, sizeof(int), 0);
		send(serverMemoria, paqueteSerializado, tamanio, 0);
//		recv(serverMemoria, &estadoDeEjecucion, sizeof(int), 0);

		estadoDeEjecucion = 1; // SOLO PARA TESTEAR
		logearEscritura(infoHilo, idProceso, estadoDeEjecucion, array[1], array[2]);
		if(estadoDeEjecucion == 0){
			printf("mProc %s - Error de escritura de página\n", idProceso);
			resultado = 0;
		}else if (estadoDeEjecucion == 1){
			printf("mProc %s - página %s escrita: %s\n", idProceso, array[1], array[2]);
			resultado = 1;
		}

		break;

	case 7:									// instrucción entrada-salida T
		array = string_split(linea, " ");
		paquete = empaquetar(array[0], array[1]);
		paqueteSerializado = serializarEmpaquetado(paquete);
		id = atoi(idProceso);
		int tiempo = atoi(array[1]);
		tamanio = strlen(paqueteSerializado);
		send(serverPlanificador, &id, sizeof(int), 0);
		send(serverPlanificador, &tiempo, tamanio, 0);
		resultado = 1;
		logearEntradaSalida(infoHilo, idProceso, array[1]);
		printf("mProc %s - en entrada/salida de tiempo %s\n", idProceso, array[1]);
		resultado = 1;

		break;

	case 8:								// instrucción finalizar
		id = atoi(idProceso);
		send(serverMemoria, &clave, sizeof(int), 0);
		send(serverMemoria, &id, sizeof(int), 0);
		resultado = 1;
		logearFinalizacion(infoHilo, idProceso);
		printf("mProc %s - Finalizado correctamente.\n", idProceso);
		resultado = 1;
		break;

	};
	return resultado;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Funciones para el logeo de las ejecuciones */

void logearIniciar(t_hilo infoHilo, int estadoDeEjecucion, char *idProceso){
	char *mensaje = string_new();
	char *id = string_itoa(infoHilo.idHilo);
	string_append(&mensaje, "CPU ");
	string_append(&mensaje, id);
	if(estadoDeEjecucion == 0){
		string_append(&mensaje, ": error al iniciar el proceso ");
		string_append(&mensaje, idProceso);
		log_error(infoHilo.logger, mensaje);
	}
	else if(estadoDeEjecucion == 1){
		string_append(&mensaje, ", proceso ");
		string_append(&mensaje, idProceso);
		string_append(&mensaje, " iniciado correctamente.");
		log_info(infoHilo.logger, mensaje);
	}
}

void logearLectura(t_hilo infoHilo, char *idProceso, int estadoDeEjecucion, char *pagina, char *contenidoDePagina){
	char *mensaje = string_new();
	char *id = string_itoa(infoHilo.idHilo);
	string_append(&mensaje, "CPU");
	string_append(&mensaje, id);
	string_append(&mensaje, ", proceso ");
	string_append(&mensaje, idProceso);
	if(estadoDeEjecucion == 0){
		string_append(&mensaje, ": error de lectura.");
		log_error(infoHilo.logger, mensaje);
	}
	else if(estadoDeEjecucion == 1){
		string_append(&mensaje, ": página ");
		string_append(&mensaje, pagina);
		string_append(&mensaje, " leída: ");
		string_append(&mensaje, contenidoDePagina);
		log_info(infoHilo.logger, mensaje);
	}
}

void logearEscritura(t_hilo infoHilo, char *idProceso, int estadoDeEjecucion, char *pagina, char *texto){
	char *mensaje = string_new();
	char *id = string_itoa(infoHilo.idHilo);
	string_append(&mensaje, "CPU");
	string_append(&mensaje, id);
	string_append(&mensaje, ", proceso ");
	string_append(&mensaje, idProceso);
	if(estadoDeEjecucion == 0){
		string_append(&mensaje, ": error de escritura.");
		log_error(infoHilo.logger, mensaje);
	}
	else if (estadoDeEjecucion == 1){
		string_append(&mensaje, ": página ");
		string_append(&mensaje, pagina);
		string_append(&mensaje, " escrita: ");
		string_append(&mensaje, texto);
		log_info(infoHilo.logger, mensaje);
	}
}

void logearEntradaSalida(t_hilo infoHilo, char *idProceso, char *tiempo){
	char *mensaje = string_new();
	char *id = string_itoa(infoHilo.idHilo);
	string_append(&mensaje, "CPU");
	string_append(&mensaje, id);
	string_append(&mensaje, ", proceso ");
	string_append(&mensaje, idProceso);
	string_append(&mensaje, " en entrada/salida de tiempo ");
	string_append(&mensaje, tiempo);
	log_info(infoHilo.logger, mensaje);
}

void logearFinalizacion(t_hilo infoHilo, char *idProceso){
	char *mensaje = string_new();
	char *id = string_itoa(infoHilo.idHilo);
	string_append(&mensaje, "CPU");
	string_append(&mensaje, id);
	string_append(&mensaje, ", proceso ");
	string_append(&mensaje, idProceso);
	string_append(&mensaje, " finalizado.");
	log_info(infoHilo.logger, mensaje);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The real deal. Esta función va a ser la que reciba la ruta y el contador de programa, y
 * la que ejecute línea por línea las instrucciones desde donde lo indique el contador.*/

t_resultadoOperacion correrArchivo(char *ruta, int contadorPrograma, char* idProceso, int serverMemoria, int serverPlanificador,
		t_hilo infoDelHilo) {
	t_resultadoOperacion resultado;
	int n = contadorPrograma;
	int retardo = getRetardo();
	char *archivoEnStrings = txtAString(ruta);
	char **listaInstrucciones = string_split(archivoEnStrings, ";\n");
	retardo = getRetardo();
	resultado.maximo = cantidadElementos(listaInstrucciones);
	resultado.m = 0;
	resultado.data = malloc(sizeof(int) * resultado.maximo);
	int consumoDeCpu = 0;
	int instruccionesEjecutadas = 0;

	/* Estructura con el contador de tiempo */
		struct itimerval contador;
		struct timeval duracionDelContador, primerSignal;

		duracionDelContador.tv_sec = 60;
		duracionDelContador.tv_usec = 0;

		primerSignal.tv_sec = 0;
		primerSignal.tv_usec = 0;

		contador.it_interval = duracionDelContador;
		contador.it_value = primerSignal;

	signal (SIGALRM, reiniciarContador);

	setitimer (ITIMER_REAL, &contador, NULL);

	int operacion = reconocerInstruccion(listaInstrucciones[n]);
	while ((listaInstrucciones[n] != NULL) && (operacion != 7)) {

		operacion = reconocerInstruccion(listaInstrucciones[n]);
		resultado.data[resultado.m] = ejecutar(listaInstrucciones[n], serverMemoria, serverPlanificador,
				idProceso, infoDelHilo);
		if(resultado.data[resultado.m] == 0){
			resultado.m = resultado.m - 1;
			resultado.contador = n;
			return resultado;
				}
		n++;
		resultado.m++;
		usleep(retardo * 1000000);
		instruccionesEjecutadas++;
		consumoDeCpu = consumoDeCpu + ((instruccionesEjecutadas * 100)  / (60 / retardo));

	}
	resultado.m = resultado.m - 1;
	resultado.contador = n;
	resultado.usoDeCpu = consumoDeCpu;
	return resultado;

	void reiniciarContador(int contador){
			consumoDeCpu = 0;
		}

}
/* Algo como esto va a ser la modificación a realizar una vez que le agreguemos el campo quantum a la pcb. */

/* if(algoritmo es FIFO){
 	 while ((listaInstrucciones[n] != NULL) && (operacion != 7)) {
		operacion = reconocerInstruccion(listaInstrucciones[n]);
		resultado.data[resultado.m] = ejecutar(listaInstrucciones[n], serverMemoria, serverPlanificador,
				idProceso, infoDelHilo);
		if(resultado.data[resultado.m] == 0){
			resultado.m = resultado.m - 1;
			resultado.contador = n;
			return resultado;
				}
		n++;
		resultado.m++;
		sleep(retardo);
		}
	}
	else if(algoritmo es RR){
	int duracionDeRafaga = 0;
	while ((listaInstrucciones[n] != NULL) && (operacion != 7) && (duracionDeRafaga < quantum)) {
		resultado.data[resultado.m] = ejecutar(listaInstrucciones[n], serverMemoria, serverPlanificador,
				idProceso);
		n++;
		resultado.m++;
		duracionDeRafaga++;
		sleep(retardo);
			}
	};
	resultado.m = resultado.m - 1;
	resultado.contador = n;
	return resultado;

 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Función para serializar y enviar un resultado de ejecución en un solo paso. */
void enviarResultado(t_resultadoEjecucion resultado, int serverMemoria) {
	char *resultadoSerializado = serializarResultado(resultado);
	send(serverMemoria, resultadoSerializado, sizeof(resultadoSerializado), 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Función para obtener la cantidad de elementos de un array cuyo último elemento es NULL (para la lista de instrucciones).*/
int cantidadElementos(char **lista){
	int cantidad = 0;
	int n = 0;
	while(lista[n] != NULL){
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

/* funcion para pasarle al pthread_create. */
void *iniciarcpu(t_hilo threadInfo) {
	 char *ip_Planificador, ip_Memoria, puerto_Planificador, puerto_Memoria;
	 int serverPlanificador, serverMemoria;
	 ip_Planificador = getIpPlanificador();
	 ip_Memoria = getIpMemoria();
	 puerto_Planificador = getPuertoPlanificador();
	 puerto_Memoria = getPuertoMemoria();

	 serverPlanificador = conectarCliente(ip_Planificador, puerto_Planificador);
	 serverMemoria = conectarCliente(ip_Memoria, puerto_Memoria);

//	char package[PACKAGESIZE];
	int status = 1;
	char *iniciar =  eventoDeLogeo("iniciada CPU ",threadInfo.idHilo);
	printf("\n");
	log_info(threadInfo.logger, iniciar);
	printf("\n");
	while (status != 0) {
			char * buffer = malloc(sizeof(int) * 3);
		 status = recv(serverPlanificador, buffer, (sizeof(int) * 3), 0);
		 Paquete *contexto_ejecucion;
		 contexto_ejecucion = deserializar_header(buffer);
		 free(buffer);
		 if (status != 0) {
		 char * dataBuffer = malloc(contexto_ejecucion->tamanio);
		 recv(serverPlanificador, dataBuffer, contexto_ejecucion->tamanio, 0);
		 contexto_ejecucion->path = malloc(contexto_ejecucion->tamanio);
		 memcpy(contexto_ejecucion->path, dataBuffer,contexto_ejecucion->tamanio);
		 free(dataBuffer);
		 destruirPaquete(contexto_ejecucion);

		 if(contexto_ejecucion->codigoOperacion == 0){		// caso Correr Archivo
			 char *rutaDelArchivo = contexto_ejecucion->path;
			 int contadorDePrograma = contexto_ejecucion->programCounter;
			 char *idProceso = string_itoa(contexto_ejecucion->pid);
			 t_resultadoOperacion resultado = correrArchivo(rutaDelArchivo, contadorDePrograma, idProceso, serverMemoria,
			 serverPlanificador,	threadInfo);
			 char *contextoRecibido = logeoDeEjecucion("Contexto de ejecución recibido. Ruta del archivo:| ; Posicion del contador de Programa:| ;Id de proceso:|",
						rutaDelArchivo, contadorDePrograma, idProceso);
			log_info(threadInfo.logger, contextoRecibido);

			int i = 0;
			printf("\noperación exitosa. El resultado de la ejecución fue el siguiente:\nposición actual del contador: %d\n",
							resultado.contador);
			printf("resultado de las ejecuciones:\n");
			for(i = 0; i < resultado.maximo; i++){
				printf("%d\n", resultado.data[i]);
					};
		 }

		 else if(contexto_ejecucion->codigoOperacion == 1){	// caso Finalizar PID
			 int cantidadInstrucciones = cantidadElementos(contexto_ejecucion->path);
			 contexto_ejecucion->programCounter = cantidadInstrucciones - 1;
		 }

		 else if(contexto_ejecucion->codigoOperacion == 2){ // caso Consumo de CPU

		 };


	/*	char *rutaDelArchivo = "/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/Debug/largoVersion3";
		int contadorDePrograma = 0;
		char *idProceso = "1";


		printf("\n"); */



//		t_resultadoOperacion resultado = correrArchivo(rutaDelArchivo, contadorDePrograma, idProceso, 123, 456,
//				threadInfo);

		printf("\n");
		char *disponible = eventoDeLogeo("liberada la CPU ", threadInfo.idHilo);
		log_info(threadInfo.logger, disponible);
		printf("PC %d liberada y disponible\n", threadInfo.idHilo);
		status = 0;
	}




	}

//	close(serverPlanificador);
//	close(serverMemoria);

	printf("\nejecución exitosa. Finalizando...\n");
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *eventoDeLogeo(char * mensajeALogear, int id){
	size_t largomensaje, largoid;
	char *mensaje = mensajeALogear;
	char *idHilo = string_itoa(id);
	largomensaje = strlen(mensaje);
	largoid = strlen(idHilo);
	void *buffer = malloc((sizeof(char) * largomensaje) + sizeof(char) * largoid);
	memcpy(buffer, mensaje, largomensaje);
	memcpy(buffer + largomensaje, idHilo, largoid);
	int longReal = largomensaje + largoid;
	char *retorno = string_substring_until(buffer, longReal);
	free(buffer);
	return retorno;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char *logeoDeEjecucion(char *mensaje, char *ruta, int contador, char *idProceso){
	size_t largoMensaje1, largoMensaje2, largoMensaje3, largoRuta, largoContador, largoId;
	char *contadorPrograma = string_itoa(contador);
	char **mensajeDesarmado = string_split(mensaje, "|");
	largoMensaje1 = strlen(mensajeDesarmado[0]);
	largoMensaje2 = strlen(mensajeDesarmado[1]);
	largoMensaje3 = strlen(mensajeDesarmado[2]);
	largoRuta = strlen(ruta);
	largoId = strlen(idProceso);
	largoContador = strlen(contadorPrograma);
	void *buffer = malloc((sizeof(char) * largoMensaje1) + (sizeof(char) * largoMensaje2) + (sizeof(char) * largoMensaje3) +
			(sizeof(char) * largoRuta) + (sizeof(char) * largoContador) + sizeof(char) * largoId);
	memcpy(buffer, mensajeDesarmado[0], largoMensaje1);
	memcpy(buffer + largoMensaje1, ruta, largoRuta);
	memcpy(buffer + largoMensaje1 + largoRuta, mensajeDesarmado[1], largoMensaje2);
	memcpy(buffer + largoMensaje1 + largoRuta + largoMensaje2, contadorPrograma, largoContador);
	memcpy(buffer + largoMensaje1 + largoRuta + largoMensaje2 + largoContador, mensajeDesarmado[2], largoMensaje3);
	memcpy(buffer + largoMensaje1 + largoRuta + largoMensaje2 + largoContador + largoMensaje3, idProceso, largoId);
	return(buffer);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*función de testeo. */

void testCpuFunction(char *accion) {
//	printf("ingrese la accion a testear, o escriba 'ayuda' para ver comandos disponibles:\n");
//	scanf("%[^\n]%*c", accion);
	if (strcmp(accion, "reconocer instruccion") == 0) {
			int respuesta;
			printf("ingrese la instruccion a reconocer\n");
			char *dataAdicional = malloc(sizeof(char) * 32);
			scanf("%[^\n]%*c", dataAdicional);
			respuesta = reconocerInstruccion(dataAdicional);
			printf("el header correspondiente a la instruccion es: %d\n",respuesta);

	} else if (strcmp(accion, "empaquetado normal") == 0) {
			printf("ingrese la instruccion a empaquetar(formato aceptado: instruccion paginas):\n");
			char *dataAdicional = malloc(sizeof(char) * 64);
			scanf("%[^\n]%*c", dataAdicional);
			t_instruccion paquete;
			char **algo;
			algo = string_split(dataAdicional, " ");
			paquete = empaquetar(algo[0], algo[1]);
			printf("la instruccion es: %s\n", paquete.instruccion);
			printf("el número de página/proceso es: %s\n", paquete.cantidadDePaginas);

	} else if (strcmp(accion, "empaquetado de escritura") == 0) {
			printf("ingrese la instruccion a empaquetar(formato aceptado: id pagina contenido):\n");
			char *dataAdicional = malloc(sizeof(char) * 64);
			fgets(dataAdicional, 64, stdin);
			t_instruccionEscritura paquete;
			char **algo;
			algo = string_n_split(dataAdicional, 3, " ");
			paquete = empaquetarEscritura(algo[0], algo[1], algo[2]);
			printf("el proceso es: %d\n", paquete.idProceso);
			printf("el número de página es: %d\n", paquete.paginas);
			printf("el contenido a escribir es: %s\n", paquete.textoAEscribir);

	} else if (strcmp(accion, "empaquetar resultado") == 0) {
			printf("ingrese los valores de resultado a empaquetar(formato válido: rutaDelArchivo contadorPrograma):\n");
			char *dataAdicional = malloc(sizeof(char) * 64);
			scanf("%[^\n]%*c", dataAdicional);
			t_resultadoEjecucion paquete;
			char **array;
			array = string_split(dataAdicional, " ");
			int pc = atoi(array[1]);
			paquete = empaquetarResultado(array[0], pc);
			printf("el archivo es: %s\n", paquete.nombrePrograma);
			printf("el valor del contador es: %d\n", paquete.programCounter);

	} else if (strcmp(accion, "archivo a texto") == 0) {
			printf("ingrese una ruta de archivo válida:\n");
			char *dataAdicional = malloc(sizeof(char) * 64);
			scanf("%[^\n]%*c", dataAdicional);
			char *texto = txtAString(dataAdicional);
			printf("el contenido del archivo es: %s\n", texto);

	} else if (strcmp(accion, "serializacion normal") == 0) {
			printf("ingrese una linea de código que desee serializar (formato válido: instruccion página):\n");
			char *paqueteSerializado;
			char *dataAdicional = malloc(sizeof(char) * 64);
			t_instruccion paquete;
			scanf("%[^\n]%*c", dataAdicional);
			char **algo;
			algo = string_split(dataAdicional, " ");
			paquete = empaquetar(algo[0], algo[1]);
			paqueteSerializado = serializarEmpaquetado(paquete);
			printf("serializacion exitosa. contenido a enviar: %s\n",paqueteSerializado);

	} else if (strcmp(accion, "serializacion con escritura") == 0) {
			printf("ingrese una linea de código que desee serializar (formato válido: ""instruccion página contenidoAEscribir):\n");
			char *paqueteSerializado;
			char *dataAdicional = malloc(sizeof(char) * 64);
			t_instruccionEscritura paquete;
			scanf("%[^\n]%*c", dataAdicional);
			char **algo;
			algo = string_n_split(dataAdicional, 3, " ");
			paquete = empaquetarEscritura(algo[0], algo[1], algo[2]);
			paqueteSerializado = serializarEmpaquetadoEscritura(paquete);
			printf("serializacion exitosa. contenido a enviar: %s\n",paqueteSerializado);

	} else if (strcmp(accion, "serializar resultado") == 0) {
			printf("ingrese los datos que desee serializar (formato válido: rutaDelArchivo contadorPrograma):\n");
			t_resultadoEjecucion paquete;
			char *paqueteSerializado;
			char *dataAdicional = malloc(sizeof(char) * 64);
			char **array;
			scanf("%[^\n]%*c", dataAdicional);
			array = string_split(dataAdicional, " ");
			int pc = atoi(array[1]);
			paquete = empaquetarResultado(array[0], pc);
			paqueteSerializado = serializarResultado(paquete);
			printf("serialización exitosa. Contenido a enviar: %s\n",paqueteSerializado);

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
			int resultado = ejecutar(dataAdicional, 123, 456, id, infoHilo);
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
			correrArchivo(ruta, contadorPrograma, "1", 123, 456, infoHilo);

	} else if (strcmp(accion, "iniciar cpu") == 0) {
			t_log *log_cpu = log_create("log_cpu", "CPU", true, LOG_LEVEL_INFO);
			t_hilo hiloInfo;
			hiloInfo.idHilo = 1;
			hiloInfo.logger = log_cpu;
			iniciarcpu(hiloInfo);

	} else if (strcmp(accion, "ayuda") == 0) {
			printf("lista de comandos disponibles:\nreconocer instruccion\nempaquetado normal\nempaquetado de escritura\n"
							"empaquetar resultado\narchivo a texto\nserializacion normal\nserializacion con escritura\n"
							"serializar resultado\nejecutar instruccion\ncorrer archivo\niniciar cpu\ngenerar hilo\n"
							"generar dos hilos\ngenerar n hilos\n\n");
			printf(
					"para las opciones 'reconocer instruccion' y 'ejecutar instrucción' utilice las siguientes instrucciones:\n"
							"iniciar N\nleer N\nescribir N contenidoAEscribir\nentrada-salida Tiempo\nfinalizar\n");

	} else if(strcmp(accion, "generar header") == 0){
			printf("ingrese una clave y un tamanio de contenido a convertir en header:\n");
			char *dataAdicional = malloc(sizeof(char) * 64);
			scanf("%[^\n]%*c", dataAdicional);
			int clave, tamanio;
			char **array = string_split(dataAdicional, " ");
			clave = atoi(array[0]);
			tamanio = atoi(array[1]);
			char *header = generarHeader(clave, tamanio);
			printf("creación exitosa. El header resultante es: %s\n", header);

	}else if(strcmp(accion, "generar hilo") == 0){
		t_log *log_cpu = log_create("log_cpu", "CPU", true, LOG_LEVEL_INFO);
		t_hilo hiloInfo;
		hiloInfo.idHilo = 1;
		hiloInfo.logger = log_cpu;
		t_hilo *puntero = malloc(sizeof(t_hilo));
		memcpy(puntero, &hiloInfo, sizeof(t_hilo));

		pthread_t hilo1;
		pthread_create(&hilo1, NULL, iniciarcpu, (void *) &hiloInfo);
		pthread_join(hilo1, NULL);

	}else if(strcmp(accion, "generar dos hilos") == 0){
		t_log *log_cpu = log_create("log_cpu", "CPU", true, LOG_LEVEL_INFO);
		t_hilo hiloInfo1, hiloInfo2;
		hiloInfo1.idHilo = 1;
		hiloInfo1.logger = log_cpu;
		hiloInfo2.idHilo = 2;
		hiloInfo2.logger = log_cpu;
		pthread_t hilo1;
		pthread_t hilo2;
		pthread_create(&hilo1, NULL, iniciarcpu, (void *) &hiloInfo1);
		pthread_create(&hilo2, NULL, iniciarcpu, (void *) &hiloInfo2);
		pthread_join(hilo1, NULL);
		pthread_join(hilo2, NULL);

	}else if (strcmp(accion, "generar n hilos") == 0){
		t_log *log_cpu = log_create("log_cpu", "CPU", true, LOG_LEVEL_INFO);
		t_hilo hiloInfo;
		hiloInfo.logger = log_cpu;
		struct t_hilo *puntero = malloc(sizeof(t_hilo));
		memcpy(puntero, &hiloInfo, sizeof(t_hilo));
		int i;
		int cantHilos = getHilos();
		pthread_t hilos[cantHilos];
		for (i = 0; i < cantHilos; i++) {
			hiloInfo.idHilo = i;
			pthread_create(&hilos[i], NULL, iniciarcpu, puntero);
				};
			for (i = 0; i < cantHilos; i++) {
				pthread_join(hilos[i], NULL);
		};

	}else if(strcmp(accion, "check") == 0){
		int check = 2;
		printf("%d\n", check);

	}else printf("comando desconocido. por favor, intente de nuevo.\n");

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
