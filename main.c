#include "funciones.h"


int main (int argc, char *const argv[]){
	
	int num = 0;
	/*socket descriptor*/
	int sd;
	
	int len;
	//char *hostname = "api.twitter.com";
	/*socket descriptor totalmente conectado*/
	int sdtc;
	struct sockaddr_in server = {};
	
	/*buffer para ver que recibo*/
	char buffer[3000];
	memset (buffer, 0, sizeof buffer);
	/*variables para parsear linea de comandos*/
	char line[150];
	char line_original[150];
	/*apunta al resto del comando luego de extraer el token*/
	char *rest;
	/*apunta al token actual*/
	char *token;	
	char *ptr;
	

	/*return value of fork*/
	int ret=0;
	/*descriptores pipe (para que escriba el hijo)*/
	int fd[2];
	/*descriptores pipe2 (para que escriba el padre)*/
	int fd2[2];

	/*para leer el timeline*/
	char timeline[1024];
	char *ret_val;


	char envio[150];

	char comando[15];
	char tweet[140];
	int c=0;
	
	int leido;
	int leido2;
	int leido3;
	int leido4 = 0;
	char *message;


	//variables para buscar el token en el timeline
	
	const char fin[5] = "fin\n";



	char respuesta[1024];

	/*creo los pipes*/
	if ( pipe (fd) != 0 ){

			perror ("pipe");
			return -1;
	}

	if ( pipe (fd2) != 0 ){ 

			perror ("pipe 2");
			return -1;
	}


	//creo el socket
	sd = socket (PF_INET, SOCK_STREAM, 0);
	if (sd < 0){
		perror ("error en el socket: ");
		return -1;
	}
	

	memset (&server, 0, sizeof (server));
	server.sin_family = AF_INET;
	server.sin_port = htons(8001);
	 
	//esta es la direc q deberia cambiar si cambio de pc?
	inet_pton (AF_INET, "127.0.0.1", &server.sin_addr);


	if ((connect(sd, (struct sockaddr*)&server, sizeof (server))) < 0){
		
		perror ("error en connect 1: ");
		return -1;
	}
	

	/*returns the pid of the child*/
	ret = fork();
	/*interprete de comandos*/

	int m=0;	
	switch(ret){

		case -1:

			perror ("error en fork");
			sleep (1);

			return -1;

		/*hijo*/
		case 0:
			close (fd[0]);
			close (fd2[1]);

			while (1){
				/*pide un comando*/
			
				if (write (1, "Cliente>", 9) <0){
					perror ("llamada write");
					return -1;
				}

				leido4 = read (STDIN_FILENO, line, sizeof line); 
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
		    			
		    			if (write(fd[1], comando, sizeof comando) <0 ){
		    				perror ("llamada write");
		    				return -1;
		    			}

		    			/*Leo el timeline que me devuelve el padre*/
		    			readTimeline(fd2);


		
		    		
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
		    			
		    			if (write(fd[1], comando, sizeof comando) <0 ){
							perror ("llamada write");
							return -1;
						}


						/*escribo lo q escribio el usuario en el pipe*/


						if (write(fd[1], line_original, sizeof line_original) <0 ){
							perror ("llamada write");
							return -1;
						}
						/*delimitador para que seapa hasta adonde leer*/
						if (write(fd[1], fin, 4) <0 ){
							perror ("llamada write");
							return -1;
						}
		    					    				
		    	
		    			/*leo lo q me contesta el padre*/
		    			while ((leido3 = read(fd2[0], respuesta, sizeof respuesta)) > 0){

		    				if(write (1, respuesta, leido3) < 0){
			    				perror ("llamada write");
			    				return -1;
		    				}
		  

		    				/*tengo que indicarle hasta adonde leer*/
		    				ret_val = strstr(respuesta, fin);
					
		    				if (ret_val){
		    					memset(respuesta, 0, sizeof respuesta);
		    					break;
		    				}
							
		    		    	close (fd2[0]);	
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
		    	
				
			
		    	
			}
        	return 0;	

		/*padre*/
		default:
			/*cierra descriptores del pipe*/
			close(fd[1]);
			close (fd2[0]);
			
			
		
			while (1){
												
				leido = read (fd[0], comando, sizeof comando); 

				c = checkCommand(comando);
			
			

				switch(c){
			

					case 1:
						
						/*mando al servidor el comando*/
						if (write(sd, comando, strlen(comando)) < 0){
									perror ("llamada write");
				       				return -1;

					   	}
					   		
					   	/*vacio el timeline*/
					   	memset (timeline, 0, sizeof timeline);

					   	/*leo del socket y escribo el timeline en el pipe para que lea el hijo*/
					   	while (	(leido = read (sd, timeline, sizeof timeline)) > 0){ 
							if (write(fd2[1], timeline, leido) <0){
								perror ("llamada write");
								return -1;
							}
							
						}
						
						write(fd2[1], fin, strlen(fin));

						break;

					case 2:
						
					   	/*mando al servidor el comando y el tweet*/
						while ((leido = read(fd[0], line_original, sizeof line_original)) >0){
						
							//VER COMO RESOLVER. TWITTEA BASURA
							if(write (sd, line_original, leido - 3) <0){
								perror ("llamada write");
								return -1;
							}
							ret_val = strstr(line_original, fin);

							if (ret_val){
								break;
							}

						}

						
						/*leo respuesta del servidor y la escribo en el pipe*/
						 /*vacio el buf respuesta*/
					   	memset (respuesta, 0, sizeof respuesta);
						
					
						while ((leido2 = read (sd, respuesta, sizeof respuesta)) >0){
							
					
							
							if (write(fd2[1], respuesta, leido2) <0){
								perror ("llamada write");
								return -1;
							}
						

							
						}
						write(fd2[1], fin, strlen(fin));
						//close (fd2[1]);
						
						break;
					

					default:
						write (1, "No es un comando valido", 24);
						break;
				}

				close (sdtc);

				//abro una nueva conexion
				sd = socket (PF_INET, SOCK_STREAM, 0);
				if (sd < 0){
					perror ("error en el socket: ");
					return -1;
				}
				server.sin_family = AF_INET;
				server.sin_port = htons(8001);
	 
				//esta es la direc q deberia cambiar si cambio de pc?
				inet_pton (AF_INET, "127.0.0.1", &server.sin_addr);



				if ((connect(sd, (struct sockaddr*)&server, sizeof (server))) < 0){
		
					perror ("error en connect 2: ");
					return -1;
				}
			
			}
			
			break;
	}
	

	
	
	return 0;

}
	

int readTimeline (int fd2[2]){

	int leido;
	/*para leer el timeline*/
	char timeline[1024];
	char *ret_val;


		while ((leido = read(fd2[0], timeline, sizeof timeline)) > 0){


			if(write (1, timeline, leido) < 0){
				perror ("llamada write");
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

int checkCommand(char comando[15]){
		int c;

		if ((strncmp(comando, "timeline", 8)) == 0){
			c = 1;
				
		}else if ((strncmp(comando, "tweet", 5)) == 0){
			c = 2;

			
		}
		return c;
}

int verifyTweetLength(char line_original[150]){

	int i;
	//para que no cuente el comando y el espacio ("tweet ")
	int charCount = -5;
	//write (1, "fc new ", 8);
	//write (1, line_original, strlen(line_original));
	for (i = 0; line_original[i]; i++){
		if (line_original[i] != '\0' && line_original[i] != '\n'){
			charCount++;
		}
	}

	write (1, "las letras totales del tw son: ", 32);
	printf ("%d\n", charCount);

	if (charCount > 140){
		return 1;
	}else{
		return 0;
	}
	

	

}
