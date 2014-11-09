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
ARGLIST* constructArglist(char* arg, ARGLIST* arglist);
%}
%union{
	char* keyword;
	char* variable;
	char* string;
	char* word;
	char* metachar;
	ARGLIST* arg_list;
};

%token LS DEFPROMPT CD LISTJOBS BYE NEWLINE ASSIGNTO RUN EQUAL COMMENT BG
%token <keyword> KEYWORD
%token <variable> VARIABLE
%token <string> STRING
%token <word> WORD
%token <metachar> META
%type <string> command
%type <string> arg
%type <arg_list> arglist
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
	DEFPROMPT arg		{builtInCmd(DEFPROMPT, $2, NULL);}
	|VARIABLE EQUAL arg     {builtInCmd(EQUAL, $3, $1);}
	|CD WORD		{builtInCmd(CD, $2, NULL);}
	|BYE			{builtInCmd(BYE, NULL, NULL);}
	|COMMENT arglist {yyerrok;
				 yyclearin;
				 printCommandPrompt();}
    |LISTJOBS {printJobList();}
	|ASSIGNTO VARIABLE arglist {assignCommand($2, $3);}
	|RUN arglist BG {runCommand($2, 1);}
	|arglist BG {runCommand($1, 1);}
	|RUN arglist {runCommand($2, 0);}
	|arglist {runCommand($1, 0);}
	;
arg:
        WORD                    {$$ = $1;}
        |STRING                 {$$ = $1;}
		|VARIABLE				{$$ = $1;}
        ;
arglist:
	arg arglist {$$ = constructArglist($1, $2);}
	|arg {$$ = constructArglist($1, NULL);}
	;
%%
ARGLIST* constructArglist(char* arg, ARGLIST* arglist){
	ARGLIST* new_arg = malloc(sizeof(ARGLIST));
	strncpy(new_arg->args, arg, sizeof(new_arg->args));
	new_arg->next = arglist;
	arglist = new_arg;
	return arglist;
}
int main(void){
    svshInit();
	printCommandPrompt();
	yyparse();
	return 0;
}
void yyerror (char *s) {
   fprintf (stdout,"%s\n", s);
}
