#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
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
	char cwd[1024]; 
	switch(command){
		case(BYE):
			exit(0);
			break; 
		case(DEFPROMPT):
			prompt = string; 
			break;
		case(CD):
			if(chdir(string) < 0)
				perror("cd");
			break;
		case(LS):
       			if (getcwd(cwd, sizeof(cwd)) != NULL)
           			fprintf(stdout, "Current working dir: %s\n", cwd);
       			else
           			perror("getcwd() error");
			break; 
		default: break; 
	}
}


