#include "funciones.h"


int main (int argc, char *const argv[]){
	
	int num = 0;
	/*socket descriptor*/
	int sd;
	
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

	
	char comando[15];
	char tweet[140];
	int c=0;
	
	int leido;
	int leido2;
	int leido3;
	int leido4 = 0;
	char *message;


	//variables para buscar el token en el timeline
	char *ret_val;
	const char fin[5] = "fin\n";


	/*para leer el timeline*/
	char timeline[1024];

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
	server.sin_port = htons(7001);
	 
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
				strcpy (line_original, line);
				/*ptr apunta al inicio de line. Hago una "copia" para no perderlo (strtok_r me hace perderlo)*/
				ptr = line;
			
				/*parser*/
				while(token = strtok_r(ptr, " ", &rest)) {
					
		    		if (strcmp(token, "timeline") == 0){
		    			/*copio el comando al arreglo comando para escribir en el pipe*/
		    			
		    			strcpy(comando, token);
		    			
		    			if (write(fd[1], comando, sizeof comando) <0 ){
		    				perror ("llamada write");
		    				return -1;
		    			}
		    			/*Leo el timeline que me devuelve el padre*/

		    			while ((leido3 = read(fd2[0], timeline, sizeof timeline)) > 0){


		    				if(write (1, timeline, leido3) < 0){
		    					perror ("llamada write");
		    				return -1;
		    				}
		    				/*tengo que indicarle hasta donde leer*/
		    				ret_val = strstr(timeline, fin);
					
		    				if (ret_val){
		    					memset(timeline, 0, sizeof timeline);
		    					break;
		    				}

		    		    	
		    			}		
		    		//strncmp
		    		}else if(strcmp(token, "tweet") == 0){
		    				    		
		    			
		    			//STRNCPY
		    			strcpy(comando, token);
		    			//ptr va a tener el resto de la cadena. Puede ser un tweet, un usuario, etc.
		    			ptr = rest;
		    			memset (tweet, 0, sizeof tweet);
		    			//guardo en tweet lo que voy a twitear
		    			strcpy (tweet, rest);
		    			write (1, "sto per scrivire\n", 17);
		    	
		    			/*escribo el comando en el pipe*/
		   				if (write(fd[1], comando, sizeof comando) <0 ){
		    				perror ("llamada write");
		    				return -1;
		    			}
		    			//write (1, "pase el write\n", 14);
		 
		    			/*escribo lo q escribio el usuario en el pipe*/
		    		
		    			if (write(fd[1], line_original, sizeof line_original) <0 ){
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
							
		    		    	
		    			}	
		    			close (fd2[0]);	

		    			}else{
		    				write (1, "estoy en else\n", 14);
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
			
				if ((strcmp(comando, "timeline")) == 0){
					c = 1;
				
				}else if ((strcmp(comando, "tweet")) == 0){
					c = 2;
					
				}

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
					   	while (	(leido2 = read (sd, timeline, sizeof timeline)) > 0){ 
							if (write(fd2[1], timeline, leido2) <0){
								perror ("llamada write");
								return -1;
							}
							
						}
						//write (1, "el padre escribe el token",26);
						write(fd2[1], fin, strlen(fin));
						break;

					case 2:
						
					   	/*mando al servidor el comando y el tweet*/
						while ((leido = read(fd[0], line_original, sizeof line_original)) >0){
							
							//VER COMO RESOLVER. TWITTEA BASURA
							if(write (sd, line_original, leido) <0){
								perror ("llamada write");
								return -1;
							}
							close (fd[0]);
						
						}
						
						/*leo respuesta del servidor y la escribo en el pipe*/
						 /*vacio el buf respuesta*/
					   	memset (respuesta, 0, sizeof respuesta);
						
					
						while ((leido2 = read (sd, respuesta, sizeof respuesta)) >0){
							
							
							if (write(fd2[1], respuesta, leido2) <0){
								perror ("llamada write");
								return -1;
							}
							write(fd2[1], fin, strlen(fin));
							close (fd2[1]);
						}
						
						
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
				server.sin_port = htons(7001);
	 
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
	
