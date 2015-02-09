#include "hijo.h"
#include "commonLib.h"
#include "twitter.h"

int childService(int pipefd, int pipefd2){

	int leido;
	/*variables para parsear linea de comandos*/
	char line[150];
	char line_original[150];
	char comando[15];
	/*apunta al resto del comando luego de extraer el token*/
	char *rest;
	/*apunta al token actual*/
	char *token;	
	char *ptr;
	int len;
	int leido2;
	const char fin[5] = "fin\n";
	char respuesta[1024];
	/*para leer el timeline*/
	char *ret_val;


	leido = read (STDIN_FILENO, line, sizeof line); 
	/*lleno de ceros el arreglo "comando"*/
	memset (comando, 0, sizeof comando);
	memset (line_original, 0, sizeof line_original);
	strncpy (line_original, line, 150);
	/*ptr apunta al inicio de line. Hago una "copia" para no perderlo (strtok_r me hace perderlo)*/
	ptr = line;

	/*parser*/
	while(token = strtok_r(ptr, " ", &rest)) {
		
		if (strncmp(token, "timeline", 8) == 0){
			/*copio el comando al arreglo comando para escribir en el pipe*/
			
			strncpy(comando, token, 8);
			
			if (write(pipefd, comando, sizeof comando) <0 ){
				perror ("llamada write");
				return -1;
			}

			/*Leo el timeline que me devuelve el padre*/
			showTimeline(pipefd2);


		
		}else if(strncmp(token, "tweet", 5) == 0){
				    		
			/*corroboro la longitud del tweet*/
			len = verifyTweetLength(line_original);

			if (len == 1){
				write (1, "El tweet excede los 140 caracteres\n", 35);
				break;
			}

			strncpy(comando, token, 5);
			//ptr va a tener el resto de la cadena. Puede ser un tweet, un usuario, etc.
			ptr = rest;
			

			/*envio el comando en el pipe*/
			
			if (write(pipefd, comando, sizeof comando) <0 ){
				perror ("llamada write");
				return -1;
			}


			/*escribo lo q escribio el usuario en el pipe*/


			if (write(pipefd, line_original, leido) <0 ){
				perror ("llamada write");
				return -1;
			}
			/*delimitador para que seapa hasta adonde leer*/
			if (write(pipefd, fin, 4) <0 ){
				perror ("llamada write");
				return -1;
			}
					    				
	
			/*leo lo q me contesta el padre*/
			while ((leido2 = read(pipefd2, respuesta, sizeof respuesta)) > 0){
				
				if(write (1, respuesta, leido2) < 0){
    				perror ("llamada write");
    				return -1;
				}


				/*tengo que indicarle hasta adonde leer*/
				ret_val = strstr(respuesta, fin);
		
				if (ret_val){
					memset(respuesta, 0, sizeof respuesta);
					break;
				}
				
		    	close (pipefd2);	
			}	
			
			}else if(strncmp(token, "user", 4) == 0){
				
				strncpy(comando, token, 4);
				//ptr va a tener el resto de la cadena. Puede ser un tweet, un usuario, etc.
				ptr = rest;

				/*envio el comando en el pipe*/
			
				if (write(pipefd, comando, sizeof comando) <0 ){
					perror ("llamada write");
					return -1;
				}

				/*envio comando y usuario al padre*/
				if (write(pipefd, line_original, leido) <0 ){
					perror ("llamada write");
					return -1;
				}
				

				/*delimitador para que seapa hasta adonde leer*/
				
				if (write(pipefd, fin, 4) <0 ){
					perror ("llamada write");
					return -1;
				}
					
				/*leo el timeline del usuario especificado*/


				showTimeline(pipefd2);


			}else if(strncmp(token, "search", 6) == 0){

				strncpy(comando, token, 6);
				//ptr va a tener el resto de la cadena. 
				ptr = rest;

				/*envio el comando en el pipe*/
			
				if (write(pipefd, comando, sizeof comando) <0 ){
					perror ("llamada write");
					return -1;
				}

				/*envio comando, usuario y palabra a buscar al padre*/
				if (write(pipefd, line_original, leido) <0 ){
					perror ("llamada write");
					return -1;
				}

				/*delimitador para que seapa hasta adonde leer*/
				
				if (write(pipefd, fin, 4) <0 ){
					perror ("llamada write");
					return -1;
				}



			}else{
		
    			if (write (1, "no es un comando valido\n", 25) <0){
    				perror ("llamada write");
    				return -1;
    			}



			break;
		}


		//ptr va a tener el resto de la cadena. Puede ser un tweet, un usuario, etc.
		ptr = rest; // rest contains the left over part..assign it to ptr...and start tokenizing again.    
			
			break;
    }		
	memset (line, 0, sizeof line);
	token = NULL;
	return 0;

}