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

%token LS DEFPROMPT CD LISTJOBS BYE NEWLINE ASSIGN EQUAL COMMENT
%token <keyword> KEYWORD
%token <variable> VARIABLE 
%token <string> STRING
%token <word> WORD 
%token <metachar> META 
%type <string> command
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
	DEFPROMPT STRING	{builtInCmd(DEFPROMPT, $2, NULL);}
	|VARIABLE EQUAL WORD    {builtInCmd(EQUAL, $3, $1);}
	|VARIABLE EQUAL STRING	{builtInCmd(EQUAL, $3, $1);}
	|CD WORD		{builtInCmd(CD, $2, NULL);}
	|KEYWORD 		{printf("Token Type: keyword\t Token: %s\n", $1);}
	|VARIABLE 		{printf("Token Type: variable\t Token: %s\n", $1);} 
	|STRING 		{printf("Token Type: string\t Token: %s\n", $1);}   
	|WORD			{printf("Token Type: word\t Token: %s\n", $1); } 
	|BYE			{builtInCmd(BYE, NULL, NULL);}
	|LS			{builtInCmd(LS, NULL, NULL); }
	;
%%

int main(void){
	loadCommandPrompt();
	printCommandPrompt(); 
	yyparse(); 
	return 0;
}
void yyerror (char *s) {
   fprintf (stdout,"%s\n", s);
}
