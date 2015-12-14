#include "tests_utils.h"

context(test_admin_memoria) {

	describe("iniciar_n") {

		t_config * configuracion;

		before {

			tabla_paginas = list_create();

		}end

		describe("sin tlb") {

			before {

				configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionSinTlb");
				iniciar_marcos(configuracion);

			}end

			it("agrega una entrada a la tabla de páginas") {

				should_bool(iniciar_n(1,2,configuracion,true)) be equal to (true);

				should_int(list_size(tabla_paginas)) be equal to(2);

				assert_tabla_paginas_item(list_get(tabla_paginas,0),1,1,0,false);
				assert_tabla_paginas_item(list_get(tabla_paginas,1),1,2,1,false);

			}end

			it("no puede registrar el proceso por exceso de páginas") {

				should_bool(iniciar_n(1,5,configuracion,true)) be equal to (false);
				should_int(list_size(tabla_paginas)) be equal to(0);

			}end

			it("agrega una entrada a la tabla de páginas swapeando") {

				should_bool(iniciar_n(1,3,configuracion,true)) be equal to (true);

				should_int(list_size(tabla_paginas)) be equal to(2);
				assert_tabla_paginas_item(list_get(tabla_paginas,0),1,2,1,false);
				assert_tabla_paginas_item(list_get(tabla_paginas,1),1,3,0,false);

			}end

		}end

		describe("con tlb") {

			before {

				configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
				iniciar_marcos(configuracion);
				tlb = list_create();

			}end

			it("agrega una entrada a la tabla de páginas swapeando intentando remover la entrada swapeada de la tlb") {

				should_bool(iniciar_n(1,3,configuracion,true)) be equal to (true);

				should_int(list_size(tabla_paginas)) be equal to(2);
				assert_tabla_paginas_item(list_get(tabla_paginas,0),1,2,1,false);
				assert_tabla_paginas_item(list_get(tabla_paginas,1),1,3,0,false);

			}end

		}end

	}end

	describe("escribir_n") {

		t_config * configuracion;

		before {

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionSinTlb");
			iniciar_marcos(configuracion);
			tabla_paginas = list_create();

		}end

		it("escribe estando en memoria") {

			iniciar_n(1,2,configuracion,true);

			escribir_n(1,1,"Hola",configuracion);
			escribir_n(1,2,"Chau",configuracion);

			int tamanio_marco = config_get_int_value(configuracion, "TAMANIO_MARCO");

			char * pagina_1 = malloc(tamanio_marco);
			char * pagina_2 = malloc(tamanio_marco);

			memcpy(pagina_1,memoria,tamanio_marco);
			memcpy(pagina_2,memoria+tamanio_marco,tamanio_marco);

			should_string((char *)pagina_1) be equal to("Hola");
			should_string((char *)pagina_2) be equal to("Chau");

		}end

		it("escribe estando en memoria, usando leer_n") {

			iniciar_n(1,2,configuracion,true);

			escribir_n(1,1,"Hola",configuracion);
			escribir_n(1,2,"Chau",configuracion);

			should_string(leer_n(1,1,configuracion)) be equal to("Hola");
			should_string(leer_n(1,2,configuracion)) be equal to("Chau");

		}end

		it("escribe estando en la tlb") {

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos(configuracion);

			iniciar_n(1,2,configuracion,true);

			escribir_n(1,1,"Hola",configuracion);
			escribir_n(1,2,"Chau",configuracion);

			should_int(list_size(tlb)) be equal to(2);
			should_string(leer_n(1,1,configuracion)) be equal to("Hola");
			should_string(leer_n(1,2,configuracion)) be equal to("Chau");

		}end

	}end

	describe("leer_n") {

		t_config * configuracion;

		before {

			tabla_paginas = list_create();
			tlb = list_create();

		}end

		it("lee estando en memoria") {

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionSinTlb");
			iniciar_marcos(configuracion);

			iniciar_n(1,2,configuracion,true);

			escribir_n(1,1,"Hola",configuracion);
			escribir_n(1,2,"Chau",configuracion);

			should_string(leer_n(1,1,configuracion)) be equal to("Hola");
			should_string(leer_n(1,2,configuracion)) be equal to("Chau");

		}end

		it("lee estando en tlb") {

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos(configuracion);

			iniciar_n(1,2,configuracion,true);

			escribir_n(1,1,"Hola",configuracion);
			escribir_n(1,2,"Chau",configuracion);

			should_string(leer_n(1,1,configuracion)) be equal to("Hola");
			should_string(leer_n(1,2,configuracion)) be equal to("Chau");

		}end

	}end

	describe("finalizar") {

		t_config * configuracion;

		it("remueve procesos de la tlb y la tabla liberando marcos") {

			tabla_paginas = list_create();
			tlb = list_create();

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos(configuracion);

			iniciar_n(1,1,configuracion,true);
			iniciar_n(2,1,configuracion,true);

			tlb_agregar_entrada(1,1,0,configuracion,false);
			tlb_agregar_entrada(2,1,1,configuracion,false);

			finalizar(1,configuracion,true);

			should_int(list_size(tlb)) be equal to(1);
			should_int(list_size(tabla_paginas)) be equal to(1);

		}end

		it("con un pid que no existe no remueve nada de ningún lado") {

			tabla_paginas = list_create();
			tlb = list_create();

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos(configuracion);

			iniciar_n(1,2,configuracion,true);

			tlb_agregar_entrada(1,1,0,configuracion,false);
			tlb_agregar_entrada(1,2,1,configuracion,false);

			finalizar(2,configuracion,true);

			should_int(list_size(tlb)) be equal to(2);
			should_int(list_size(tabla_paginas)) be equal to(2);

		}end

	}end

	describe("señales") {

		t_config * configuracion;

		it("vuelca el contenido de la memoria en un log") {

			tabla_paginas = list_create();
			tlb = list_create();

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos(configuracion);

			remove("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/log_test_volcado_memoria");

			logger = log_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/log_test_volcado_memoria", "PAM", true, LOG_LEVEL_DEBUG);
			loggearInfo("Inicio del test");

			iniciar_n(1,2,configuracion,true);

			escribir_n(1,1,"Hola",configuracion);
			escribir_n(1,2,"Chau",configuracion);

			volcar_memoria_principal(configuracion);
			loggearInfo("Fin del test");

			should_int(1) be equal to(1);

		}end

		it("vacia la tlb") {

			tabla_paginas = list_create();
			tlb = list_create();

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos(configuracion);

			iniciar_n(1,2,configuracion,true);

			escribir_n(1,1,"Hola",configuracion);
			escribir_n(1,2,"Chau",configuracion);

			vaciar_tlb(configuracion);

			should_int(list_size(tlb)) be equal to(0);
			should_int(list_size(tabla_paginas)) be equal to(2);
		}end

		it("vacia la tabla de paginas") {

			tabla_paginas = list_create();
			tlb = list_create();

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionConTlb");
			iniciar_marcos(configuracion);

			iniciar_n(1,2,configuracion,true);

			vaciar_tabla_paginas(configuracion);

			should_int(list_size(tlb)) be equal to(0);
			should_int(list_size(tabla_paginas)) be equal to(0);
		}end

	}end

	describe("fifo") {

		t_config * configuracion;

		before {

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionFifo");
			iniciar_marcos(configuracion);
			tabla_paginas = list_create();

		}end

		it("swappea paginas por fifo") {

			iniciar_n(1,5,configuracion,true);

			//Es el anteultimo (el primero con el que usa fifo)
			t_tlb_item * item = list_get(tabla_paginas,1);

			assert_tabla_paginas_item(item, 1, 4,0, false);

			//Es el ultimo (el segundo con el que usa fifo)
			item = list_get(tabla_paginas,2);

			assert_tabla_paginas_item(item, 1, 5,1, false);

		}end

	}end

	describe("lru") {
		t_config * configuracion;

		before {

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionLru");
			iniciar_marcos(configuracion);
			tabla_paginas = list_create();

		}end

		it("swappea usando lru") {

			//Tiene 3 entradas la tabla de páginas
			iniciar_n(1,3,configuracion,true);

			//Tendria que volar esta ya que fué la ultima utilizada
			leer_n(1,3,configuracion);
			leer_n(1,1,configuracion);
			leer_n(1,2,configuracion);

			//No tiene lugar y usa lru
			iniciar_n(2,1,configuracion,true);

			t_tlb_item * item = list_get(tabla_paginas,2);

			assert_tabla_paginas_item(item, 2, 1,2, false);

		}end

	}end

	describe("clock_m") {
		t_config * configuracion;

		before {

			configuracion= config_create("/home/utnso/Desarrollo/tp-2015-2c-signiorcodigo/memoria/test/configuracionClock-M");
			iniciar_marcos(configuracion);
			tabla_paginas = list_create();

		}end

		it("agrega una entrada a la tabla de páginas swapeando intentando remover la entrada swapeada de la tlb") {

		}end

	}end

}
