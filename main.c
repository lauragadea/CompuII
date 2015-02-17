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

	char *word;
	char *saveptr;

	//search
	char palabraBuscada[20] = {'\0'};
	//variables archivo de configuracion
	int opc;
	char conf[20];
	char *direc;
	char direccion[15];
	//char *path = "./history.txt";

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

	//parsea archivo de configuracion
	while ((opc = getopt (argc, argv, "f:")) != -1){
		switch (opc) {
			case 'f':
					printf ("Archivo de configuración utilizado: %s\n", optarg);
					strcpy (conf, optarg);
	
					direc = parseo (conf);
					//inicializar direccion en 0
					strcpy(direccion,direc);
					printf ("direccion servidor = %s\n", direccion);
					break;
		}

	}

	//creo el socket
	sd = socket (PF_INET, SOCK_STREAM, 0);
	if (sd < 0){
		perror ("error en el socket: ");
		return -1;
	}
	
	//delete previous history
	//unlink (path);

	memset (&server, 0, sizeof (server));
	server.sin_family = AF_INET;
	server.sin_port = htons(8001);
	
	 
	//converts direction to ascii
	inet_pton (AF_INET, direccion, &server.sin_addr);


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
			
				if (write (1, "\nCliente>", 9) <0){
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
			
					/*get Timeline DONE CON FIN*/
					case 1:
						
						/*mando al servidor el comando*/
						if (write(sd, comando, strlen(comando)) < 0){
							perror ("llamada write");
	   						return -1;

	   					}
						getTimeline(sd, pipefd2[1]);

						break;

					/*post a tweet DONE CON FIN*/
					case 2:
						
						
						/*mando al servidor el comando y el tweet*/
						while ((leido = read(pipefd[0], line_original, sizeof line_original)) >0){
						
							
							if(write (sd, line_original, leido) <0){
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
					
					/*get a user's timeline DONE CON FIN*/
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
										
							if(write (sd, line_original, leido) <0){
								perror ("llamada write");
								return -1;
							}
							ret_val = strstr(line_original, fin);

							if (ret_val){
								break;
							}

						}

						
						saveTimeline(sd);
						//comando
						strtok_r(line_original, " \n\r\t", &saveptr);
						//usuario
						strtok_r(NULL, " \n\r\t", &saveptr);
						//palabra a buscar
						word = strtok_r(NULL, " \n\r\t", &saveptr);
						memset (palabraBuscada, 0, sizeof palabraBuscada);
						strcpy (palabraBuscada, word);
						
						write (1, "palabra buscada: ",18);
						write(1, palabraBuscada, strlen(palabraBuscada));
						
						searchWord (palabraBuscada, pipefd2[1]);
						
						break;

					/*exit*/
					case 5:
						printf ("Thanks for using our service. Have a nice day.\n");
						//close (pipefd);
						//close (pipefd2);
						close (sdtc);

						exit(0);
						
						break;

					default:
						write (1, "Not a valid command", 20);
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
	
//CUANDO HAGA EXIT, HACE UNLINK DEL HISTORY
	
	
	return 0;

}
	


