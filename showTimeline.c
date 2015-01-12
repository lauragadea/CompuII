#include "funciones.h"
#include "hijo.h"

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