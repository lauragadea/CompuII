#include "hijo.h"
#include "commonLib.h"
#include "twitter.h"


void *history(void *threadpipe){
	
    int pipelectura = (intptr_t)threadpipe;

	int desc;
	size_t len;
	int leido;

	if ((desc = open ("./history.txt", O_CREAT | O_WRONLY | O_APPEND, S_IRWXU)) <0){ 
	    	perror ("error en open history");
	    	return NULL;
	}

	while(1){
		char newline[150];
		
		while ((leido = read(pipelectura, newline, sizeof newline)) > 0){
			//strncpy(line, newline, sizeof line);
			len = strlen(newline);
		    write (desc, newline, len);
		}

	}
	
	close (desc);
	pthread_exit(NULL);
}


int childService(int pipefd, int pipefd2, int ejec, int threadpipe[2]){

	int leido;
	/*variables para parsear linea de comandos*/
	char line[150];
		
	memset (line, 0, sizeof line);
			
	char line_original[150];
	char comando[15];
	/*apunta al resto del comando luego de extraer el token*/
	char *rest;
	/*apunta al token actual*/
	char *token;	
	char *ptr;
	int len;
	int leido2;
	pthread_t thread_id;
	const char fin[5] = "fin\n";
	char respuesta[1024];

	/*para leer el timeline*/
	char *ret_val;
	

	leido = read (STDIN_FILENO, line, sizeof line); 
	
	memset (comando, 0, sizeof comando);
	
	memset (line_original, 0, sizeof line_original);
	strncpy (line_original, line, 150);

	//crea el hilo solo la primera vez
	if (ejec < 1){
		
		pthread_create (&thread_id, NULL, history, (void *)(intptr_t)threadpipe[0]);	
		if (write(threadpipe[1], line, sizeof line) <0 ){
				perror ("llamada write");
				return -1;
		}
	}
	//lo que se ejecuta el resto de las veces
	if (ejec > 0){
		

		if (write(threadpipe[1], line, sizeof line) <0 ){
				perror ("llamada write");
				return -1;
		}

		
	}
	


	/*ptr apunta al inicio de line.*/
	ptr = line;

	/*parser*/
	while ((token = strtok_r(ptr, " ", &rest))) {
		
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
				write (1, "The tweet exceeds 140 characters\n", 35);
				break;
			}

			strncpy(comando, token, 5);
			//ptr va a tener el resto de la cadena.
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
				
				/*tengo que indicarle hasta adonde leer*/
				ret_val = strstr(respuesta, fin);

				if (ret_val){

					if(write (1, respuesta, leido2 - 4) < 0){
    					perror ("llamada write");
    					return -1;
					}

					memset(respuesta, 0, sizeof respuesta);
					break;
				}

				if(write (1, respuesta, leido2) < 0){
    				perror ("llamada write");
    				return -1;
				}

				
		    	close (pipefd2);	
			}	
			
			}else if(strncmp(token, "user", 4) == 0){
				
				strncpy(comando, token, 4);
				//ptr va a tener el resto de la cadena.
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

				/*envio al padre comando, usuario y palabra a buscar*/
				if (write(pipefd, line_original, leido) <0 ){
					perror ("llamada write");
					return -1;
				}

				/*delimitador para que sepa hasta adonde leer*/
				
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



			}else if (strncmp(token, "exit", 4) == 0){

				strncpy(comando, token, 4);
			
				if (write(pipefd, comando, sizeof comando) <0 ){
					perror ("llamada write");
					return -1;
				}
				exit(0);

			}else{
		
    			if (write (1, "no es un comando valido\n", 25) <0){
    				perror ("llamada write");
    				return -1;
    			}



			break;
		}


		//ptr va a tener el resto de la cadena. 
		ptr = rest; // rest contains the left over part..assign it to ptr...and start tokenizing again.    
			
			break;
    }
   
	token = NULL;
	
	return 0;

}