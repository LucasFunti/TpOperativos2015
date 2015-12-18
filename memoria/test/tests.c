#include "tests_utils.h"

context(test_admin_memoria) {

	test = true;
	describe("iniciar_n") {

		describe("sin tlb") {

			before {

				remove("/tp-2015-2c-signiorcodigo/memoria/test/logTest");

				logger = log_create("/tp-2015-2c-signiorcodigo/memoria/test/logTest",
						"Test", false, LOG_LEVEL_DEBUG);

				memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionSinTlb");
				iniciar_marcos();

			}end

			it("agrega entradas a la tabla de páginas") {

				should_bool(iniciar_n(1,2)) be equal to (true);

				should_int(list_size(tabla_paginas)) be equal to(2);

				assert_tabla_paginas_item(list_get(tabla_paginas,0),1,0,0,false,false);
				assert_tabla_paginas_item(list_get(tabla_paginas,1),1,1,0,false,false);

			}end

		}end

		describe("con tlb") {

			before {

				remove("/tp-2015-2c-signiorcodigo/memoria/test/logTest");

				logger = log_create("/tp-2015-2c-signiorcodigo/memoria/test/logTest",
						"Test", false, LOG_LEVEL_DEBUG);

				memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
				iniciar_marcos();

			}end

			it("agrega una entrada a la tabla de páginas y con una lectura se carga en la tlb") {

				should_bool(iniciar_n(1,1)) be equal to (true);

				leer_n(1,0);

				iniciar_n(2,2);

				should_int(list_size(tabla_paginas)) be equal to(3);
				should_int(list_size(tlb)) be equal to(1);

				assert_tabla_paginas_item(list_get(tabla_paginas,0),1,0,0,false,true);

				leer_n(1,0);

				registroTlb[1].pedidos = 2;
				registroTlb[1].aciertos = 1;

			}end

			it("calcula correctamente la tasa de aciertos globales y la tasa de acierto por proceso") {

				iniciar_n(1,2);

				leer_n(1,0);
				leer_n(1,0);
				leer_n(1,0);
				leer_n(1,1);

				should_int(pedidos_totales) be equal to(4);
				should_int(aciertos_totales) be equal to(2);

				should_int(registroTlb[1].aciertos) be equal to(2);
				should_int(registroTlb[1].pedidos) be equal to(4);

			}end

		}end

	}end

	describe("escribir_n") {

		before {

			remove("/tp-2015-2c-signiorcodigo/memoria/test/logTest");

			logger = log_create("/tp-2015-2c-signiorcodigo/memoria/test/logTest",
					"Test", false, LOG_LEVEL_DEBUG);

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionSinTlb");
			iniciar_marcos();

		}end

		it("escribe estando en memoria") {

			iniciar_n(1,2);

			escribir_n(1,0,"Hola");
			escribir_n(1,1,"MasDe10Caracteres");

			int tamanio_marco = config_get_int_value(memoriaConfig, "TAMANIO_MARCO");

			char * pagina_0 = malloc(tamanio_marco);
			char * pagina_1 = malloc(tamanio_marco);

			memcpy(pagina_0,memoria,tamanio_marco);
			memcpy(pagina_1,memoria+tamanio_marco,tamanio_marco);

			should_string((char *)pagina_0) be equal to("Hola");
			should_string((char *)pagina_1) be equal to("MasDe10Ca");

		}end

		it("escribe estando en memoria, usando leer_n") {

			iniciar_n(1,2);

			escribir_n(1,0,"Hola");
			escribir_n(1,1,"Chau");

			should_bool(leer_n(1,0)) be equal to(true);
			should_string(contenido_lectura) be equal to("Hola");

			should_bool(leer_n(1,1)) be equal to(true);
			should_string(contenido_lectura) be equal to("Chau");

		}end

		it("escribe estando en la tlb") {

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,2);

			escribir_n(1,0,"Hola");
			escribir_n(1,1,"Chau");

			should_int(list_size(tlb)) be equal to(2);

			should_bool(leer_n(1,0)) be equal to(true);
			should_string(contenido_lectura) be equal to("Hola");

			should_bool(leer_n(1,1)) be equal to(true);
			should_string(contenido_lectura) be equal to("Chau");

		}end

	}end

	describe("leer_n") {

		before {

			remove("/tp-2015-2c-signiorcodigo/memoria/test/logTest");

			logger = log_create("/tp-2015-2c-signiorcodigo/memoria/test/logTest",
					"Test", false, LOG_LEVEL_DEBUG);
		}end

		it("lee estando en memoria") {

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionSinTlb");
			iniciar_marcos();

			iniciar_n(1,2);

			escribir_n(1,0,"Hola");
			escribir_n(1,1,"Chau");

			should_bool(leer_n(1,0)) be equal to(true);
			should_string(contenido_lectura) be equal to("Hola");

			should_bool(leer_n(1,1)) be equal to(true);
			should_string(contenido_lectura) be equal to("Chau");

		}end

		it("lee estando en tlb") {

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,2);

			escribir_n(1,0,"Hola");
			escribir_n(1,1,"Chau");

			should_bool(leer_n(1,0)) be equal to(true);
			should_string(contenido_lectura) be equal to("Hola");

			should_bool(leer_n(1,1)) be equal to(true);
			should_string(contenido_lectura) be equal to("Chau");

		}end

	}end

	describe("finalizar") {

		it("remueve procesos de la tlb y la tabla liberando marcos") {

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,1);
			iniciar_n(2,1);

			leer_n(1,0);
			leer_n(2,0);

			finalizar(1);

			should_int(list_size(tlb)) be equal to(1);
			should_int(list_size(tabla_paginas)) be equal to(1);
			should_int(list_size(cola_llegada)) be equal to(1);

		}end

		it("con un pid que no existe no remueve nada de ningún lado") {

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,2);

			escribir_n(1,0,"Hola");
			escribir_n(1,1,"Hola");

			finalizar(2);

			should_int(list_size(tlb)) be equal to(2);
			should_int(list_size(tabla_paginas)) be equal to(2);

		}end

	}end

	describe("señales") {

		before {
			remove("/tp-2015-2c-signiorcodigo/memoria/test/logTest");

			logger = log_create("/tp-2015-2c-signiorcodigo/memoria/test/logTest",
					"Test", false, LOG_LEVEL_DEBUG);
		}end

		it("vuelca el contenido de la memoria en un log") {

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			remove("/tp-2015-2c-signiorcodigo/memoria/test/log_test_volcado_memoria");

			printf("\n");

			logger = log_create("/tp-2015-2c-signiorcodigo/memoria/test/log_test_volcado_memoria", "PAM", false, LOG_LEVEL_DEBUG);
			loggearInfo("Inicio del test");

			iniciar_n(1,2);

			escribir_n(1,0,"Hola");
			escribir_n(1,1,"Chau");

			volcar_memoria_principal();
			loggearInfo("Fin del test");
			printf("\n");

			should_int(1) be equal to(1);

		}end

		it("vacia la tlb") {

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,2);

			escribir_n(1,0,"Hola");
			escribir_n(1,1,"Chau");

			vaciar_tlb();

			should_int(list_size(tlb)) be equal to(0);
			should_int(list_size(tabla_paginas)) be equal to(2);
		}end

		it("vacia la tabla de paginas") {

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos();

			iniciar_n(1,2);

			vaciar_memoria_principal();

			should_int(list_size(tlb)) be equal to(0);
			should_int(list_size(tabla_paginas)) be equal to(0);
		}end

	}end

	describe("fifo") {

		before {

			remove("/tp-2015-2c-signiorcodigo/memoria/test/logTest");

			logger = log_create("/tp-2015-2c-signiorcodigo/memoria/test/logTest",
					"Test", false, LOG_LEVEL_DEBUG);

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionFifo");
			iniciar_marcos();

			/*
			 MAXIMO_MARCOS_POR_PROCESO=5
			 CANTIDAD_MARCOS=3
			 TAMANIO_MARCO=10
			 */

		}end

		it("swappea paginas por fifo") {

			iniciar_n(1,5);

			escribir_n(1,0,"Hola");
			escribir_n(1,1,"Hola");
			escribir_n(1,2,"Hola");
			escribir_n(1,3,"Hola");

			should_int(list_size(cola_llegada)) be equal to(3);

			assert_tabla_paginas_item(list_get(tabla_paginas,0),1,0,0,false,false);
			assert_tabla_paginas_item(list_get(tabla_paginas,1),1,1,1,true,true);
			assert_tabla_paginas_item(list_get(tabla_paginas,2),1,2,2,true,true);
			assert_tabla_paginas_item(list_get(tabla_paginas,3),1,3,0,true,true);

			escribir_n(1,0,"Hola");

			assert_tabla_paginas_item(list_get(tabla_paginas,0),1,0,1,true,true);
			assert_tabla_paginas_item(list_get(tabla_paginas,1),1,1,1,false,false);

		}end

		it("ejecuta mem.cod") {

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configAlgoritmoTestFifo");
			iniciar_marcos();
			/*
			 * iniciar 6;
			 escribir 2 "dos";
			 leer 2;
			 escribir 3 "tres";
			 leer 2;
			 escribir 1 "uno"
			 leer 1;
			 escribir 5 "cinco";
			 leer 2;
			 escribir 4 "cuatro";
			 leer 5;
			 leer 3;
			 escribir 2 "dos";
			 leer 5;
			 leer 2;
			 */
			iniciar_n(1,6);

			escribir_n(1,1,"Dos");
			leer_n(1,1);

			escribir_n(1,2,"Tres");
			leer_n(1,1);

			escribir_n(1,0,"Uno");
			leer_n(1,0);

			escribir_n(1,4,"Cinco");
			leer_n(1,1);

			escribir_n(1,3,"Cuatro");
			leer_n(1,4);
			leer_n(1,2);

			escribir_n(1,1,"Dos");
			leer_n(1,4);
			leer_n(1,1);

			should_int(page_faults) be equal to(9);
			should_int(accesos_swap) be equal to(15);

			assert_tabla_paginas_item(list_get(tabla_paginas,1),1,1,2,false,true);
			assert_tabla_paginas_item(list_get(tabla_paginas,2),1,2,0,false,true);
			assert_tabla_paginas_item(list_get(tabla_paginas,4),1,4,1,false,true);

		}end

	}end

	describe("lru") {

		before {

			remove("/tp-2015-2c-signiorcodigo/memoria/test/logTest");

			logger = log_create("/tp-2015-2c-signiorcodigo/memoria/test/logTest",
					"Test", false, LOG_LEVEL_DEBUG);

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionLru");
			iniciar_marcos();

		}end

		it("swappea usando lru") {

			iniciar_n(1,4);

			t_item * item = list_get(tabla_paginas,0);

			should_int(item->numero_operacion) be equal to(0);

			leer_n(1,0);
			should_int(item->numero_operacion) be equal to(1);

			leer_n(1,0);
			leer_n(1,0);
			should_int(item->numero_operacion) be equal to(3);

			escribir_n(1,0,"Hola");
			should_int(item->numero_operacion) be equal to(4);

			leer_n(1,1);

			item = list_get(tabla_paginas,1);
			should_int(item->numero_operacion) be equal to(5);

			leer_n(1,2);

			leer_n(1,3);

			item = list_get(tabla_paginas,3);
			should_int(item->numero_operacion) be equal to(7);

			assert_tabla_paginas_item(list_get(tabla_paginas,0),1,0,0,false,false);

		}end

		it("ejecuta mem.cod") {

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configAlgoritmoTestLRU");
			iniciar_marcos();
			/*
			 * iniciar 6;
			 escribir 2 "dos";
			 leer 2;
			 escribir 3 "tres";
			 leer 2;
			 escribir 1 "uno"
			 leer 1;
			 escribir 5 "cinco";
			 leer 2;
			 escribir 4 "cuatro";
			 leer 5;
			 leer 3;
			 escribir 2 "dos";
			 leer 5;
			 leer 2;
			 */
			iniciar_n(1,6);

			escribir_n(1,1,"Dos");
			leer_n(1,1);

			escribir_n(1,2,"Tres");
			leer_n(1,1);

			escribir_n(1,0,"Uno");
			leer_n(1,0);

			escribir_n(1,4,"Cinco");
			leer_n(1,1);

			escribir_n(1,3,"Cuatro");
			leer_n(1,4);
			leer_n(1,2);

			escribir_n(1,1,"Dos");
			leer_n(1,4);
			leer_n(1,1);

			should_int(page_faults) be equal to(7);
			should_int(accesos_swap) be equal to(11);

			assert_tabla_paginas_item(list_get(tabla_paginas,1),1,1,2,true,true);
			assert_tabla_paginas_item(list_get(tabla_paginas,2),1,2,0,false,true);
			assert_tabla_paginas_item(list_get(tabla_paginas,4),1,4,1,true,true);

		}end

	}end

	describe("clock_m") {

		before {

			remove("/tp-2015-2c-signiorcodigo/memoria/test/logTest");

			logger = log_create("/tp-2015-2c-signiorcodigo/memoria/test/logTest",
					"Test", false, LOG_LEVEL_DEBUG);

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configuracionClock-M");
			iniciar_marcos();

		}end

		it("ejecuta mem.cod") {

			memoriaConfig= config_create("/tp-2015-2c-signiorcodigo/memoria/test/configAlgoritmoClockM");
			iniciar_marcos();

			/*
			 * iniciar 6;
			 escribir 2 "dos";
			 leer 2;
			 escribir 3 "tres";
			 leer 2;
			 escribir 1 "uno"
			 leer 1;
			 escribir 5 "cinco";
			 leer 2;
			 escribir 4 "cuatro";
			 leer 5;
			 leer 3;
			 escribir 2 "dos";
			 leer 5;
			 leer 2;
			 */

			iniciar_n(1,6);

			escribir_n(1,1,"Dos");
			leer_n(1,1);

			escribir_n(1,2,"Tres");
			leer_n(1,1);

			escribir_n(1,0,"Uno");
			leer_n(1,0);

			escribir_n(1,4,"Cinco");
			leer_n(1,1);

			escribir_n(1,3,"Cuatro");
			leer_n(1,4);
			leer_n(1,2);

			escribir_n(1,1,"Dos");
			leer_n(1,4);
			leer_n(1,1);

			should_int(page_faults) be equal to(8);
			should_int(accesos_swap) be equal to(12);

			t_item * item = list_get(tabla_paginas,1);

			should_bool(item->modificado) be equal to(true);
			should_bool(item->uso) be equal to(true);

			item = list_get(tabla_paginas,2);
			should_bool(item->modificado) be equal to(false);
			should_bool(item->uso) be equal to(true);

			item = list_get(tabla_paginas,4);
			should_bool(item->modificado) be equal to(true);
			should_bool(item->uso) be equal to(true);

		}end

	}end

}
