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
	|NEWLINE	{
				printf("No input detected!\n");
				printCommandPrompt();  
			}
	;
command:
	DEFPROMPT WORD		{
					addToTokenList("word", $2, "anytext"); 
					addToTokenList("keyword", "defprompt", "defprompt");
					printf("Error: Prompt must be a string; e.g. defprompt \"prompt > \"\n"); 
				}
	|DEFPROMPT WORD arg	{
					argcount = 1; //make sure the usage is never 'cmd'
					addToTokenList("word", $2, "anytext"); 
					addToTokenList("keyword", "defprompt", "defprompt");
					printf("Error: Prompt must be a string; e.g. defprompt \"prompt > \"\n");
				}
	|DEFPROMPT STRING arg   {
					argcount = 1; //make sure the usage is never 'cmd'
					addToTokenList("string", $2, "anytext");
					addToTokenList("keyword", "defprompt", "defprompt");
					printf("Error: Prompt must be a string; e.g. defprompt \"prompt > \"\n");
				}
        |DEFPROMPT STRING       {
                                        addToTokenList("string", $2, "anytext");
                                        addToTokenList("keyword", "defprompt", "defprompt");
					builtInCmd(DEFPROMPT, $2, NULL);
                                }
	|VARIABLE EQUAL STRING arg{
					argcount = 1; //make sure the usage is never cmd
					addToTokenList("string", $3, "variable_def"); 
					addToTokenList("metachar", "=", "assignment");
                                        addToTokenList("word", $1, "variable_name");
                                        printf("Variable assignment should be of the form Variable = Definiton\n");
                                }
	|VARIABLE EQUAL WORD arg{
					argcount = 1; //make sure the usage is never cmd
					addToTokenList("word", $3, "variable_def");
                                        addToTokenList("metachar", "=", "assignment");
                                        addToTokenList("word", $1, "variable_name");
                                        printf("Variable assignment should be of the form Variable = Definiton\n");
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
	|CD WORD arg		{
					argcount = 1; //make sure the usage is never cmd
					addToTokenList("word", $2, "directory");
                                        addToTokenList("keyword", "cd", "change directory");
					printf("The cd command must be in the form 'cd location'\n");
				}
	|BYE			{
					addToTokenList("keyword", "bye", "exit"); 
					builtInCmd(BYE, NULL, NULL);
				}
	|BYE arg		{
					argcount = 1; //make sure the usage is never cmd
					addToTokenList("keyword", "bye", "exit"); 
					printf("The command 'bye' should only be typed if you wish to exit!\n");}
	|COMMENT arglist 	{
					argcount = 0; 
					addToTokenList("metachar", "#", "comment"); 
				 	yyerrok;
				 	yyclearin;
					printCommandPrompt();
				}
	|LISTJOBS 		{
        				addToTokenList("keyword", "listjobs", "listjobs");
				        printJobList();
    				}
	|LISTJOBS arg		{
					argcount = 0; 
					addToTokenList("keyword", "listjobs", "listjobs");
					printf("Listjobs must be singularly typed to list current jobs!\n");}

	|ASSIGNTO VARIABLE arglist {
					argcount = 1; 
				        addToTokenList("variable", $2, "destination");
				        addToTokenList("keyword", "assignto", "assignto");
				        assignCommand($2, $3);
   				}

	|RUN arglist BG 	{
					argcount = 0; 
					addToTokenList("background", "<bg>", "background process");
					addToTokenList("keyword", "run", "run");  
					runCommand($2, 1);
				}
	|arglist BG 		{
					argcount = 0; 
					addToTokenList("background", "<bg>", "background process"); 
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
