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
	char varName[LIMIT]; 
	char varVal[LIMIT]; 
} VARLIST; 

#endif
