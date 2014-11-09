#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "svsh_structs.h"
#include "svsh.h"
#include "parser.tab.h"

static char* prompt;
extern VARLIST* varlist;

void loadCommandPrompt(){
	prompt = "svsh > ";
}
void printCommandPrompt(void){
	printf("%s", prompt);
}
void addToVarList(char * variable, char * value){
	VARLIST * current = varlist;
	int exists = 0;
	while(current !=NULL && !exists){
		if(strcmp(current->variable, value) == 0){
			strncpy(current->value, value, sizeof(current->	value));
			exists = 1;
		}
		current = current->next;
	}
	if(!exists){
		VARLIST * new_entry = malloc(sizeof(VARLIST));
		strncpy(new_entry->variable, variable, sizeof(new_entry->variable));
		strncpy(new_entry->value, value, sizeof(new_entry->value));
		new_entry -> prev = NULL;
		new_entry -> next = varlist;
		if(varlist != NULL)
			varlist->prev = new_entry;
		varlist = new_entry;
	}
}
void printVarlist(){
	VARLIST * current = varlist;
	while(current != NULL){
		printf("%s = %s\n", current->variable, current->value);
		current = current->next;
	}
}
void builtInCmd(int command, char* string, char* variable){
	char cwd[1024];
	switch(command){
		case(BYE):
			exit(0);
			break;
		case(DEFPROMPT):
			prompt = string;
			break;
		case(CD):
			if(chdir(string) < 0)
				perror("cd");
			break;
		case(LS):
       			if (getcwd(cwd, sizeof(cwd)) != NULL)
           			fprintf(stdout, "Current working dir: %s\n", cwd);
       			else
           			perror("getcwd() error");
			break;
		case(EQUAL):
			variable = strtok(variable, " ");
			addToVarList(variable, string);
			printf("%s = %s\n", variable, string);
			printf("Variables assigned so far: \n");
			printVarlist();
			break;
		default: break;
	}
}

/* assumes that there is sufficient space in `source' */
/* modifies `source' */
/* replace `sub' with `repl' in `source' */
void strsubst(char* source, char* sub, char* repl) {
    size_t src_len = strlen(source),
        sub_len = strlen(sub),
        repl_len = strlen(repl);

    char* beg, *end, *tmp;

    tmp = (char*)malloc(sizeof(char[LIMIT]));

    while((beg = strstr(source, sub))) {
        end = beg + sub_len;
        strncpy(tmp, end, LIMIT);
        /* the remaining space in the buffer is LIMIT - (beg - source) */
        strncpy(beg, repl, LIMIT - (beg - source));
        /* again, copy at most remaining space in buffer */
        strncpy(beg + repl_len, tmp, LIMIT - (beg + repl_len - source));
    }

    free(tmp);
}

char** build_argv(ARGLIST* arglist) {
    VARLIST* head = varlist, *var = varlist;
	ARGLIST* arg_it = arglist;
	int arg_size = 0;

	while(arg_it != NULL){
		arg_size++;
		arg_it = arg_it->next;
	}

    char **argv = malloc(sizeof(arg_size+1) * sizeof(char[LIMIT]));
	arg_it = arglist;
	int i = 0;
	while(arg_it != NULL){
		argv[i] = arg_it->args;
        while(var != NULL) {
            strsubst(argv[i], var->variable, var->value);
            var = var->next;
        }
        var = head;
        printf("argv[%d] = %s\n", i, argv[i]);
		arg_it = arg_it->next;
		i++;
	}
	argv[i] = NULL;

    return argv;
}

void assignCommand(char* varname, ARGLIST* arglist) {
  pid_t pid;
  int state;
  /* file descriptor to pipe data from command */
  int fd[2];

  printf("assignto: %s\n", varname);

  char** argv = build_argv(arglist);
  char* output = (char*)malloc(sizeof(char[LIMIT]));

  pipe(fd);
  if((pid = fork()) == 0) {
    dup2(fd[1], STDOUT_FILENO);
    execvp(argv[0], argv);
    exit(1);
  }
  if(waitpid(pid, &state, 0) < 0) {
    perror("WAITPID");
    kill(pid, SIGKILL);
  }

  printf("assigning to var %s ", varname);
  /* read the output into a variable */
  read(fd[0], output, LIMIT);
  output[LIMIT-1] = '\0';
  printf("value: %s\n", output);
  addToVarList(varname, output);
  free(argv);
}

void runCommand(ARGLIST* arglist) {
	pid_t pid;
	int state;

    printf("run!\n");

    char** argv = build_argv(arglist);

	if((pid = fork()) == 0){
		execvp(argv[0], argv);
		exit(1);
	/*	if(execve(argv[0], argv, vars) < 0){
			printf("%s\n", argv[0]);
			printf("%s: command not found\n", argv[0]);
			exit(0);
		} */
	}
	if(waitpid(pid, &state, 0) < 0){
		perror("WAITPID");
		kill(pid, SIGKILL);
	}
	free(argv);
}
