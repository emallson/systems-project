%{
	#define YY_NO_INPUT
	#include "parser.tab.h"
	#include <string.h>
	#include <stdlib.h>
	void yyerror(char *s); 
%}
%option noyywrap
%option nounput
VARIABLE 	[$][A-Za-z0-9]*
STRING 		["][A-Za-z0-9&+,:;?@|i=#<>.^*()%!`~\/-_" "-]*["]
KEYWORD		(?:defprompt|cd|listjobs|bye|run|assignto|<bg>)
META		[=|#]
WORD		[a-zA-Z0-9&+,:;?@|'<>.^*()%!`~\/-_-]+
%%

[\t ]+		/* ignore whitespace */ ;
"bye"           {return BYE;}
"listjobs"      {return LISTJOBS; }
"cd"            {return CD; }
"defprompt"     {return DEFPROMPT; }
"#".*           ;
{KEYWORD}	{yylval.keyword = yytext; return KEYWORD; }

{VARIABLE}	{yylval.variable = yytext; return VARIABLE;} 

{META}		{yylval.metachar = yytext; return META; }

{STRING}	{yylval.string = yytext; 
		yylval.string = strdup(yytext); 
		yylval.string++; 
		yylval.string[strlen(yylval.string)-1] = '\0';
		return STRING; }

{WORD}		{yylval.word = yytext; return WORD; } 
"\n"		{return NEWLINE;}
%%
