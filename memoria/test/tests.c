#include "tests_utils.h"

context(test_admin_memoria) {

	describe("iniciar_n") {

		before {

			tabla_paginas = list_create();

		}end

		describe("sin tlb") {

			before {

				memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionSinTlb");
				iniciar_marcos();

			}end

			it("agrega una entrada a la tabla de páginas") {

				should_bool(iniciar_n(1,2,true)) be equal to (true);

				should_int(list_size(tabla_paginas)) be equal to(2);

				assert_tabla_paginas_item(list_get(tabla_paginas,0),1,1,0,false);
				assert_tabla_paginas_item(list_get(tabla_paginas,1),1,2,1,false);

			}end

			it("no puede registrar el proceso por exceso de páginas") {

				should_bool(iniciar_n(1,5,true)) be equal to (false);
				should_int(list_size(tabla_paginas)) be equal to(0);

			}end

			it("agrega una entrada a la tabla de páginas swapeando") {

				should_bool(iniciar_n(1,3,true)) be equal to (true);

				should_int(list_size(tabla_paginas)) be equal to(2);
				assert_tabla_paginas_item(list_get(tabla_paginas,0),1,2,1,false);
				assert_tabla_paginas_item(list_get(tabla_paginas,1),1,3,0,false);

			}end

		}end

		describe("con tlb") {

			before {

				memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
				iniciar_marcos();
				tlb = list_create();

			}end

			it("agrega una entrada a la tabla de páginas swapeando intentando remover la entrada swapeada de la tlb") {

				should_bool(iniciar_n(1,3,true)) be equal to (true);

				should_int(list_size(tabla_paginas)) be equal to(2);
				assert_tabla_paginas_item(list_get(tabla_paginas,0),1,2,1,false);
				assert_tabla_paginas_item(list_get(tabla_paginas,1),1,3,0,false);

			}end

		}end

	}end

	describe("escribir_n") {

		before {

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionSinTlb");
			iniciar_marcos();
			tabla_paginas = list_create();

		}end

		it("escribe estando en memoria") {

			iniciar_n(1,2,true);

			escribir_n(1,1,"Hola");
			escribir_n(1,2,"Chau");

			int tamanio_marco = config_get_int_value(memoriaConfig, "TAMANIO_MARCO");

			char * pagina_1 = malloc(tamanio_marco);
			char * pagina_2 = malloc(tamanio_marco);

			memcpy(pagina_1,memoria,tamanio_marco);
			memcpy(pagina_2,memoria+tamanio_marco,tamanio_marco);

			should_string((char *)pagina_1) be equal to("Hola");
			should_string((char *)pagina_2) be equal to("Chau");

		}end

		it("escribe estando en memoria, usando leer_n") {

			iniciar_n(1,2,true);

			escribir_n(1,1,"Hola");
			escribir_n(1,2,"Chau");

			should_string(leer_n(1,1)) be equal to("Hola");
			should_string(leer_n(1,2)) be equal to("Chau");

		}end

		it("escribe estando en la tlb") {

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,2,true);

			escribir_n(1,1,"Hola");
			escribir_n(1,2,"Chau");

			should_int(list_size(tlb)) be equal to(2);
			should_string(leer_n(1,1)) be equal to("Hola");
			should_string(leer_n(1,2)) be equal to("Chau");

		}end

	}end

	describe("leer_n") {

		before {

			tabla_paginas = list_create();
			tlb = list_create();

		}end

		it("lee estando en memoria") {

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionSinTlb");
			iniciar_marcos();

			iniciar_n(1,2,true);

			escribir_n(1,1,"Hola");
			escribir_n(1,2,"Chau");

			should_string(leer_n(1,1)) be equal to("Hola");
			should_string(leer_n(1,2)) be equal to("Chau");

		}end

		it("lee estando en tlb") {

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,2,true);

			escribir_n(1,1,"Hola");
			escribir_n(1,2,"Chau");

			should_string(leer_n(1,1)) be equal to("Hola");
			should_string(leer_n(1,2)) be equal to("Chau");

		}end

	}end

	describe("finalizar") {

		it("remueve procesos de la tlb y la tabla liberando marcos") {

			tabla_paginas = list_create();
			tlb = list_create();

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,1,true);
			iniciar_n(2,1,true);

			tlb_agregar_entrada(1,1,0,false);
			tlb_agregar_entrada(2,1,1,false);

			finalizar(1,true);

			should_int(list_size(tlb)) be equal to(1);
			should_int(list_size(tabla_paginas)) be equal to(1);

		}end

		it("con un pid que no existe no remueve nada de ningún lado") {

			tabla_paginas = list_create();
			tlb = list_create();

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,2,true);

			tlb_agregar_entrada(1,1,0,false);
			tlb_agregar_entrada(1,2,1,false);

			finalizar(2,true);

			should_int(list_size(tlb)) be equal to(2);
			should_int(list_size(tabla_paginas)) be equal to(2);

		}end

	}end

	describe("señales") {

		it("vuelca el contenido de la memoria en un log") {

			tabla_paginas = list_create();
			tlb = list_create();

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			remove("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/log_test_volcado_memoria");

			logger = log_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/log_test_volcado_memoria", "PAM", true, LOG_LEVEL_DEBUG);
			loggearInfo("Inicio del test");

			iniciar_n(1,2,true);

			escribir_n(1,1,"Hola");
			escribir_n(1,2,"Chau");

			volcar_memoria_principal();
			loggearInfo("Fin del test");

			should_int(1) be equal to(1);

		}end

		it("vacia la tlb") {

			tabla_paginas = list_create();
			tlb = list_create();

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,2,true);

			escribir_n(1,1,"Hola");
			escribir_n(1,2,"Chau");

			vaciar_tlb();

			should_int(list_size(tlb)) be equal to(0);
			should_int(list_size(tabla_paginas)) be equal to(2);
		}end

		it("vacia la tabla de paginas") {

			tabla_paginas = list_create();
			tlb = list_create();

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,2,true);

			vaciar_tabla_paginas();

			should_int(list_size(tlb)) be equal to(0);
			should_int(list_size(tabla_paginas)) be equal to(0);
		}end

	}end

	describe("fifo") {

		before {

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionFifo");
			iniciar_marcos();
			tabla_paginas = list_create();

		}end

		it("swappea paginas por fifo") {

			iniciar_n(1,5,true);

			//Es el anteultimo (el primero con el que usa fifo)
			t_tlb_item * item = list_get(tabla_paginas,1);

			assert_tabla_paginas_item(item, 1, 4,0, false);

			//Es el ultimo (el segundo con el que usa fifo)
			item = list_get(tabla_paginas,2);

			assert_tabla_paginas_item(item, 1, 5,1, false);

		}end

	}end

	describe("lru") {

		before {

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionLru");
			iniciar_marcos();
			tabla_paginas = list_create();

		}end

		it("swappea usando lru") {

			//Tiene 3 entradas la tabla de páginas
			iniciar_n(1,3,true);

			//Tendria que volar esta ya que fué la ultima utilizada
			leer_n(1,3);
			leer_n(1,1);
			leer_n(1,2);

			//No tiene lugar y usa lru
			iniciar_n(2,1,true);

			t_tlb_item * item = list_get(tabla_paginas,2);

			assert_tabla_paginas_item(item, 2, 1,2, false);

		}end

	}end

	describe("clock_m") {

		before {

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionClock-M");
			iniciar_marcos();
			tabla_paginas = list_create();

		}end

		it("agrega una entrada a la tabla de páginas swapeando intentando remover la entrada swapeada de la tlb") {

			iniciar_n(1,2,true);
			iniciar_n(2,1,true);
			iniciar_n(3,2,true);
			//Aloja uno y desaloja el marco cero que era del proceso 1 pagina 1
			//El puntero pasó al segundo elemento ( p1 pag 2)

			t_tlb_item * item = list_get(tabla_paginas,3);

			assert_tabla_paginas_item(item, 3, 2, 0, false);

			iniciar_n(4,2,true);
			//Para la pagina 1 desaloja al segundo elemento (p1 pag2). El puntero pasa a ser el tercer elemento
			item = list_get(tabla_paginas,2);

			assert_tabla_paginas_item(item, 4, 1, 1, false);

			item = list_get(tabla_paginas,3);

			assert_tabla_paginas_item(item, 4, 2, 2, false);

		}end

	}end

}
