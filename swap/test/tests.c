#include <cspecs/cspec.h>
#include <commons/collections/list.h>

context(pruebas_swap) {

	setSwapConfig("/tp-2015-2c-signiorcodigo/swap/test/swapConfigTest");

	describe("reservar memoria") {

		before {

			setupSwap();
			setPages();

			//64 Paginas de tamanio 50

		}end

		it("reserva lugar para un proceso") {

			should_int(reserve(0,10)) be equal to(0);

			should_int(getProcessReservedSpace(0)) be equal to(10);
			should_int(getBlankPages()) be equal to(54);

		}end

		it("reserva lugar para varios procesos") {

			reserve(0,10);
			reserve(1,30);
			reserve(2,15);

			should_int(getProcessReservedSpace(0)) be equal to(10);
			should_int(getProcessReservedSpace(1)) be equal to(30);
			should_int(getProcessReservedSpace(2)) be equal to(15);

			should_int(getBlankPages()) be equal to(9);

		}end

		it("no reserva lugar porque no tiene espacio suficiente") {

			reserve(0,62);
			should_int(reserve(1,10)) be equal to(-1);

			should_int(getProcessReservedSpace(0)) be equal to(62);
			should_int(getProcessReservedSpace(1)) be equal to(0);

			should_int(getBlankPages()) be equal to(2);

		}end

	}end

	describe("eliminar un proceso") {

		before {

			setupSwap();
			setPages();

			//64 Paginas de tamanio 50

		}end

		it("lo elimina de sus índices") {

			reserve(0,10);
			reserve(1,10);

			freeSpace(0);

			should_int(getProcessReservedSpace(0)) be equal to(0);
			should_int(getProcessReservedSpace(1)) be equal to(10);

			should_int(getBlankPages()) be equal to(54);

			freeSpace(1);

			should_int(getProcessReservedSpace(0)) be equal to(0);
			should_int(getProcessReservedSpace(1)) be equal to(0);

			should_int(getBlankPages()) be equal to(64);

		}end
	}end

	describe("leer datos") {

		before {

			setupSwap();
			setPages();
			//64 Paginas de tamanio 50

		}end

		it("Lee paginas") {

			reserve(0,10);
			reserve(1,10);

			writePage(0,"Hola!");
			writePage(1,"Chau!");

			writePage(10,"Hola!");

			should_string(readProcessPage(0,0)) be equal to("Hola!");
			should_string(readProcessPage(0,1)) be equal to("Chau!");
			should_string(readProcessPage(1,0)) be equal to("Hola!");

		}end

	}end

	describe("escribir datos") {

		before {

			setupSwap();
			setPages();
			//64 Paginas de tamanio 50

		}end

		it("Escribe paginas") {

			reserve(0,10);
			reserve(1,10);

			writeProcessPage(0,0,"Hola!");
			writeProcessPage(0,1,"Chau!");
			writeProcessPage(0,2,"Hola2!");

			writeProcessPage(1,2,"JoJoJo!");

			should_string(readProcessPage(0,0)) be equal to("Hola!");
			should_string(readProcessPage(0,1)) be equal to("Chau!");
			should_string(readProcessPage(0,2)) be equal to("Hola2!");

			should_string(readProcessPage(1,2)) be equal to("JoJoJo!");

		}end

	}end

	describe("Algoritmo de compactación") {

		before {

			setupSwap();
			setPages();
			//64 Paginas de tamanio 50

		}end

		it("compacta los huecos") {

			reserve(0,10);
			reserve(1,10);
			reserve(2,10);
			reserve(3,10);
			reserve(4,10);
			reserve(5,10);

			should_int(getLargestContiguousSpace()) be equal to(4);

			freeSpace(1);

			should_int(getLargestContiguousSpace()) be equal to(10);

			freeSpace(3);

			writeProcessPage(0,0,"Hola!");

			writeProcessPage(2,5,"Hola!");

			writeProcessPage(4,9,"Hola!");

			freeSpace(5);

			should_int(getLargestContiguousSpace()) be equal to(14);

			compact();

			should_int(getLargestContiguousSpace()) be equal to(34);

			should_string(readProcessPage(0,0)) be equal to("Hola!");
			should_string(readProcessPage(2,5)) be equal to("Hola!");
			should_string(readProcessPage(4,9)) be equal to("Hola!");

		}end

	}end
}
