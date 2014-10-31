#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "svsh_structs.h"
#include "svsh.h"
#include "parser.tab.h"

static char* prompt; 

void loadCommandPrompt(){
	prompt = "svsh > "; 
}
void printCommandPrompt(void){
	printf("%s", prompt); 
}
void builtInCmd(int command, char* string, char* variable){
	switch(command){
		case(BYE):
			exit(0);
			break; 
		default: break; 
	}
}
