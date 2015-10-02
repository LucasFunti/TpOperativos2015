/*
 * libSockets.h
 *
 *  Created on: 28/9/2015
 *      Author: utnso
 */

#ifndef LIBSOCKETS_H_
#define LIBSOCKETS_H_

struct addrinfo* cargarInfoSockets(char *IP, char* Port);
int conectarCliente(char *IP, char* Port);
int conectarServidor(char* IP,char* Port,int backlog);

#endif /* LIBSOCKETS_H_ */
