/*
 * conexiones.c
 *
 *  Created on: 1/11/2015
 *      Author: utnso
 */
#include "conexiones.h"

void atenderConexiones() {

	socketEscucha = setup_listen("localhost",
			dictionary_get(diccionario_configuraciones, "PUERTO_ESCUCHA"));

	listen(socketEscucha, 15);

	loggearInfo(
			string_from_format("Se escuchan conexiones en el socket %d",
					socketEscucha));

	fd_set sockets_para_revisar, sockets_activos;

	int socketMasGrande = socketEscucha;

	FD_ZERO(&sockets_activos);
	FD_SET(socketEscucha, &sockets_activos);

	while (1) {

		sockets_para_revisar = sockets_activos;

		select(socketMasGrande + 1, &sockets_para_revisar, NULL, NULL, NULL);

		int i;
		for (i = 0; i <= socketMasGrande; i++) {
			if (FD_ISSET(i, &sockets_para_revisar)) { //Hay actividad

				// La actividad es en el puerto de escucha (osea es conexión nueva)
				if (i == socketEscucha) {

					loggearInfo(
							string_from_format(
									"Se detecto conexión entrante al puerto de escucha"));

					struct sockaddr_storage remoteaddr;
					socklen_t addrlen;
					addrlen = sizeof remoteaddr;

					int socket_nueva_conexion = accept(socketEscucha,
							(struct sockaddr *) &remoteaddr, &addrlen);

					if (socket_nueva_conexion == -1) {
						loggearError(
								"Error al asignar socket a la nueva conexion");
					} else {
						loggearInfo(

								string_from_format("Se asigna el socket %d",
										socket_nueva_conexion));

						FD_SET(socket_nueva_conexion, &sockets_activos); // Lo añadimos al set de los sockets activos

						if (socket_nueva_conexion > socketMasGrande) {
							socketMasGrande = socket_nueva_conexion;
						}

					}
					// La actividad es en un puerto de escucha (tenemos que atender su pedido)
				} else {

					void * auxiliar_peek = malloc(sizeof(int));

					//El flag MSG_PEEK sirve para que no saque la data del socket, simplemente la copia
					int cantidad_recibida = recv(i, auxiliar_peek, sizeof(int),
					MSG_PEEK);

					//Si es cero, significa que no había nada nuevo
					if (cantidad_recibida > 0) {
						loggearInfo(
								string_from_format(
										"Se detecto una conexión existente con un pedido"));
						atenderConexion(i, sockets_activos);
					}
					free(auxiliar_peek);
				}
			}
		}
	}
}

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

struct addrinfo* common_setup(char *IP, char* Port) {
	struct addrinfo hints;
	struct addrinfo* serverInfo;
	int16_t error;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if (!strcmp(IP, "localhost")) {
		hints.ai_flags = AI_PASSIVE;
		error = getaddrinfo(NULL, Port, &hints, &serverInfo);
	} else
		error = getaddrinfo(IP, Port, &hints, &serverInfo);
	if (error) {
		printf("Problema con el getaddrinfo()!: %s", gai_strerror(error));
		return NULL;
	}
	return serverInfo;
}

int setup_listen(char* IP, char* Port) {
	struct addrinfo* serverInfo = common_setup(IP, Port);
	if (serverInfo == NULL)
		return -1;
	int socketEscucha;
	socketEscucha = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	bind(socketEscucha, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);
	return socketEscucha;
}
