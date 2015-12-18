#include <cspecs/cspec.h>
#include <commons/collections/list.h>

context(pruebas_swap) {

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

			should_char(readProcessPage(0,0)) be equal to("Hola!");
			should_char(readProcessPage(0,1)) be equal to("Chau!");
			should_char(readProcessPage(1,0)) be equal to("Hola!");

		}end

	}end
	/*

	 char* file_name = getSwapFileName();
	 int pagesAmount = getSwapPagesAmount();
	 char str[100];
	 strcpy(str, "/tp-2015-2c-signiorcodigo/swap/Debug/");
	 strcpy(str, file_name);
	 t_list *pages = setPages(pagesAmount);

	 describe("describe_1") {

	 it("delete_archivo_swap") {
	 remove(str);
	 int result = doesFileExist(str);
	 should_int(result) be equal to(0);
	 }end

	 }end

	 describe("describe_2") {

	 it("creacion_archivo_swap") {
	 remove(str);
	 setupSwap();
	 int result = doesFileExist(str);
	 should_int(result) be equal to(1);
	 }end

	 }end

	 describe("describe_3") {

	 it("reservar_paginas") {
	 reserve("prueba",10, pages);
	 should_int(getProcessReservedSpace("prueba",pages)) be equal to(10);
	 }end

	 }end

	 describe("describe_4") {

	 before {
	 puts("before 1");
	 freeSpace("prueba");
	 }end

	 it("reservar_sin_lugar") {
	 reserve("prueba",50, pages);
	 reserve("otro",5, pages);
	 should_int(getProcessReservedSpace("prueba",pages)) be equal to(50);
	 should_int(getProcessReservedSpace("otro",pages)) be equal to(5);
	 }end

	 }end

	 describe("describe_5") {

	 before {
	 puts("before 1");
	 freeSpace("prueba");
	 freeSpace("otro");
	 }end

	 it("liberar_paginas") {
	 reserve("prueba",60, pages);
	 should_int(getProcessReservedSpace("prueba",pages)) be equal to(60);
	 freeSpace("prueba",pages);
	 should_int(getProcessReservedSpace("prueba",pages)) be equal to(0);
	 }end

	 }end

	 describe("describe_6") {

	 it("escribir") {
	 writePage(0,"un texto");
	 should_string(readPage(0)) be equal to("un texto");
	 writePage(22,"otro texto");
	 should_string(readPage(22)) be equal to("otro texto");
	 }end

	 }end

	 describe("describe_7") {

	 before {
	 puts("before 1");
	 freeSpace("prueba");
	 freeSpace("otromas");
	 freeSpace("ultimo");
	 }end

	 it("cantidad_paginas_libres") {
	 reserve("prueba",14, pages);
	 reserve("otromas",15, pages);
	 reserve("ultimo",17, pages);
	 should_int(getBlankPages(pages)) be equal to(18);
	 }end

	 }end

	 describe("describe_8") {

	 before {
	 puts("before 1");
	 freeSpace("prueba");
	 freeSpace("otromas");
	 freeSpace("ultimo");
	 }end

	 it("compactar") {
	 reserve("prueba",14, pages);
	 should_int(getLargestContiguousSpace(pages)) be equal to (50);
	 reserve("otromas",15, pages);
	 should_int(getLargestContiguousSpace(pages)) be equal to (35);
	 reserve("ultimo",17, pages);
	 should_int(getLargestContiguousSpace(pages)) be equal to (18);
	 freeSpace("otromas",pages);
	 should_int(getLargestContiguousSpace(pages)) be equal to (18);
	 compact(pages);
	 should_int(getLargestContiguousSpace(pages)) be equal to (33);
	 }end

	 }end*/

}
