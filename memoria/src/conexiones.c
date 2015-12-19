/*
 * conexiones.c
 *
 *  Created on: 1/11/2015
 *      Author: utnso
 */
#include "conexiones.h"

void atenderConexiones() {

	socketEscucha = setup_listen("localhost",
			config_get_string_value(memoriaConfig, "PUERTO_ESCUCHA"));

	listen(socketEscucha, 1024);

	loggearInfo(
			string_from_format("Se escuchan conexiones en el socket %d",
					socketEscucha));

	int socketMasGrande = socketEscucha;

	FD_ZERO(&sockets_activos);

	FD_SET(socketEscucha, &sockets_activos);
	//FD_SET(socketSwap, &sockets_activos);

	while (1) {

		labelSelect: sockets_para_revisar = sockets_activos;

		int resultadoSelect = select(socketMasGrande + 1, &sockets_para_revisar,
		NULL, NULL, NULL);

		if (resultadoSelect == -1) {
			goto labelSelect;

		}

		pthread_mutex_lock(&semaforo_memoria);

		int i;
		for (i = 0; i <= socketMasGrande; i++) {

			if (FD_ISSET(i, &sockets_para_revisar)) {
				//Hay actividad

				if (i == socketEscucha) {
					// La actividad es en el puerto de escucha (osea es conexión nueva)

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

						// Lo añadimos al set de los sockets activos
						FD_SET(socket_nueva_conexion, &sockets_activos);

						if (socket_nueva_conexion > socketMasGrande) {
							socketMasGrande = socket_nueva_conexion;
						}

					}
				} else {

					// La actividad es en un puerto de cpu (tenemos que atender su pedido)

					atenderConexion(i, sockets_activos);

				}
			}
		}
		pthread_mutex_unlock(&semaforo_memoria);
	}
}

t_data * leer_paquete(int socket) {

	t_data * paquete_entrante = malloc(sizeof(t_data));
	paquete_entrante->header = malloc(sizeof(t_header));

	int resultado;
	label: resultado = recv(socket, &paquete_entrante->header->codigo_operacion,
			sizeof(int),
			MSG_WAITALL);
	if (resultado == -1) {
		goto label;
	}
	label2: resultado = recv(socket, &paquete_entrante->header->tamanio_data,
			sizeof(int),
			MSG_WAITALL);
	if (resultado == -1) {
		goto label2;
	}

	paquete_entrante->data = malloc(paquete_entrante->header->tamanio_data);

	label3: resultado = recv(socket, paquete_entrante->data,
			paquete_entrante->header->tamanio_data,
			MSG_WAITALL);
	if (resultado == -1) {
		goto label3;
	}

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
		exit(-1);
	int socketEscucha;
	socketEscucha = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	if (bind(socketEscucha, serverInfo->ai_addr, serverInfo->ai_addrlen) < 0) {
		printf("Puerto en TIME-WAIT");
		exit(-1);
	}
	freeaddrinfo(serverInfo);
	return socketEscucha;
}

int connect_to(char *IP, char* Port) {

	struct addrinfo* serverInfo = common_setup(IP, Port);
	if (serverInfo == NULL) {
		return -1;
	}
	int serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
	if (serverSocket == -1) {
		return -1;
	}
	if (connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen)
			== -1) {

		close(serverSocket);
		return -1;
	}

	freeaddrinfo(serverInfo);
	return serverSocket;
}

t_data * pedirPaquete(int codigoOp, int tamanio, void * data) {

	t_data * paquete = malloc(sizeof(t_data));
	paquete->header = malloc(sizeof(t_header));

	paquete->header->codigo_operacion = codigoOp;
	paquete->header->tamanio_data = tamanio;
	paquete->data = data;

	return paquete;
}

char* serializar(t_data * unPaquete) {
	void * stream = malloc(sizeof(t_header) + unPaquete->header->tamanio_data);

	memcpy(stream, unPaquete->header, sizeof(t_header));
	memcpy(stream + sizeof(t_header), unPaquete->data,
			unPaquete->header->tamanio_data);
	return stream;
}

void common_send(int socket, t_data * paquete) {
	char* buffer;
	int tamanio_total;
	buffer = serializar(paquete);
	tamanio_total = paquete->header->tamanio_data + sizeof(t_header);

	send(socket, buffer, tamanio_total, MSG_WAITALL);

	free(buffer);
}

void conectarseAlSwap() {

	if (!test) {

		socketSwap = connect_to(
				config_get_string_value(memoriaConfig, "IP_SWAP"),
				config_get_string_value(memoriaConfig, "PUERTO_SWAP"));

		int null_data = 0;

		t_data * paquete = pedirPaquete(1, sizeof(int), &null_data);

		common_send(socketSwap, paquete);

		paquete = leer_paquete(socketSwap);

		if (paquete->header->codigo_operacion == 2) {
			loggearInfo(
					string_from_format(
							"Conexión con el swap exitosa, se le registra el socket %d",
							socketSwap));
		} else {

			loggearError("No se pudo encontrar al swap, se cierra todo");
			exit(EXIT_FAILURE);
		}
	}

}
