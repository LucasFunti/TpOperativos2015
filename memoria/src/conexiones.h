/*
 * conexiones.h
 *
 *  Created on: 1/11/2015
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include "globals.h"
#include "loggear.h"
#include "memoria.h"

int socketEscucha, socketSwap;
fd_set sockets_para_revisar, sockets_activos;

t_data * pedirPaquete(int codigoOp, int tamanio, void * data);
char* serializar(t_data * unPaquete);
void common_send(int socket, t_data * paquete);
void conectarseAlSwap();
void atenderConexiones();
int connect_to(char *IP, char* Port);
struct addrinfo* common_setup(char *IP, char* Port);
int setup_listen(char* IP, char* Port);
t_data * leer_paquete(int socket);

#endif /* CONEXIONES_H_ */
