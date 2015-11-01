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
t_instruccionEscritura empaquetarEscritura(char *instruccionRecibida,
		char *paginas, char *texto) {
	t_instruccionEscritura escrituraEmpaquetada;
	escrituraEmpaquetada.instruccion = instruccionRecibida;
	escrituraEmpaquetada.cantidadDePaginas = paginas;
	escrituraEmpaquetada.textoAEscribir = texto;

	return escrituraEmpaquetada;
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
	size_t numeroDePaginas, tamanioTexto;
	char *paginas = instruccionEmpaquetada.cantidadDePaginas;
	char *texto = instruccionEmpaquetada.textoAEscribir;
	string_append(&paginas, "\0");
	string_append(&texto, "\0");
	numeroDePaginas = strlen(paginas);
	tamanioTexto = strlen(texto);
	void *buffer = malloc((sizeof(char) * numeroDePaginas) + (sizeof(char) * tamanioTexto));
	memcpy(buffer, paginas, numeroDePaginas);
	memcpy(buffer + numeroDePaginas, texto, tamanioTexto);
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
	string_append(&claveString, "\0");
	string_append(&tamanioString, "0");
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
int ejecutar(char *linea, int serverMemoria, int serverPlanificador, char *idProceso) {
	int tamanio, tamanioContenido, resultado, estadoDeEjecucion, paginas, header;
	char *contenidoDePagina = malloc(256); /* ver tamañano real mas adelante. */
	char **array;
	t_instruccion paquete;
	t_instruccionEscritura paqueteEscritura;
	t_mensaje mensaje;
	char *paqueteSerializado;
	int clave = reconocerInstruccion(linea);

	switch (clave) {

	case 4:									// instrucción iniciar N
		array = string_split(linea, " ");
		int paginas = atoi(array[1]);
		tamanio = sizeof(paginas);
		header = concatenar(clave, tamanio);
		send(serverMemoria, &header, sizeof(int) + tamanio, 0);
		send(serverMemoria, paginas, tamanio, 0);

//		recv(serverMemoria, &estadoDeEjecucion, sizeof(int), 0);

		estadoDeEjecucion = 1; // SOLO PARA TESTEAR
		if (estadoDeEjecucion == 0) {
			resultado = 0;
			printf("mProc %s - fallo al iniciar.\n", idProceso);
		} else if (estadoDeEjecucion == 1) {
			resultado = 1;
			printf("mProc %s - iniciado\n", idProceso);
		};
		break;

	case 5:									// instrucción leer N
		array = string_split(linea, " ");
		paginas = atoi(array[1]);
		tamanio = sizeof(paginas);
		header = concatenar(header, tamanio);
		send(serverMemoria, &header, sizeof(int) + tamanio, 0);
		send(serverMemoria, paginas, tamanio, 0);
//		recv(serverMemoria, &estadoDeEjecucion, sizeof(int), 0);

		estadoDeEjecucion = 1; // SOLO PARA TESTEAR

		if(estadoDeEjecucion == 0){
			resultado = 0;
			printf("Error al leer la página solicitada.\n");
		}
		else if(estadoDeEjecucion == 1){
//		recv(serverMemoria, contenidoDePagina, 256, 0); /*que el administrador sólo envíe el contenido!!! */

		contenidoDePagina = "texto de prueba"; /* únicamente para testear */
		resultado = 1;
		printf("proceso: %s, pagina: %s leida: %s\n", idProceso, array[1],
				contenidoDePagina);
		}
		break;

	case 6:										//instrucción escribir N contenido
		array = string_n_split(linea, 3, " ");
		paqueteEscritura = empaquetarEscritura(array[0], array[1], array[2]);
		paqueteSerializado = serializarEmpaquetadoEscritura(paqueteEscritura);
		tamanio = strlen(paqueteSerializado);
		send(serverMemoria, &clave, sizeof(int), 0);
		send(serverMemoria, paqueteSerializado, tamanio, 0);
//		recv(serverMemoria, &estadoDeEjecucion, sizeof(int), 0);

		estadoDeEjecucion = 1; // SOLO PARA TESTEAR

		if(estadoDeEjecucion == 0){
			resultado = 0;
			printf("Fallo de escritura o página inválida. Abortando...\n");
		}
		else if(estadoDeEjecucion == 1){
//		recv(serverMemoria, contenidoDePagina, sizeof(int), 0);
		resultado = 1;
		contenidoDePagina = array[2]; /* unicamente para testear */
		printf("proceso %s, Página %s escrita: %s\n", idProceso, array[1],
				contenidoDePagina);
		}
		break;

	case 7:									// instrucción entrada-salida T
		array = string_split(linea, " ");
		paquete = empaquetar(array[0], array[1]);
		paqueteSerializado = serializarEmpaquetado(paquete);
		tamanio = strlen(paqueteSerializado);
		send(serverPlanificador, &clave, sizeof(int), 0);
		send(serverPlanificador, paqueteSerializado, tamanio, 0);
		resultado = 1;
		printf("proceso %s en entrada-salida de tiempo %s\n", idProceso,array[1]);
		break;

	case 8:								// instrucción finalizar
		tamanio = strlen(idProceso);
		send(serverMemoria, &clave, sizeof(int), 0);
		send(serverMemoria, &idProceso, tamanio, 0);
		resultado = 1;
		printf("proceso %s finalizado.\n", idProceso);
		break;

	};
	return resultado;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* The real deal. Esta función va a ser la que reciba la ruta y el contador de programa, y
 * la que ejecute línea por línea las instrucciones desde donde lo indique el contador.*/

t_resultadoOperacion correrArchivo(char *ruta, int contadorPrograma, char* idProceso, int serverMemoria, int serverPlanificador) {
	t_resultadoOperacion resultado;
	int n = contadorPrograma;
	int retardo = getRetardo();
	char *archivoEnStrings = txtAString(ruta);
	char **listaInstrucciones = string_split(archivoEnStrings, ";\n");
	retardo = getRetardo();
	resultado.maximo = cantidadElementos(listaInstrucciones);
	resultado.m = 0;
	resultado.data = malloc(sizeof(int) * resultado.maximo);
	while (listaInstrucciones[n] != NULL) {
		resultado.data[resultado.m] = ejecutar(listaInstrucciones[n], serverMemoria, serverPlanificador,
				idProceso);
		n++;
		resultado.m++;
		sleep(retardo);
	}
	resultado.m = resultado.m - 1;
	resultado.contador = n;
	return resultado;


}
/* Algo como esto va a ser la modificación a realizar una vez que le agreguemos el campo quantum a la pcb. */

/* if(algoritmo es FIFO){
 	 while (listaInstrucciones[n] != NULL) {
		resultado.data[resultado.m] = ejecutar(listaInstrucciones[n], serverMemoria, serverPlanificador,
				idProceso);
		n++;
		resultado.m++;
		sleep(retardo);
			}
	}
	else if(algoritmo es RR){
	int duracionDeRafaga = 0;
	while ((listaInstrucciones[n] != NULL) && (duracionDeRafaga < quantum)) {
		resultado.data[resultado.m] = ejecutar(listaInstrucciones[n], serverMemoria, serverPlanificador,
				idProceso);
		n++;
		resultado.m++;
		duracionDeRagaga++;
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

/* Función para concatenar enteros. */
int concatenar(int valorA, int valorB){
	char * stringA = string_itoa(valorA);
	char * stringB = string_itoa(valorB);
	char *concatenado = string_new();
	string_append(&concatenado, stringA);
	string_append(&concatenado, stringB);
	int retorno = atoi(concatenado);
	return retorno;
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
	/*	char *ip_Planificador, ip_Memoria, puerto_Planificador, puerto_Memoria;
	 int serverPlanificador, serverMemoria;
	 ip_Planificador = getIpPlanificador();
	 ip_Memoria = getIpMemoria();
	 puerto_Planificador = getPuertoPlanificador();
	 puerto_Memoria = getPuertoMemoria();

	 serverPlanificador = conectarCliente(ip_Planificador, puerto_Planificador);
	 serverMemoria = conectarCliente(ip_Memoria, puerto_Memoria); */

	/*char package[PACKAGESIZE];*/
	int status = 1;
	char *iniciar =  eventoDeLogeo("iniciada CPU ",threadInfo.idHilo);
	printf("\n");
	log_info(threadInfo.logger, iniciar);
	printf("\n");
	while (status != 0) {
		/*	char * buffer = malloc(sizeof(int) * 3);
		 status = recv(serverPlanificador, buffer, (sizeof(int) * 3), 0);
		 Paquete *contexto_ejecucion;
		 contexto_ejecucion = deserializar_header(buffer);
		 free(buffer);
		 //estos printf son probar que ande nada mas
		 //		printf("El codigo de la operacion es:%d \n",
		 //				contexto_ejecucion->codigoOperacion);
		 //		printf("El PC es:%d \n", contexto_ejecucion->programCounter);
		 //		printf("El tamanio a recivir es:%d\n", contexto_ejecucion->tamanio);
		 if (status != 0) {
		 char * dataBuffer = malloc(contexto_ejecucion->tamanio);
		 recv(serverPlanificador, dataBuffer, contexto_ejecucion->tamanio,
		 0);
		 contexto_ejecucion->mensaje = malloc(contexto_ejecucion->tamanio);
		 memcpy(contexto_ejecucion->mensaje, dataBuffer,
		 contexto_ejecucion->tamanio);
		 free(dataBuffer);
		 printf("el mensaje es:%s \n", contexto_ejecucion->mensaje);
		 destruirPaquete(contexto_ejecucion);  */

/*		printf("ingrese una ruta de archivo válida, seguida del contador del programa y una id de proceso, o ingrese 'fin' "
				"para finalizar\n");
		char *prueba = malloc(sizeof(char) * 128);
		scanf("%[^\n]%*c", prueba);
		if(strcmp(prueba, "fin") == 0){
			status = 0;
		}
		else{
		char **array = string_split(prueba, " ");*/ /* acá va contexto_ejecucion->mensaje */
/*		char *rutaDelArchivo = array[0];
		int contadorDePrograma = atoi(array[1]);
		char *idProceso = array[2];



		correrArchivo(rutaDelArchivo, contadorDePrograma, idProceso, 123, 456); /* para testear la función por separado. */

		/* definir si lo que vamos a recibir es solo el contexto,  solo la pcb, o ambas cosas! */

		char *rutaDelArchivo = "/home/utnso/git/tp-2015-2c-signiorcodigo/cpu/Debug/largoVersion2";
		int contadorDePrograma = 0;
		char *idProceso = "1";

		char *contextoRecibido = logeoDeEjecucion("Contexto de ejecución recibido. Ruta del archivo:| ; Posicion del contador de Programa:| ;Id de proceso:|",
				rutaDelArchivo, contadorDePrograma, idProceso);
		log_info(threadInfo.logger, contextoRecibido);
		printf("\n");

		t_resultadoOperacion resultado = correrArchivo(rutaDelArchivo, contadorDePrograma, idProceso, 123, 456);
		int i = 0;

		printf("\noperación exitosa. El resultado de la ejecución fue el siguiente:\nposición actual del contador: %d\n",
				resultado.contador);
		printf("resultado de las ejecuciones:\n");
		for(i = 0; i > resultado.maximo; i++){
			printf("%d\n", resultado.data[i]);
		};
		printf("\n");
		char *disponible = eventoDeLogeo("liberada la CPU ", threadInfo.idHilo);
		log_info(threadInfo.logger, disponible);
		status = 0;
	}




//	}
	/*send(serverMemoria, package, sizeof(package), 0);*/

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
	int longReal = strlen(buffer);
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
			printf("ingrese la instruccion a empaquetar(formato aceptado: instruccion pagina contenido):\n");
			char *dataAdicional = malloc(sizeof(char) * 64);
			fgets(dataAdicional, 64, stdin);
			t_instruccionEscritura paquete;
			char **algo;
			algo = string_n_split(dataAdicional, 3, " ");
			paquete = empaquetarEscritura(algo[0], algo[1], algo[2]);
			printf("la instruccion es: %s\n", paquete.instruccion);
			printf("el número de página es: %s\n", paquete.cantidadDePaginas);
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
			scanf("%[^\n]%*c", dataAdicional);
			printf("ingrese una id de proceso:\n");
			scanf("%[^\n]%*c", idProceso);
			id = idProceso;
			int resultado = ejecutar(dataAdicional, 123, 456, id);
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
			correrArchivo(ruta, contadorPrograma, "1", 123, 456);

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

	} else if(strcmp(accion, "concatenar") == 0){
			printf("ingrese dos valores, separados por un espacio:\n");
			char *dataAdicional = malloc(sizeof(char) * 64);
			scanf("%[^\n]%*c", dataAdicional);
			char **array = string_split(dataAdicional, " ");
			int valorA = atoi(array[0]);
			int valorB = atoi(array[1]);
			int concatenacion = concatenar(valorA, valorB);
			printf("el resultado de concatenar es: %d\n", concatenacion);

	}else if(strcmp(accion, "generar hilo") == 0){
		pthread_t hilo1;
		pthread_create(&hilo1, NULL, iniciarcpu, NULL);
		pthread_join(hilo1, NULL);

	}else if(strcmp(accion, "generar dos hilos") == 0){
		pthread_t hilo1;
		pthread_t hilo2;
		pthread_create(&hilo1, NULL, iniciarcpu, NULL);
		pthread_create(&hilo2, NULL, iniciarcpu, NULL);
		pthread_join(hilo1, NULL);
		pthread_join(hilo2, NULL);

	}else if (strcmp(accion, "generar n hilos") == 0){
		int cantHilos, i;
		cantHilos = getHilos();
		pthread_t hilos[cantHilos];
		pthread_mutex_t mutex;
		pthread_mutex_init(&mutex, NULL);
		for (i = 0; i > cantHilos; i++) {
			pthread_create(&hilos[i], NULL, iniciarcpu, NULL);
			pthread_mutex_lock(&mutex);
			pthread_join(hilos[i], NULL);
			pthread_mutex_unlock(&mutex);
		};

	}else if(strcmp(accion, "check") == 0){
		int check = 2;
		printf("%d\n", check);

	}else if(strcmp(accion, "mensaje inicial") == 0){
		printf("ingrese un valor:\n");
		char *dataAdicional = malloc(sizeof(char) * 5);
		scanf("%[^\n]%*c", dataAdicional);
		int valor = atoi(dataAdicional);
		char *mensaje = mensajeDeInicio(valor);
		printf("el mensaje es: %s\n", mensaje);

	}
	else printf("comando desconocido. por favor, intente de nuevo.\n");

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
