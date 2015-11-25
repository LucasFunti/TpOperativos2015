#include "tests_utils.h"

context(test_admin_memoria) {

	describe("iniciar_n") {

		remove(
				"/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/logTest");

		logger =
		log_create(
				"/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/logTest",
				"Test", true, LOG_LEVEL_DEBUG);

		describe("sin tlb") {

			before {

				memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionSinTlb");
				iniciar_marcos();

			}end

			it("agrega una entrada a la tabla de páginas") {

				should_bool(iniciar_n(1,2,true)) be equal to (true);

				should_int(list_size(tabla_paginas)) be equal to(2);

				assert_tabla_paginas_item(list_get(tabla_paginas,0),1,1,0,false,true);
				assert_tabla_paginas_item(list_get(tabla_paginas,1),1,2,1,false,true);

			}end

			it("no puede registrar el proceso por exceso de páginas") {

				should_bool(iniciar_n(1,5,true)) be equal to (false);
				should_int(list_size(tabla_paginas)) be equal to(0);

			}end

			it("agrega una entrada a la tabla de páginas swapeando") {

				should_bool(iniciar_n(1,3,true)) be equal to (true);

				should_int(list_size(tabla_paginas)) be equal to(3);

				assert_tabla_paginas_item(list_get(tabla_paginas,0),1,1,0,false,false);
				assert_tabla_paginas_item(list_get(tabla_paginas,1),1,2,1,false,true);
				assert_tabla_paginas_item(list_get(tabla_paginas,2),1,3,0,false,true);

			}end

		}end

		describe("con tlb") {

			before {

				memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
				iniciar_marcos();

			}end

			it("agrega una entrada a la tabla de páginas swapeando intentando remover la entrada swapeada de la tlb") {

				should_bool(iniciar_n(1,1,true)) be equal to (true);

				leer_n(1,1);

				iniciar_n(2,2,true);

				//Como swappeo una entrada que estaba en la tlb, la remueve

				should_int(list_size(tabla_paginas)) be equal to(3);
				should_int(list_size(tlb)) be equal to(1);
				should_int(list_size(cola_llegada)) be equal to(2);

				assert_tabla_paginas_item(list_get(tabla_paginas,0),1,1,0,false,false);
				assert_tabla_paginas_item(list_get(tabla_paginas,1),2,1,1,false,true);
				assert_tabla_paginas_item(list_get(tabla_paginas,2),2,2,0,false,true);

			}end

			it("calcula correctamente la tasa de aciertos globales y la tasa de acierto por proceso") {

				iniciar_n(1,2,true);

				leer_n(1,1);
				leer_n(1,1);
				leer_n(1,1);
				leer_n(1,2);

				should_int(pedidos_totales) be equal to(4);
				should_int(aciertos_totales) be equal to(2);

				should_int(registroTlb[1].aciertos) be equal to(2);
				should_int(registroTlb[1].pedidos) be equal to(4);

			}end

		}end

	}end

	describe("escribir_n") {

		before {

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionSinTlb");
			iniciar_marcos();

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

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,1,true);
			iniciar_n(2,1,true);

			leer_n(1,1);
			leer_n(2,1);

			finalizar(1,true);

			should_int(list_size(tlb)) be equal to(1);
			should_int(list_size(tabla_paginas)) be equal to(1);
			should_int(list_size(cola_llegada)) be equal to(1);

		}end

		it("con un pid que no existe no remueve nada de ningún lado") {

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

		}end

		it("swappea paginas por fifo") {

			iniciar_n(1,5,true);

			should_int(list_size(cola_llegada)) be equal to(3);

			t_tlb_item * item = list_get(tabla_paginas,3);

			assert_tabla_paginas_item(item, 1, 4,0, false,true);

			item = list_get(tabla_paginas,4);

			assert_tabla_paginas_item(item, 1, 5,1, false,true);

			item = list_get(tabla_paginas,0);

			assert_tabla_paginas_item(item, 1, 1,0, false,false);

			item = list_get(tabla_paginas,1);

			assert_tabla_paginas_item(item, 1, 2,1, false,false);

		}end

	}end

	describe("lru") {

		before {

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionLru");
			iniciar_marcos();

		}end

		it("swappea usando lru") {

			iniciar_n(1,3,true);

			leer_n(1,3);
			leer_n(1,1);
			leer_n(1,2);

			//No tiene lugar y usa lru
			iniciar_n(2,1,true);

			t_tlb_item * item = list_get(tabla_paginas,3);

			assert_tabla_paginas_item(item, 2, 1,2, false,true);

			item = list_get(tabla_paginas,2);

			assert_tabla_paginas_item(item, 1, 3,2, false,false);

			iniciar_n(3,1,true);

			item = list_get(tabla_paginas,4);

			assert_tabla_paginas_item(item, 3, 1,0, false,true);

			item = list_get(tabla_paginas,0);

			assert_tabla_paginas_item(item, 1, 1,0, false,false);

		}end

	}end

	describe("clock_m") {

		before {

			memoriaConfig= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionClock-M");
			iniciar_marcos();

		}end

		it("swappea usando clock_m") {

			iniciar_n(1,2,true);
			iniciar_n(2,1,true);
			iniciar_n(3,2,true);
			//Aloja uno y desaloja el marco cero que era del proceso 1 pagina 1
			//El puntero pasó al segundo elemento ( p1 pag 2)

			t_tlb_item * item = list_get(tabla_paginas,4);

			assert_tabla_paginas_item(item, 3, 2, 0, false,true);

			item = list_get(tabla_paginas,0);

			assert_tabla_paginas_item(item, 1, 1, 0, false,false);

			iniciar_n(4,2,true);
			//Para la pagina 1 desaloja al segundo elemento (p1 pag2). El puntero pasa a ser el tercer elemento
			item = list_get(tabla_paginas,5);

			assert_tabla_paginas_item(item, 4, 1, 1, false,true);

			item = list_get(tabla_paginas,6);

			assert_tabla_paginas_item(item, 4, 2, 2, false,true);

		}end

	}end

}
