#include "funciones.h"
#include "padre.h"

int getTimeline(int sd, char comando[15], int pipefd2){

	int leido;
	/*para leer el timeline*/
	char timeline[1024];
	/*delimiter*/
	const char fin[5] = "fin\n";

	/*mando al servidor el comando*/
	if (write(sd, comando, strlen(comando)) < 0){
				perror ("llamada write");
   				return -1;

   	}
   	

   	/*vacio el timeline*/
   	memset (timeline, 0, sizeof timeline);

   	/*leo del socket y escribo el timeline en el pipe para que lea el hijo*/
   	while (	(leido = read (sd, timeline, sizeof timeline)) > 0){ 
		if (write(pipefd2, timeline, leido) <0){
			perror ("llamada write");
			return -1;
		}
		
	}
	
	write(pipefd2, fin, strlen(fin));
	return 0;
}