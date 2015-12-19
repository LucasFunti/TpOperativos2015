/*
 * swap.c
 *
 <<<<<<< HEAD
 *  Created on: 5/9/2015
 =======
 *  Created on: 28/9/2015
 >>>>>>> e5bed079387090f438690c736e7ce6b16c1bb69c
 *      Author: utnso
 */

#include "swap.h"

#define INICIAR 4
#define LEER 5
#define ESCRIBIR 6
#define FINALIZAR 8

int main(int argc, char **argv) {

	remove("/tp-2015-2c-signiorcodigo/swap/log_swap");

	swapConfig = config_create("/tp-2015-2c-signiorcodigo/swap/swapConfig");

	setupSwap();

	setPages();

	int socketEscucha, socketMemoria;

	char * puerto = getPort();

	socketEscucha = setup_listen("localhost", puerto);

	socketMemoria = esperarConexionEntrante(socketEscucha, 1024, log_swap);

	t_data * paqueteInicio = leer_paquete(socketMemoria);

	if (paqueteInicio->header->codigo_operacion == 1) {

		int datonulo = 0;

		paqueteInicio = pedirPaquete(2, sizeof(int), &datonulo);

		common_send(socketMemoria, paqueteInicio);

		log_info(log_swap, "Conectado con la memoria en el socket: %d",
				socketMemoria);

	} else {

		log_info(log_swap, "Falla en la conexion con la memoria");

		exit(EXIT_FAILURE);
	}

	t_data * paquete;

	while (1) {

		paquete = leer_paquete(socketMemoria);

		switch (paquete->header->codigo_operacion) {

		case LEER: {

			int pid, page;

			memcpy(&pid, paquete->data, sizeof(int));
			memcpy(&page, paquete->data + sizeof(int), sizeof(int));

			char * content = readProcessPage(pid, page);

			paquete = pedirPaquete(LEER, getSwapPagesSize(), content);

			common_send(socketMemoria, paquete);

			break;
		}

		case ESCRIBIR: {

			int pid, page;
			char * content = malloc(getSwapPagesSize());

			memcpy(&pid, paquete->data, sizeof(int));
			memcpy(&page, paquete->data + sizeof(int), sizeof(int));
			memcpy(content, paquete->data + 2 * sizeof(int),
					getSwapPagesSize());

			writeProcessPage(pid, page, content);

			break;
		}

		case INICIAR: {

			int pid, pagesAmount;

			memcpy(&pid, paquete->data, sizeof(int));
			memcpy(&pagesAmount, paquete->data + sizeof(int), sizeof(int));

			int blank_pages = getBlankPages();
			int success;

			if (blank_pages >= pagesAmount) {
				success = reserve(pid, pagesAmount);

				if (success == -1) {
					compact();
					success = reserve(pid, pagesAmount);
				}
			}else{
				success = -1;
			}


			if (success == -1) {

				paquete = pedirPaquete(0, sizeof(int), &pid);
				log_info(log_swap,
						"No se pudo reservar las paginas solicitadas para el proceso con pid: %d",
						pid);

			} else {

				paquete = pedirPaquete(1, sizeof(int), &pid);
				int absolutePage = getProcessFirstPage(pid);

				int byteInicial = absolutePage * getSwapPagesSize();

				int size = pagesAmount * getSwapPagesSize();

				log_info(log_swap,
						"Se inicia el proceso %d en el byte %d con tamanio %d",
						pid, byteInicial, size);

			}

			common_send(socketMemoria, paquete);

			break;
		}

		case FINALIZAR: {

			int pid;

			memcpy(&pid, paquete->data, sizeof(int));

			int processSpace = getProcessReservedSpace(pid)
					* getSwapPagesSize();

			int byteInicial = getProcessFirstPage(pid) * getSwapPagesSize();

			freeSpace(pid);

			log_info(log_swap,
					"Proceso %d liberado. N° de byte inicial: %d, y tamaño en bytes: %d",
					pid, byteInicial, processSpace);

			break;
		}

		default: {

			break;
		}

		}

	}

	return 0;
}
