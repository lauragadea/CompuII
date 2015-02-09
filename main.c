#include "commonLib.h"
#include "padre.h"
#include "hijo.h"
#include "twitter.h"


int main (int argc, char *const argv[]){

	
	/*socket descriptor*/
	int sd;

	/*socket descriptor totalmente conectado*/
	int sdtc = 0;
	struct sockaddr_in server = {};
	
	/*buffer para ver que recibo*/
	char buffer[3000];
	memset (buffer, 0, sizeof buffer);
	/*variables para parsear linea de comandos*/
	char line_original[150];

	char *ret_val;
	
	/*return value of fork*/
	int ret=0;
	/*descriptores pipe (para que escriba el hijo)*/
	int pipefd[2];
	/*descriptores pipe2 (para que escriba el padre)*/
	int pipefd2[2];

	char comando[15];
	
	int c=0;
	
	int leido;


	//char *com;
	//char *usr;
	char *word;
	char *saveptr;


	char palabraBuscada[20];
		
	const char fin[5] = "fin\n";

	/*creo los pipes*/
	if ( pipe (pipefd) != 0 ){

			perror ("pipe");
			return -1;
	}

	if ( pipe (pipefd2) != 0 ){ 

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
	switch(ret){

		case -1:

			perror ("error en fork");
			sleep (1);

			return -1;

		/*hijo*/
		case 0:
			close (pipefd[0]);
			close (pipefd2[1]);

			while (1){
				/*pide un comando*/
			
				if (write (1, "Cliente>", 9) <0){
					perror ("llamada write");
					return -1;
				}
				/*paso solo los descriptores que va a usar*/
				childService(pipefd[1], pipefd2[0]);

		   			
		    	
			}
        	return 0;	

		/*padre*/
		default:
			/*cierra descriptores del pipe*/
			close(pipefd[1]);
			close (pipefd2[0]);
			
			
		
			while (1){
												
				leido = read (pipefd[0], comando, sizeof comando); 

				c = checkCommand(comando);
			
			

				switch(c){
			
					/*get Timeline*/
					case 1:
						
						/*mando al servidor el comando*/
						if (write(sd, comando, strlen(comando)) < 0){
							perror ("llamada write");
	   						return -1;

	   					}
						getTimeline(sd, pipefd2[1]);

						break;

					/*post a tweet*/
					case 2:
						
						
						/*mando al servidor el comando y el tweet*/
						while ((leido = read(pipefd[0], line_original, sizeof line_original)) >0){
						
							//le resto 4 asi no manda el "fin" 
							if(write (sd, line_original, leido - 4) <0){
								perror ("llamada write en tweeting");
								return -1;
							}
							ret_val = strstr(line_original, fin);

							if (ret_val){
								break;
							}

						}
					   							
						/*leo respuesta del servidor y la escribo en el pipe*/
						
						getTwitterResponse(sd, pipefd2[1]);	

					   	
						break;
					
					/*get a user's timeline*/
					case 3:
						
						/*mando al servidor el comando y el nombre de usuario*/
						while ((leido = read(pipefd[0], line_original, sizeof line_original)) >0){
						
							
							if(write (sd, line_original, leido - 4) <0){
								perror ("llamada write");
								return -1;
							}
							ret_val = strstr(line_original, fin);

							if (ret_val){
								break;
							}

						}

						/*leo respuesta del servidor y la escribo en el pipe*/
						getTimeline(sd, pipefd2[1]);	
					   
						break;

					/*search a word in a user's timeline*/
					case 4:

						/*mando al servidor el comando, el usuario y la palabra a buscar*/
						while ((leido = read(pipefd[0], line_original, sizeof line_original)) >0){
						
						
							
							if(write (sd, line_original, leido - 4) <0){
								perror ("llamada write");
								return -1;
							}
							ret_val = strstr(line_original, fin);

							if (ret_val){
								break;
							}

						}

						
						//EL HIJO TIENE Q ESPERAR QUE EL PADRE TERMINE ANTES DE VOLVER A DECIR CLIENTE
						saveTimeline(sd);
						//comando
						strtok_r(line_original, " ", &saveptr);
						//usuario
						strtok_r(NULL, " ", &saveptr);
						//palabra a buscar
						word = strtok_r(NULL, " ", &saveptr);
						//write (1, word, sizeof word);
						//OJO, deberia usar strncpy
						strcpy (palabraBuscada, word);
						printf ("palabraBuscada = %s\n", palabraBuscada);
						//searchWord (palabraBuscada);
					
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
	


