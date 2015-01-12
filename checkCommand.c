#include "padre.h"
#include "funciones.h"

int checkCommand(char comando[15]){
		int c;

		if ((strncmp(comando, "timeline", 8)) == 0){
			c = 1;
				
		}else if ((strncmp(comando, "tweet", 5)) == 0){
			c = 2;

			
		}
		return c;
}