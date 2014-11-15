#ifndef _SVSH_STRUCTS_H_
#define _SVSH_STRUCTS_H_

#define LIMIT 256

typedef struct arglist{
	struct arglist *next; 
	char args[LIMIT]; 
} ARGLIST; 

typedef struct varlist{
	struct varlist *next; 
	struct varlist *prev; 
	char variable[LIMIT]; 
	char value[LIMIT]; 
} VARLIST; 

typedef struct tokenlist{
	struct tokenlist *next; 
	char type[LIMIT]; 
	char token[LIMIT]; 
	char usage[LIMIT]; 
} TOKENLIST; 
#endif
