#include "marcos.h"

void iniciar_marcos(t_dictionary * diccionario_confgs) {

	cantidad_marcos = atoi(
			dictionary_get(diccionario_confgs, "CANTIDAD_MARCOS"));

	tamanio_marcos = atoi(dictionary_get(diccionario_confgs, "TAMANIO_MARCO"));

	memoria = malloc(tamanio_marcos * cantidad_marcos);

	if (memoria == NULL) {
		exit(EXIT_FAILURE);
	}

	marcos_disponibles = list_create();

	t_marco * marco;
	int i;
	for (i = 0; i < cantidad_marcos; i++) {

		marco = malloc(sizeof(t_marco));

		marco->numero_marco = i;
		marco->disponible = true;

		list_add(marcos_disponibles, true);
	}
}

int cantidad_marcos_libres() {

	return list_count_satisfying(marcos_disponibles, esta_libre);

}

bool esta_libre(void * data) {

	t_marco * marco = data;

	return (marco->disponible == true);
}
