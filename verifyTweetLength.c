#include "funciones.h"
#include "hijo.h"


int verifyTweetLength(char line_original[150]){

	int i;
	//para que no cuente el comando y el espacio ("tweet ")
	int charCount = -6;
	
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