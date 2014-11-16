#ifndef _SVSH_H_
#define _SVSH_H_
#define KERNEL_SUPPORT

#include <stdlib.h>

void loadCommandPrompt(void); 
void printCommandPrompt(void); 

void builtInCmd(int command, char* string, char* variable); 
void userCommand(ARGLIST *arglist, char *input, char *output); 

VARLIST * varlist; 
TOKENLIST* tokenlist; 
#endif
