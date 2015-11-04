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

int socketEscucha;


void atenderConexiones();
struct addrinfo* common_setup(char *IP, char* Port);
int setup_listen(char* IP, char* Port);
t_data * leer_paquete(int socket);

#endif /* CONEXIONES_H_ */
