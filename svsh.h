#ifndef _SVSH_H_
#define _SVSH_H_

#include <stdlib.h>

void loadCommandPrompt(void); 
void printCommandPrompt(void); 

void builtInCmd(int command, char* string, char* variable); 
void userCommand(ARGLIST *arglist, char *input, char *output); 

VARLIST * varlist; 

#endif
