%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "svsh_structs.h"
#include "svsh.h"
extern void builtInCmd(int command, char* string, char* variable);
extern void userCommand(ARGLIST *arglist, char *input, char *output);
int yylex(void); 
void yyerror(char *s); 
%}
%union{
	char* keyword; 
	char* variable; 
	char* string; 
	char* word; 
	char* metachar; 
}; 

%token DEFPROMPT CD LISTJOBS BYE NEWLINE
%token <keyword> KEYWORD
%token <variable> VARIABLE 
%token <string> STRING
%token <word> WORD 
%token <metachar> META 
%%
program: prompts
	 |
	 ;
prompts: 
	prompt 
 	| prompts prompt
	;
prompt:
	command NEWLINE	{printCommandPrompt();}
	| command prompt
	| error NEWLINE	{
				yyerrok; 
				yyclearin; 
				printCommandPrompt(); 
			}
	; 
command:
	KEYWORD 	{printf("Token Type: keyword\t Token: %s\n", $1);}
	| VARIABLE 	{printf("Token Type: variable\t Token: %s\n", $1);}
	| STRING 	{printf("Token Type: string\t Token: %s\n", $1);}
	| WORD		{printf("Token Type: word\t Token: %s\n", $1); }
	| META		{printf("Token Type: metachar\t Token: %s\n", $1);}
	| BYE		{builtInCmd(BYE, NULL, NULL);}	
	; 

%%

int main(void){
	loadCommandPrompt();
	yyparse(); 
	return 0;
}
void yyerror (char *s) {
   fprintf (stdout,"%s\n", s);
}
