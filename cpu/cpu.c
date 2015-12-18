/*
 * cpu.c *
 */

#include "cpu.h"

#define BACKLOG 5
#define PACKAGESIZE 32

int porcentajeDeUso[50], instruccionesEjecutadas[50];

int main(int argc, char **argv) {

// Defino la función que va a manejar los contadores

	void iniciarFlags(){
		int i;
		for (i = 0; i <= 50; i++) {
			hayQueFinalizar[i] = false;
		}
	}

	void resetearContadores() {
		int i;
		for (i = 0; i <= 50; i++) {
			porcentajeDeUso[i] = 0;
			instruccionesEjecutadas[i] = 0;
		}
	}

	void funcionResetearContadores() {
		while (1) {
			sleep(60);
			resetearContadores();
		}
	}

// Inicio las variables iniciales
	int i;
	int cantHilos = getHilos();
	resetearContadores();
	pthread_t hilos[cantHilos], hiloContador;
	iniciarFlags();

	remove("/tp-2015-2c-signiorcodigo/cpu/log_cpu");
	t_log *log_cpu = log_create("/tp-2015-2c-signiorcodigo/cpu/log_cpu", "CPU",
			true, LOG_LEVEL_INFO);
	t_hilo *infoHilo;

// Creo el hilo que gestiona el contador
	pthread_create(&hiloContador, NULL, funcionResetearContadores, NULL);

// Creo las instancias de CPU
	for (i = 0; i < cantHilos; i++) {
		infoHilo = malloc(sizeof(t_hilo));
		infoHilo->idHilo = i;
		infoHilo->logger = log_cpu;
		pthread_create(&hilos[i], NULL, iniciarcpu, infoHilo);
	};
	for (i = 0; i < cantHilos; i++) {
		pthread_join(hilos[i], NULL);
	};
	pthread_join(hiloContador, NULL);

	/*	printf("ingrese la accion a testear, o escriba 'ayuda' para ver comandos disponibles:\n");
	 char *accion = malloc(sizeof(char) * 32);
	 scanf("%[^\n]%*c", accion);
	 while(strcmp(accion,"exit")!=0){
	 testCpuFunction(accion);
	 printf("ingrese la accion a testear, o escriba 'ayuda' para ver comandos disponibles:\n");
	 scanf("%[^\n]%*c", accion);

	 }; */

	return 0;
}
