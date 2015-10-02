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
	int tamanio;
	char *mensaje;
} Paquete;


enum estado {
	listo = 98, ejecucion, bloqueado, finalizado
};
typedef struct {
	int id;
	char *dirProceso;
	int estado;
} tipo_pcb;


struct addrinfo* cargarInfoSockets(char *IP, char* Port);
int conectarCliente(char *IP, char* Port);
int conectarServidor(char* IP,char* Port,int backlog);
Paquete generarPaquete(int codigoOperacion, int tamMessage, char *message);
char *serializar(Paquete *unPaquete);
int reconocerIdentificador() ;
int generarPID(int* pid);
tipo_pcb generarPCB(int pid, char *path, int estado);
#endif /* LIBSOCKETS_H_ */
