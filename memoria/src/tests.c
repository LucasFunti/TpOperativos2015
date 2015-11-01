#include <cspecs/cspec.h>
#include "pedidos.h"

context(test_admin_memoria) {

	describe("tlb") {

		void assert_tlb_item(t_tlb_item * fila_tlb, int pid, int pagina, int marco) {

			should_ptr(fila_tlb) not be null;
			should_int(fila_tlb->pid) be equal to(pid);
			should_int(fila_tlb->par_pagina_marco->pagina) be equal to(pagina);
			should_int(fila_tlb->par_pagina_marco->marco) be equal to(marco);

		}

		void assert_tlb_match(int pagina, int marco) {

			should_int(tlb_match->pagina) be equal to(pagina);
			should_int(tlb_match->marco) be equal to(marco);
		}

		t_tlb * tlb;

		before {
			tlb = list_create();
		}end

		it("crea items para el tlb") {

			t_tlb_item * item1 = new_tlb_item(1,1,5);

			assert_tlb_item(item1,1,1,5);

		}end

		it("agrega filas al tlb") {

			t_tlb_item * item1 = new_tlb_item(1,1,5);

			tlb_add(tlb,item1);

			should_int(list_size(tlb)) be equal to(1);

		}end

		it("remueve un item") {

			t_tlb_item * item1 = new_tlb_item(1,1,5);
			t_tlb_item * item2 = new_tlb_item(2,1,3);
			t_tlb_item * item3 = new_tlb_item(3,1,2);

			tlb_add(tlb,item1);
			tlb_add(tlb,item2);
			tlb_remover(tlb,1);

			tlb_add(tlb,item3);

			should_int(list_size(tlb)) be equal to(2);

		}end

		it("encuentra un item, devuelve HIT y guarda el par pagina-marco en tabla_paginas_match") {

			t_tlb_item * item1 = new_tlb_item(1,1,5);

			tlb_add(tlb,item1);

			should_int(tlb_buscar(tlb, 1, 1)) be equal to(HIT);
			assert_tlb_match(1,5);

		}end

		it("no encuentra un item y devuelve MISS") {

			t_tlb_item * item1 = new_tlb_item(1,1,5);

			tlb_add(tlb,item1);

			should_int(tlb_buscar(tlb,2 ,1)) be equal to(MISS);

		}end

	}end

	describe("tabla de páginas") {

		void assert_tabla_paginas_item(t_tabla_paginas_item * fila_tabla_paginas, int pid, int pagina, int marco) {

			should_ptr(fila_tabla_paginas) not be null;
			should_int(fila_tabla_paginas->pid) be equal to(pid);
			should_int(marco_para_pagina(fila_tabla_paginas,pagina))be equal to(marco);
		}

		void assert_tabla_paginas_match(int pagina, int marco) {

			should_int(tabla_paginas_match->pagina) be equal to(pagina);
			should_int(tabla_paginas_match->marco) be equal to(marco);
		}

		t_tabla_paginas * tabla_paginas;

		before {
			tabla_paginas = list_create();
		}end

		it("agrega filas a la tabla de páginas") {

			t_tabla_paginas_item * item1 = new_tabla_paginas_item(2,1,2);

			tabla_paginas_add(tabla_paginas,item1);

			should_int(list_size(tabla_paginas)) be equal to(1);

		}end

		it("remueve un par pagina-marco de un proceso") {

			t_tabla_paginas_item * item1 = new_tabla_paginas_item(1,1,5);
			t_tabla_paginas_item * item2 = new_tabla_paginas_item(2,1,3);
			t_tabla_paginas_item * item3 = new_tabla_paginas_item(3,1,2);

			tabla_paginas_add(tabla_paginas,item1);
			tabla_paginas_add(tabla_paginas,item2);
			tabla_paginas_remover(tabla_paginas,1);

			tabla_paginas_add(tabla_paginas,item3);

			should_int(list_size(tabla_paginas)) be equal to(2);

		}end

		it("agrega un par pagina-marco de un proceso") {

			t_tabla_paginas_item * item1 = new_tabla_paginas_item(1,1,5);
			t_tabla_paginas_item * item2 = new_tabla_paginas_item(2,1,3);

			tabla_paginas_add(tabla_paginas,item1);
			tabla_paginas_add(tabla_paginas,item2);
			tabla_paginas_anadir_par_pagina_marco(tabla_paginas,1,2,1);

			should_int(list_size(tabla_paginas)) be equal to(2);

		}end

		it("encuentra un item, devuelve HIT y guarda el par pagina-marco en tabla_paginas_match") {

			t_tabla_paginas_item * item1 = new_tabla_paginas_item(1,2,5);

			tabla_paginas_add(tabla_paginas,item1);

			should_int(tabla_paginas_buscar(tabla_paginas,1 ,2)) be equal to(HIT);
			assert_tabla_paginas_match(2,5);

		}end

		it("no encuentra un item y devuelve MISS") {

			t_tabla_paginas_item * item1 = new_tabla_paginas_item(1,2,5);

			tabla_paginas_add(tabla_paginas,item1);

			should_int(tabla_paginas_buscar(tabla_paginas,2 ,5)) be equal to(MISS);

		}end

	}end

	describe("pedido de marcos") {

		before {
			tlb = list_create();
			tabla_paginas = list_create();
			diccionario_configuraciones = dictionary_create();
		}end

		describe("con tlb habilitada") {

			before {
				dictionary_put(diccionario_configuraciones,"TLB_HABILITADA","SI");
			}end

			it("busca una página que está en la tlb") {

				tlb_add(tlb,new_tlb_item(1,1,2));
				tabla_paginas_add(tabla_paginas,new_tabla_paginas_item(1,1,2));

				should_int(leer_n(1,1)) be equal to(2);

			}end

			it("busca una página que no está en la tlb y si en la tabla") {

				tabla_paginas_add(tabla_paginas,new_tabla_paginas_item(1,1,2));

				should_int(leer_n(1,1)) be equal to(2);

			}end

		}end

		describe("con tlb deshabilitada") {

			before {
				dictionary_put(diccionario_configuraciones,"TLB_HABILITADA","NO");
			}end

			it("busca una página que está en la tabla") {

				tabla_paginas_add(tabla_paginas,new_tabla_paginas_item(1,1,2));

				should_int(leer_n(1,1)) be equal to(2);

			}end

		}end

	}end

}
