#include "commonLib.h"
#include "twitter.h"

int getTwitterResponse(int sd, int pipefd2){
	
	char respuesta[1024];
	int leido2;

	const char fin[5] = "fin\n";

	memset (respuesta, 0, sizeof respuesta);
						

	while ((leido2 = read (sd, respuesta, sizeof respuesta)) >0){
		

		if (write(pipefd2, respuesta, leido2) <0){
			perror ("llamada write");
			return -1;
		}
	
		
	}

	write(pipefd2, fin, strlen(fin));

	return 0;
	
}

int getTimeline(int sd, int pipefd2){

	int leido;
	/*para leer el timeline*/
	char timeline[1024];
	/*delimiter*/
	const char fin[5] = "fin\n";
   	

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

int verifyTweetLength(char line_original[150]){

	int i;
	//para que no cuente el comando y el espacio ("tweet ")
	int charCount = -6;
	
	for (i = 0; line_original[i]; i++){
		if (line_original[i] != '\0' && line_original[i] != '\n'){
			charCount++;
		}
	}

	if (charCount > 140){
		return 1;
	}else{
		return 0;
	}
	

	

}

int showTimeline (int pipefd2){

	int leido;
	/*para leer el timeline*/
	char timeline[1024];
	char *ret_val;


	while ((leido = read(pipefd2, timeline, sizeof timeline)) > 0){

		/*tengo que indicarle hasta donde leer*/
		ret_val = strstr(timeline, "fin\n");

		if (ret_val){
			if(write (1, timeline, leido - 4) < 0){
				perror ("llamada write");
				return -1;
			}
			memset(timeline, 0, sizeof timeline);
			break;
		}
		if(write (1, timeline, leido) < 0){
			perror ("llamada write");
			return -1;
		}
		

    	
	}
	return 0;
}

int saveTimeline(int sd){

	int leido;
	/*para leer el timeline*/
	char timeline[1024];
	int descriptorArchivo;
	char *path = "./salida.txt";
   	/*vacio el timeline*/
   	memset (timeline, 0, sizeof timeline);


   	unlink (path);
	//open devuelve un nro positivo si lo puede abrir y negativo, si no puede. uso O_APPEND para q no sobre escriba
	if ((descriptorArchivo = open ("./salida.txt", O_CREAT | O_RDWR | O_APPEND, S_IRWXU)) <0){ 
    	perror ("error en open savetl");
    	return errno;
    }

   	/*leo del socket y escribo el timeline en el archivo*/
   	while (	(leido = read (sd, timeline, sizeof timeline)) > 0){ 

		if (write(descriptorArchivo, timeline, leido) <0){
			perror ("llamada write");
			return -1;
		}
		
	}
	close(descriptorArchivo);

	return 0;
}

int searchWord(char *palabraBuscada, int pipefd2){
	int leido;
	char timeline[1024];
	int descriptorArchivo;
	int countWords = 0;
	char *ptr;
	char palabra[20];
	char *token;
	char tmp[20];
	char *rest;
	
		
	/*delimiter*/
	const char fin[5] = "fin\n";
	
	

	if ((descriptorArchivo = open ("./salida.txt", O_RDONLY)) < 0){ 
    	perror ("error en open srchwrd ");
    	return errno;
    }

    memset(palabra, 0, sizeof palabra);
    memset(tmp, 0, sizeof tmp);
    memset(timeline, 0, sizeof timeline);

   	//hago strcpy para poder calcular el strlen cuando uso strncmp
   	strncpy(palabra, palabraBuscada, sizeof palabra);
   	printf ("la palabra buscada es %s\n", palabra);

	while (	(leido = read (descriptorArchivo, timeline, sizeof timeline)) > 0){ 
		//ptr apunta a timeline. Hago una copia para no perdero con el strtok
		ptr  = timeline;
		printf ("Imprimo timeline palabra por palabra: \n");
		while ((token = strtok_r (ptr, " \r\n\t.!?¿-_,;", &rest))) {
			
			printf ("%s ", tmp);
		
			if (strncmp(token, palabra, strlen(palabra)) == 0){	
				
				countWords++;
			}
			ptr = NULL;
		}
		memset (timeline, 0, sizeof timeline);
		
	}	
	if (countWords > 0) {
        printf ("\nThe word was found %d times \n", countWords);
    } else {
        printf ("Word not found\n");
    }

	close (descriptorArchivo);


	write(pipefd2, fin, strlen(fin));

	

	return 0;
}