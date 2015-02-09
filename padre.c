#include "padre.h"
#include "commonLib.h"

int checkCommand(char comando[15]){
		int c;

		if ((strncmp(comando, "timeline", 8)) == 0){
			c = 1;
				
		}else if ((strncmp(comando, "tweet", 5)) == 0){
			c = 2;

		}else if ((strncmp(comando, "user", 4)) == 0){

			c = 3;

		}else if((strncmp(comando, "search", 6)) == 0){

			c = 4;

		}
		return c;
}