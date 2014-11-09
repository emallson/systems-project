%{
	#define YY_NO_INPUT
	#include "svsh_structs.h"
	#include "parser.tab.h"
	#include <string.h>
	#include <stdlib.h>
	void yyerror(char *s);
%}
%option noyywrap
%option nounput
VARIABLE 	\$[A-Z0-9a-z]+
STRING 		["][$A-Za-z0-9&+,:;?@|i=#<>.^*()%!`~\/-_" "-]*["]
KEYWORD		(?:listjobs|run|assignto|<bg>)
META		[=|#]
WORD		[a-zA-Z0-9&+,:;?@|<>.^*()%!`~\/-_-]+
%%

[\t ]+		/* ignore whitespace */ ;
^bye            {return BYE;}
^listjobs       {return LISTJOBS; }
^cd             {return CD; }
^defprompt     	{return DEFPROMPT; }
^run            {return RUN; }
^assignto       {return ASSIGNTO; }
"<bg>"			{return BG; }
\=		{return EQUAL;}
#		{return COMMENT;}
{KEYWORD}	{yylval.keyword = yytext; return KEYWORD; }

{VARIABLE}	{yylval.variable = strdup(yytext); return VARIABLE;}

{STRING}	{
			yylval.string = yytext;
			yylval.string = strdup(yytext);
			yylval.string++;
			yylval.string[strlen(yylval.string)-1] = '\0';
			return STRING;
		 }

{WORD}		{
			yylval.word = yytext;
			yylval.string = strdup(yytext);
			return WORD;
		}
"\n"		{return NEWLINE;}
%%
