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

	//write (1, "las letras totales del tw son: ", 32);
	//printf ("%d\n", charCount);

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


			if(write (1, timeline, leido) < 0)
{				perror ("llamada write");
			return -1;
			}
			/*tengo que indicarle hasta donde leer*/
			ret_val = strstr(timeline, "fin\n");

			if (ret_val){
				memset(timeline, 0, sizeof timeline);
				break;
			}

	    	
		}
		return 0;
}

int saveTimeline(int sd){

	int leido;
	/*para leer el timeline*/
	char timeline[1024];
	int descriptorArchivo;

   	/*vacio el timeline*/
   	memset (timeline, 0, sizeof timeline);


   	remove ("./salida.txt");
	//open devuelve un nro positivo si lo puede abrir y negativo, si no puede
	if ((descriptorArchivo = open ("./salida.txt", O_CREAT | O_RDWR, S_IRWXU)) <0){ 
    	perror ("error en open");
    	return errno;
    }

   	/*leo del socket y escribo el timeline en el archivo*/
   	while (	(leido = read (sd, timeline, sizeof timeline)) > 0){ 

		if (write(descriptorArchivo, timeline, leido) <0){
			perror ("llamada write");
			return -1;
		}
		
	}
	

	return 0;
}

int searchWord(char palabraBuscada[20]){
	int leido;
	char timeline[1024];
	int descriptorArchivo;

	//write (1, "srchword ", 10);
	//write (1, palabraBuscada, sizeof palabraBuscada);

	if ((descriptorArchivo = open ("./salida.txt", O_RDWR, S_IRWXU)) <0){ 
    	perror ("error en open");
    	return errno;
    }
		
	while (	(leido = read (descriptorArchivo, timeline, sizeof timeline)) > 0){ 

		if(strstr(timeline, palabraBuscada)){
			write (1,"la encontre", 12);
		}
	}	


	//}

	return 0;
}