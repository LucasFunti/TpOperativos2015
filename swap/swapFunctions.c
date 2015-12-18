/*
 * swapFunctions.c
 *
 * Created on: 28/9/2015
 * Author: utnso
 */
#include "swapFunctions.h"
int doesFileExist(const char *filename) {
	FILE *fp = fopen(filename, "r");
	if (fp != NULL)
		fclose(fp);
	return (fp != NULL);
}
int getSwapPagesAmount() {
	t_config *swapConfiguracion;
	swapConfiguracion = config_create(
			"/tp-2015-2c-signiorcodigo/swap/swapConfig");
	int pages_amount = config_get_int_value(swapConfiguracion,
			"CANTIDAD_PAGINAS");
	return pages_amount;
}
int getSwapPagesSize() {
	t_config *swapConfiguracion;
	swapConfiguracion = config_create(
			"/tp-2015-2c-signiorcodigo/swap/swapConfig");
	int page_size = config_get_int_value(swapConfiguracion, "TAMANIO_PAGINA");
	return page_size;
}
char* getSwapFileName() {
	t_config *swapConfiguracion;
	swapConfiguracion = config_create(
			"/tp-2015-2c-signiorcodigo/swap/swapConfig");
	char* file_name = config_get_string_value(swapConfiguracion, "NOMBRE_SWAP");
	return file_name;
}
void setupSwap() {
//Traigo datos del archivo de configuracion de Swap y creo un archivo
//con las especificaciones de configuracion con /0
	int pages_amount = getSwapPagesAmount();
	int page_size = getSwapPagesSize();
	char *file_name = getSwapFileName();
	char command[100];
	int total = pages_amount * page_size;
	sprintf(command, "dd if=/dev/zero of=%s bs=%d count=1", file_name, total);
	system(command);
//printf("Archivo de swap creado bajo el nombre %s\n",file_name );
}
void setPages() {

	int pagesAmount = getSwapPagesAmount();
	pages = list_create();
	int i;
	int top = pagesAmount;
	for (i = 0; i < top; i++) {
		t_nodo_swap *newItemPtr = malloc(sizeof(t_nodo_swap));
		newItemPtr->numeroPagina = i;
		newItemPtr->pid = -1;
		list_add(pages, newItemPtr);
	}
}
void markPage(int absolutePageNumber, int pid) {

	t_nodo_swap* item = list_get(pages, absolutePageNumber);

	item->pid = pid;

}

void writePage(int absolutePageNumber, char *content) {

	int page_size = getSwapPagesSize();
	char *file_name = getSwapFileName();
	FILE *fp = fopen(file_name, "rb+");
	int position = absolutePageNumber * page_size;
	fseek(fp, position, SEEK_SET);
	fwrite(content, sizeof(char), strlen(content), fp);
	fclose(fp);
}

char* readPage(int absolutePageNumber) {
	int page_size = getSwapPagesSize();
	char *file_name = getSwapFileName();
	FILE *fp = fopen(file_name, "r");
	char* page = malloc(sizeof(char) * page_size);
	int position = absolutePageNumber * page_size;
	fseek(fp, position, SEEK_SET);
	fgets(page, page_size + 1, fp);
	return page;
}

//void evaluateAction(char* action) {
//	if (strcmp(action, "reservar") == 0) {
//		printf("Ingrese nombre de Proceso\n");
//		char *process_name = malloc(sizeof(char) * 15);
//		scanf("%s", process_name);
//		printf("Ingrese cantidad de paginas a reservar\n");
//		int amount_to_reserve;
//		scanf("%d", &amount_to_reserve);
//		reserve(process_name, amount_to_reserve, pages);
//	} else if (strcmp(action, "escribir") == 0) {
//		printf("Ingrese nombre de Proceso\n");
//		char *process_name = malloc(sizeof(char) * 15);
//		scanf("%s", process_name);
//		int hasReservedPages;
//		hasReservedPages = getProcessReservedSpace(process_name, pages);
//		if (hasReservedPages >= 0) {
//			char *text = "texto de prueba para signior funti";
//			int start = getProcessFirstPage(process_name, pages);
//			writePage(start, text);
//			markPage(start, process_name, pages);
//		} else {
//			printf("El proceso no posee páginas reservadas\n");
//		}
//	} else if (strcmp(action, "leer") == 0) {
//		printf("Ingrese pagina de lectura\n");
//		int pagina_lectura;
//		scanf("%d", &pagina_lectura);
//		char *retorno = readPage(pagina_lectura);
//		printf("%s\n", retorno);
//	} else if (strcmp(action, "compactar") == 0) {
//		compact(pages);
//	} else if (strcmp(action, "liberar") == 0) {
//		printf("Ingrese nombre de Proceso\n");
//		char *process_name = malloc(sizeof(char) * 15);
//		scanf("%s", process_name);
//		freeSpace(process_name, pages);
//	} else if (strcmp(action, "imprimir") == 0) {
//		printf("Paginas\n");
//		imprimir(pages);
//	} else {
//		printf("Accion desconocida\n");
//	}
//}

int checkSpaceAvailability(int amount) {

	int top = list_size(pages);
	int count = 0;
	int i;
	if (amount <= list_size(pages)) {
		for (i = 0; i < top; i++) {

			t_nodo_swap *newItemPtr = (t_nodo_swap*) list_get(pages, i);

			if (newItemPtr->pid == -1) {

				count++;

			} else {

				count = 0;
			}

			if (count == amount) {

				return (i - amount + 1);

			}
		}
	}

	return -1;
}

int getLargestContiguousSpace() {
	int top = list_size(pages);
	int largest = 0;
	int count = 0;
	int i;
	for (i = 0; i < top; i++) {
		t_nodo_swap *newItemPtr = (t_nodo_swap*) list_get(pages, i);
		if (newItemPtr->pid == -1) {
			count++;
		} else {
			if (count > largest) {
				largest = count;
			}
			count = 0;
		}
	}
	if (count > largest) {
		largest = count;
	}
	return largest;
}

void freeSpace(int pid) {

	int top = list_size(pages);
	int i;
	for (i = 0; i < top; i++) {
		t_nodo_swap *newItemPtr = (t_nodo_swap*) list_get(pages, i);
		if (newItemPtr->pid == pid) {
			newItemPtr->pid = -1;
		}
	}
}

int reserve(int pid, int amount) {

	int result = checkSpaceAvailability(amount);

	if (result >= 0) {
		int i;
		int top = result + amount;
		for (i = result; i < top; i++) {
			markPage(i, pid);
		}
//printf("Paginas reservadas\n");
		return 0;
	} else {
//printf("No existe espacio suficiente para el Proceso\n");
		return -1;
	}
}
//Retorna la primer pagina reservada para
//el proceso o -1 si no tiene espacio reservado

int getProcessFirstPage(int pid) {
	int i;
	int top = getSwapPagesAmount();
	t_nodo_swap *newItemPtr;
	for (i = 0; i < top; i++) {
		newItemPtr = (t_nodo_swap*) list_get(pages, i);

		if (newItemPtr->pid == pid) {
			return newItemPtr->numeroPagina;
		}
	}
	return -1;
}

int getProcessReservedSpace(int pid) {
	int i;
	int top = getSwapPagesAmount();
	t_nodo_swap *newItemPtr;
	int amount = 0;
	for (i = 0; i < top; i++) {
		newItemPtr = (t_nodo_swap*) list_get(pages, i);
		if (newItemPtr->pid == pid) {
			amount++;
		}
	}
	return amount;
}
//Usando 2 listas voy copiando las posiciones en las listas

void compact() {
	int i;
	int top = getSwapPagesAmount();
	t_nodo_swap *newItemPtr;
	int index = 0;
	t_list *sortedPages = list_create();
	for (i = 0; i < top; i++) {

		newItemPtr = (t_nodo_swap*) list_get(pages, i);

		if (newItemPtr->pid != -1) {

			t_nodo_swap * newListItemPtr = malloc(sizeof(t_nodo_swap));

			newListItemPtr->numeroPagina = index;

			newListItemPtr->pid = newItemPtr->pid;

			list_add(sortedPages, newListItemPtr);

			copyPage(i, index);

			index++;
		}
	}

	pages = sortedPages;

	fillRemainingSpace();
}
//Leo una pagina y luego la muevo a la posicion deseada
void copyPage(int from, int to) {

	char *page = readPage(from);
	writePage(to, page);

}
void fillRemainingSpace() {

	int top = getSwapPagesAmount();
	int i;
	int from = list_size(pages);

	for (i = from; i < top; i++) {

		t_nodo_swap *newItemPtr = list_get(pages, i);
		newItemPtr->numeroPagina = i;
		newItemPtr->pid = -1;

	}
}
//Imprime todas las paginas de la lista Indice,
//Mostrando numero de pagina y nombre de proceso
//void imprimir() {
//	int size = list_size(list);
//	int i;
//	for (i = 0; i < size; i++) {
//		t_nodo_swap *newItemPtr = list_get(list, i);
//		printf("%d ", newItemPtr->numeroPagina);
//		printf("%s\n", newItemPtr->nombreProceso);
//	}
//}

int getBlankPages() {

	int i;
	int top = getSwapPagesAmount();
	int count = 0;

	t_nodo_swap *newItemPtr;

	for (i = 0; i < top; i++) {

		newItemPtr = list_get(pages, i);

		if (newItemPtr->pid == -1) {
			count++;
		}
	}
	return count;
}
char* getPort() {
	t_config *swapConfiguracion;
	swapConfiguracion = config_create(
			"/tp-2015-2c-signiorcodigo/swap/swapConfig");
	char * port = config_get_string_value(swapConfiguracion, "PUERTO_ESCUCHA");
	return port;
}
t_data * pedirPaquete(int codigoOp, int tamanio, void * data) {
	t_data * paquete = malloc(sizeof(t_data));
	paquete->header = malloc(sizeof(t_header));
	paquete->header->codigo_operacion = codigoOp;
	paquete->header->tamanio_data = tamanio;
	paquete->data = data;
	return paquete;
}
char * serializarPaquete(t_data * unPaquete) {
	void * stream = malloc(sizeof(t_header) + unPaquete->header->tamanio_data);
	memcpy(stream, unPaquete->header, sizeof(t_header));
	memcpy(stream + sizeof(t_header), unPaquete->data,
			unPaquete->header->tamanio_data);
	return stream;
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
void common_send(int socket, t_data * paquete) {
	char* buffer;
	int tamanio_total;
	buffer = serializarPaquete(paquete);
	tamanio_total = paquete->header->tamanio_data + sizeof(t_header);
	send(socket, buffer, tamanio_total, MSG_WAITALL);
	free(buffer);
}

void writeProcessPage(int pid, int nPage, char * content) {

	int absolutePage = getProcessFirstPage(pid) + nPage;

	writePage(absolutePage, content);

}

char * readProcessPage(int pid, int nPage) {

	int absolutePage = getProcessFirstPage(pid) + nPage;

	return readPage(absolutePage);

}
