/*
 * libSockets.h
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#ifndef LIBSOCKETS_H_
#define LIBSOCKETS_H_

typedef struct Paquete {
	int codigoOperacion;
	int programCounter;
	int tamanio;
	char *mensaje;
} Paquete;

struct addrinfo* cargarInfoSockets(char *IP, char* Port);
int conectarCliente(char *IP, char* Port);
int conectarServidor(char* IP,char* Port,int backlog);
Paquete generarPaquete(int codigoOperacion, int tamMessage, char *message, int programCounter);
char *serializar(Paquete *unPaquete);
Paquete *deserializar(char *buffer);

#endif /* LIBSOCKETS_H_ */
