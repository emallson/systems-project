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
static int argcount = 0;
char argtoken[6];
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
	DEFPROMPT WORD		{
					addToTokenList("word", $2, "anytext");
					addToTokenList("keyword", "defprompt", "defprompt");
					builtInCmd(DEFPROMPT, $2, NULL);
				}
        |DEFPROMPT STRING       {
                                        addToTokenList("string", $2, "anytext");
                                        addToTokenList("keyword", "defprompt", "defprompt");
					builtInCmd(DEFPROMPT, $2, NULL);
                                }
	|VARIABLE EQUAL WORD    {
					addToTokenList("word", $3, "variable_def");
					addToTokenList("metachar", "=", "assignment");
					addToTokenList("word", $1, "variable_name");
					builtInCmd(EQUAL, $3, $1);
				}
        |VARIABLE EQUAL STRING  {
                                        addToTokenList("string", $3, "variable_def");
                                        addToTokenList("metachar", "=", "assignment");
                                        addToTokenList("word", $1, "variable_name");
                                        builtInCmd(EQUAL, $3, $1);
                                }
	|CD WORD		{
					addToTokenList("word", $2, "directory");
					addToTokenList("keyword", "cd", "change directory");
					builtInCmd(CD, $2, NULL);
				}
	|BYE			{builtInCmd(BYE, NULL, NULL);}
	|COMMENT arglist 	{
				 	yyerrok;
				 	yyclearin;
				}
	|LISTJOBS 		{
        addToTokenList("keyword", "listjobs", "listjobs");
        printJobList();
     }

	|ASSIGNTO VARIABLE arglist {
        addToTokenList("variable", $2, "destination");
        addToTokenList("keyword", "assignto", "assignto");
        assignCommand($2, $3);
    }

	|RUN arglist BG 	{
					runCommand($2, 1);
				}
	|arglist BG 		{
        runCommand($1, 1);
				}
	|RUN arglist 		{
					argcount = 0;
					addToTokenList("keyword", "run", "run");
					runCommand($2, 0);
				}
	|arglist 		{
					argcount = 0;
					runCommand($1, 0);
				}
	;
arg:
        WORD                    {
					sprintf(argtoken, "arg %d", argcount);
					addToTokenList("word", $1, argtoken);
					argcount++;
					$$ = $1;
				}
        |STRING                 {
			 		sprintf(argtoken, "arg %d", argcount);
                                        addToTokenList("string", $1, argtoken);
                                        argcount++;
					$$ = $1;
				}
	|VARIABLE		{
					sprintf(argtoken, "arg %d", argcount);
					addToTokenList("variable", $1, argtoken);
					argcount++;
					$$ = $1;
				}
	|EQUAL			{
					sprintf(argtoken, "arg %d", argcount);
					addToTokenList("metachar", "=", argtoken);
					argcount++;
				}
	|COMMENT		{
					sprintf(argtoken, "arg %d", argcount);
					addToTokenList("metachar", "#", argtoken);
					argcount++;
				}
        ;
arglist:
	arg arglist 		{
					$$ = constructArglist($1, $2);
				}
	|arg 			{
					$$ = constructArglist($1, NULL);
				}
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
