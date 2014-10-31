svsh: parser.tab.c lex.yy.c svsh.c
	gcc -o svsh parser.tab.c lex.yy.c svsh.c
parser.tab.c: parser.y
	bison -d parser.y
lex.yy.c: scanner.l
	flex scanner.l
