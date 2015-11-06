/*
 * libSockets.h
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#ifndef LIBSOCKETS_H_
#define LIBSOCKETS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <commons/log.h>
#include <commons/string.h>

typedef struct Paquete {
	int codigoOperacion;
	int programCounter;
	int pid;
	int quantum;
	int tamanio;
	char *path;
} Paquete;

/* setup_listen(IP,PORT) *
 * Devuelve el socket que se consiguió para escuchar
 *
 * IP = Ip de escucha
 * PORT = Puerto de escucha
 */
int setup_listen(char*, char*);

/* setup_listen(IP,PORT,LOGGER) *
 * Variante del setup_listen con la posibilidad de loggeo
 *
 * IP = Ip de escucha
 * PORT = Puerto de escucha
 * LOGGER = Log para escribir
 */
int setup_listen_con_log(char*, char*, t_log *);

/* cargarInfoSocket(IP,PORT) *
 * Devuelve un addr configurado
 *
 * IP = Ip de escucha
 * PORT = Puerto de escucha
 */
struct addrinfo* cargarInfoSocket(char *, char*);

/* conectarCliente(IP,PORT) *
 * Devuelve el socket que se conectó con el servidor
 *
 * IP = Ip de escucha
 * PORT = Puerto de escucha
 */
int conectarCliente(char *IP, char* Port);

/* conectarCliente(IP,PORT,LOGGER) *
 * Devuelve el socket que se conectó con el servidor
 *
 * IP = Ip de escucha
 * PORT = Puerto de escucha
 * LOGGER = Log para escribir
 */
int conectarCliente_con_log(char *IP, char* Port, t_log *);

/* esperarConexionEntrante(SocketEscucha,Backlog,LOGGER)
 * Devuelve el socket para mandar datos
 *
 * LOGGER = Log para escribir
 */
int esperarConexionEntrante(int, int, t_log *);

int conectarServidor(char* IP, char* Port, int backlog);
Paquete *generarPaquete(int codigoOperacion, int tamMessage, char *message,
		int programCounter);
char *serializar(Paquete *unPaquete);
Paquete *deserializar_header(char *buffer);
void deserializar_data(Paquete *unPaquete, char *buffer);
void destruirPaquete(Paquete * unPaquete);

#endif /* LIBSOCKETS_H_ */
