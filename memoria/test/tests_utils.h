/*
 * tests_utils.h
 *
 *  Created on: 1/11/2015
 *      Author: utnso
 */

#ifndef TESTS_UTILS_H_
#define TESTS_UTILS_H_

#include <cspecs/cspec.h>
#include "../src/pedidos.h"

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

void assert_tabla_paginas_item(t_tabla_paginas_item * fila_tabla_paginas,
		int pid, int pagina, int marco) {

	should_ptr(fila_tabla_paginas) not be null;
	should_int(fila_tabla_paginas->pid) be equal to(pid);
	should_int(marco_para_pagina(fila_tabla_paginas,pagina))be equal to(marco);
}

void assert_tabla_paginas_match(int pagina, int marco) {

	should_int(tabla_paginas_match->pagina) be equal to(pagina);
	should_int(tabla_paginas_match->marco) be equal to(marco);
}

#endif /* TESTS_UTILS_H_ */
