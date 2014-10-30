%{
%}
%option noyywrap
VARIABLE 	[$][A-Za-z0-9]*
STRING 		["][A-Za-z0-9&+,:;?@|i=#<>.^*()%!`~\/-_" "-]*["]
STRING2		(["]*)
KEYWORD		(?:defprompt|cd|listjobs|bye|run|assignto|<bg>)
META		[=|#]
WORD		[a-zA-Z0-9&+,:;?@|'<>.^*()%!`~\/-_-]+
%%

[\t ]+		/* ignore whitespace */ ;
["]+ 
{KEYWORD}	{return KEYWORD; }

{VARIABLE}	{return VARIABLE;} 

{META}		{return META; }

{STRING}	{return STRING; }

{WORD}		{return WORD; } 

%%

int main (void) {
        int result;
        int running = 1;
        while(running){
                result = yylex();
                switch(result){
                        case KEYWORD:  printf ("Token Type: Keyword\t\t Token: %s \n", yytext); break;
                        case VARIABLE: printf("Token Type: Variable\t\t Token: %s\n", yytext); break;
                        case META:     printf("Token Type: Meta\t\t Token %s\n", yytext); break;
                        case STRING:   printf("Token Type: String\t\t Token: %s\n", yytext); break;
                        case WORD:     printf("Token Type: Word\t\t Token: %s\n", yytext); break;
                        default: break;
                }
        }
        return 0;
}

