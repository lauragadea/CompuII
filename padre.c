#include "estructura.h"
#include "padre.h"
#include "commonLib.h"



connectionData parseo (char conf[20]){

	connectionData d;
	d.direc[0] = 0;
	d.port = 0;

	int sd_conf;
	char buffer_conf[256] = {'\0'};
	//char *token = NULL;
	char *progreso_linea;
	char *linea;
	char linea_conf [32] = {'\0'};
	
	char *dirchar = NULL;	

	char *rest;

	/* Abrimos y leemos el archivo de configuración */
	sd_conf = open(conf, O_RDWR);

	if (sd_conf == -1){
		perror ("Error abriendo el archivo de configuración:");
	}

	read (sd_conf, buffer_conf, sizeof buffer_conf);

	/* Parseamos el archivo de configuración */
	/*divido el buffer_conf en dos lineas*/
	linea = strtok_r(buffer_conf, "\n", &progreso_linea);

	while (linea != NULL){
		
		strcpy(linea_conf, linea);

		if (strstr(linea_conf, "dir") != NULL) {
		
			strtok_r(linea, "=", &rest);
			dirchar = strtok_r(NULL, "=", &rest);
			strncpy(d.direc, dirchar, strlen(dirchar)+1);
		}
		
		if (strstr (linea_conf, "port") != NULL) {

			strtok_r(linea, "=", &progreso_linea);
			dirchar = strtok_r(NULL, "=", &progreso_linea);
		
			d.port = atoi(dirchar);
			
		}

		linea = strtok_r(NULL, "\n", &progreso_linea);
		memset (linea_conf, 0, sizeof linea_conf);
	}
	 
	
	return d;

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

		}else if((strncmp(comando, "exit", 4)) == 0){

			c = 5;

		}
		return c;
}
