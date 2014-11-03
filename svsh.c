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
void addToVarList(char * variable, char * value){
	VARLIST * current = varlist; 
	int exists = 0; 
	while(current !=NULL && !exists){
		if(strcmp(current->variable, value) == 0){
			strncpy(current->value, value, sizeof(current->	value));
			exists = 1; 
		}
		current = current->next; 
	}
	if(!exists){
		VARLIST * new_entry = malloc(sizeof(VARLIST)); 
		strncpy(new_entry->variable, variable, sizeof(new_entry->variable));
		strncpy(new_entry->value, value, sizeof(new_entry->value)); 
		new_entry -> prev = NULL; 
		new_entry -> next = varlist; 
		if(varlist != NULL)
			varlist->prev = new_entry; 
		varlist = new_entry;
	}
}
void printVarlist(){
	VARLIST * current = varlist; 
	while(current != NULL){
		printf("%s = %s\n", current->variable, current->value);
		current = current->next; 
	}
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
		case(EQUAL):
			variable = strtok(variable, " "); 
			addToVarList(variable, string); 
			printf("%s = %s\n", variable, string); 
			printf("Variables assigned so far: \n"); 
			printVarlist();
			break;  
		default: break; 
	}
}


