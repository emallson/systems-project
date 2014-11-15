#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>

void getCommandFromUser(char command[]); 
int parseCommand(char command[], char* parsedCommand[]); 

int main(){
	char commandLine[256]; 
	char *parsedCommand[20]; 
	int background = 0; 

	pid_t pid; 
	
	printf("svsh > "); 
	getCommandFromUser(commandLine); 
	background = parseCommand(commandLine, parsedCommand); 	
	while((strcmp(commandLine, "bye") != 0)){
		if((pid = fork()) == 0){
			execvp(parsedCommand[0], parsedCommand); 
			exit(1); 
		} else { 
			if(!background)
				waitpid(pid, NULL, 0);	
			printf("svsh > "); 
			strcpy(commandLine, "\0"); 
			getCommandFromUser(commandLine); 
			background = parseCommand(commandLine, parsedCommand); 
		}
	}	

}

void getCommandFromUser(char command[]){
	fgets(command, 256, stdin); 
	char *pos; 
	if((pos = strchr(command, '\n')) != NULL){
		*pos = '\0'; 
	}
}

int parseCommand(char command[], char* parsedCommand[]){
	int returnVal = 0; 
	*parsedCommand = strtok(command, " "); 
	while(*parsedCommand != NULL){
		if(!strcmp(*parsedCommand, "bg")){
			returnVal = 1; 
			*parsedCommand = '\0'; 
		}
		*parsedCommand++; 
		*parsedCommand = strtok(NULL, " "); 
	}
	return returnVal; 
}

