#include "padre.h"
#include "commonLib.h"

char* parseo (char conf[20]){
	
	int sd_conf;
	char buffer_conf[256] = {'\0'};
	char *token = NULL;
	char *progreso_linea;
	
	
	/* Abrimos y leemos el archivo de configuración */
	sd_conf = open(conf, O_RDWR);

	if (sd_conf == -1){
		perror ("Error abriendo el archivo de configuración:");
	}

	read (sd_conf, buffer_conf, sizeof buffer_conf);

	/* Parseamos el archivo de configuración */
	 
	token = strtok_r(buffer_conf, "=", &progreso_linea);
	token = strtok_r (NULL, "=", &progreso_linea);
	
	return token;
}





int checkCommand(char comando[15]){
		int c;

		if ((strncmp(comando, "timeline", 8)) == 0){
			c = 1;
				
		}else if ((strncmp(comando, "tweet", 5)) == 0){
			c = 2;

		}else if ((strncmp(comando, "user", 4)) == 0){

			c = 3;

		}else if((strncmp(comando, "search", 6)) == 0){

			c = 4;

		}
		return c;
}