#include "tests_utils.h"

context(test_admin_memoria) {

	describe("tlb") {

		t_tlb * tlb_test;

		before {
			tlb_test = list_create();
		}end

		it("crea fila para el tlb") {

			t_tlb_item * item1 = new_tlb_item(1,1,5);

			assert_tlb_item(item1,1,1,5);

		}end

		it("agrega entradas al tlb") {

			tlb_agregar_entrada(tlb_test,1,1,5);

			should_int(list_size(tlb_test)) be equal to(1);

		}end

		it("remueve una entrada del tlb dado un pid y una página") {

			tlb_agregar_entrada(tlb_test,1,1,5);
			tlb_agregar_entrada(tlb_test,1,2,6);
			tlb_remover_entrada(tlb_test,1,2);

			should_int(list_size(tlb_test)) be equal to(1);

		}end

		it("remueve todas las entradas de la tlb dado un pid") {

			tlb_agregar_entrada(tlb_test,1,1,5);
			tlb_agregar_entrada(tlb_test,1,2,6);
			tlb_agregar_entrada(tlb_test,2,3,1);
			tlb_remover_entradas_para(tlb_test,1);

			should_int(list_size(tlb_test)) be equal to(1);

		}end

		it("encuentra un item, devuelve HIT y guarda el par pagina-marco en tabla_paginas_match") {

			tlb_agregar_entrada(tlb_test,1,1,5);

			should_int(tlb_buscar(tlb_test, 1, 1)) be equal to(HIT);

			assert_tlb_match(1,5);

		}end

		it("no encuentra un item y devuelve MISS") {

			tlb_agregar_entrada(tlb_test,1,1,5);

			should_int(tlb_buscar(tlb_test,2 ,1)) be equal to(MISS);

		}end

	}end

	describe("tabla de páginas") {

		t_tabla_paginas * tabla_paginas_test;

		before {
			tabla_paginas_test = list_create();
		}end

		it("crea la estructura de un proceso para la tabla de páginas") {

			t_tabla_paginas_item * item1 = new_tabla_paginas_item(2,1,2);

			assert_tabla_paginas_item(item1,2,1,2);

		}end

		it("agrega un item a la tabla de páginas") {

			tabla_paginas_agregar_entrada(tabla_paginas_test,2,1,2);

			should_int(list_size(tabla_paginas_test)) be equal to(1);

		}end

		it("agrega un par pagina-marco de un proceso") {

			t_tabla_paginas_item * item = new_tabla_paginas_item(1,1,5);
			tabla_paginas_add(tabla_paginas_test,item);
			tabla_paginas_anadir_par_pagina_marco(tabla_paginas_test,1,2,1);

			should_int(list_size(item->pares_pagina_marco)) be equal to(2);

		}end

		it("remueve un par pagina-marco de un proceso") {

			tabla_paginas_agregar_entrada(tabla_paginas_test,1,1,1);

			t_tabla_paginas_item * item = new_tabla_paginas_item(2,1,2);
			list_add(item->pares_pagina_marco,new_par_pagina_marco(2,3));
			list_add(tabla_paginas_test,item);

			tabla_paginas_remover_par(tabla_paginas_test,2,2);

			should_int(list_size(item->pares_pagina_marco)) be equal to(1);

		}end

		it("remueve un proceso") {

			tabla_paginas_agregar_entrada(tabla_paginas_test,1,1,1);
			tabla_paginas_agregar_entrada(tabla_paginas_test,2,1,2);

			tabla_paginas_remover(tabla_paginas_test,1);

			should_int(list_size(tabla_paginas_test)) be equal to(1);

		}end

		it("encuentra un item, devuelve HIT y guarda el par pagina-marco en tabla_paginas_match") {

			tabla_paginas_agregar_entrada(tabla_paginas_test,1,1,1);

			should_int(tabla_paginas_buscar(tabla_paginas_test,1 ,1)) be equal to(HIT);
			assert_tabla_paginas_match(1,1);

		}end

		it("no encuentra un item y devuelve MISS") {

			tabla_paginas_agregar_entrada(tabla_paginas_test,1,1,1);

			should_int(tabla_paginas_buscar(tabla_paginas_test,2 ,5)) be equal to(MISS);

		}end

	}end

	describe("devolver ubicación del marco de una página") {

		t_tlb * tlb_test;
		t_tabla_paginas * tabla_paginas_test;
		t_dictionary * diccionario_configuraciones_test;

		before {
			tlb_test = list_create();
			tabla_paginas_test = list_create();
			diccionario_configuraciones_test = dictionary_create();
		}end

		describe("con tlb habilitada") {

			before {
				dictionary_put(diccionario_configuraciones_test,"TLB_HABILITADA","SI");
			}end

			it("busca una página que está en la tlb") {

				tlb_agregar_entrada(tlb_test,1,1,2);

				should_int(marco_pagina(tlb_test,tabla_paginas_test,diccionario_configuraciones_test,1,1)) be equal to(2);

			}end

			it("busca una página que no está en la tlb y si en la tabla") {

				tlb_agregar_entrada(tlb_test,1,1,2);

				should_int(marco_pagina(tlb_test,tabla_paginas_test,diccionario_configuraciones_test,1,1)) be equal to(2);

			}end

		}end

		describe("con tlb deshabilitada") {

			before {
				dictionary_put(diccionario_configuraciones_test,"TLB_HABILITADA","NO");
			}end

			it("busca una página que está en la tabla") {

				tabla_paginas_agregar_entrada(tabla_paginas_test,1,1,2);

				should_int(marco_pagina(tlb_test,tabla_paginas_test,diccionario_configuraciones_test,1,1)) be equal to(2);

			}end

		}end

	}end

	describe("remover") {

		t_tlb * tlb_test;
		t_tabla_paginas * tabla_paginas_test;

		before {
			tlb_test = list_create();
			tabla_paginas_test = list_create();
		}end

		it("saca el proceso de todas las estructuras") {
			tlb_agregar_entrada(tlb_test,1,1,2);
			tlb_agregar_entrada(tlb_test,2,1,3);
			tabla_paginas_agregar_entrada(tabla_paginas_test,1,1,2);
			tabla_paginas_agregar_entrada(tabla_paginas_test,2,1,3);

			remover_tlb(tlb_test,1);
			remover_tabla_paginas(tabla_paginas_test,1);

			should_int(list_size(tabla_paginas_test)) be equal to(1);
			should_int(list_size(tlb_test)) be equal to(1);

		}end
	}end

}
