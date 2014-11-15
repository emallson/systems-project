svsh: parser.tab.c lex.yy.c svsh.c
	gcc -g -o $@ $^
svshg: parser.tab.c lex.yy.c svsh.c
	gcc -g -o $@ $^
parser.tab.c: parser.y
	bison -d parser.y
lex.yy.c: scanner.l
	flex scanner.l
clean: 
	rm lex.yy.c parser.tab.c parser.tab.h svsh
