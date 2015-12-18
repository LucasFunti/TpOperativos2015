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

	log_swap = log_create("/tp-2015-2c-signiorcodigo/swap/log_swap",
			"SWAP", true, LOG_LEVEL_INFO);

	swapConfig = config_create("/tp-2015-2c-signiorcodigo/swap/swapConfig");

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

	char* file_name = getSwapFileName();

	char str[100];

	strcpy(str, "/tp-2015-2c-signiorcodigo/swap/Debug/");

	strcpy(str, file_name);

	int result = doesFileExist(str);

	if (!result) {

		setupSwap();

	} else {

		printf("El archivo de Swap ya existe. Continuamos...\n");

	}

	setPages();

	t_data * paquete;

	while (1) {

		paquete = leer_paquete(socketMemoria);

		switch (paquete->header->codigo_operacion) {

		case LEER: {

			int pid, page;

			memcpy(&pid, paquete->data, sizeof(int));
			memcpy(&page, paquete->data + sizeof(int), sizeof(int));

			char * content = readProcessPage(pid, page, log_swap);

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

			writeProcessPage(pid, page, content, log_swap);

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
			}

			if (success == -1) {

				paquete = pedirPaquete(0, sizeof(int), &pid);
				log_info(log_swap,"No se pudo reservar las paginas solicitadas para el proceso con pid: %d",pid);

			} else {

				paquete = pedirPaquete(1, sizeof(int), &pid);
				int absolutePage = getProcessFirstPage(pid) + pagesAmount;

				int byteInicial = absolutePage * getSwapPagesSize();

				log_info(log_swap,
						string_from_format(
								"Proceso mProc asignado, su PID es: %d, N° de byte inicial: %d, y tamaño en bytes: %d"),
						pid, byteInicial, pagesAmount * getSwapPagesSize());

			}

			common_send(socketMemoria, paquete);

			break;
		}

		case FINALIZAR:
		{

			int pid;

			memcpy(&pid, paquete->data, sizeof(int));

			freeSpace(pid);
			int absolutePage = getProcessFirstPage(pid) + getProcessReservedSpace(pid);

			int byteInicial = absolutePage * getSwapPagesSize();

			log_info(log_swap,"Proceso con pid: %d liberado, su byte inicial es: %d, y el tamaño liberado es:%d",pid,byteInicial,getProcessReservedSpace(pid)*getSwapPagesSize());

			break;
		}

		default:
		{

			break;
		}

	}

}

return 0;
}
