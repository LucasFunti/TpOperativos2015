/*
 * Loggear.c
 *
 *  Created on: 3/11/2015
 *      Author: utnso
 */

#include "loggear.h"

void loggearWarning(char * info) {

	log_warning(logger, info);
}

void loggearError(char * info) {

	log_error(logger, info);
}

void loggearInfo(char * info) {

	log_info(logger, info);
}

void iniciarLogger() {

//Si existe el archivo lo elimina
	remove("logMemoria");

	logger = log_create("logMemoria", "PAM", true, LOG_LEVEL_DEBUG);
	loggearInfo("Iniciado el proceso administrador de memoria");

}

