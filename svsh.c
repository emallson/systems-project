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

#define __NR_SaveVariable 315
#define __NR_GetVariable 316
#define __NR_NextVariable 317

static char* prompt;
extern VARLIST* varlist;
extern TOKENLIST* tokenlist;
static int argcount = 0;
typedef struct pidlist_node_struct {
    pid_t pid;
    char* name;
    struct pidlist_node_struct* next;
} pidlist_node;

typedef struct pidlist_struct {
    pidlist_node* head, *tail;
    size_t length;
} pidlist;

pidlist joblist;

void svshInit(){
    joblist.head = joblist.tail = NULL;
    joblist.length = 0;
	prompt = "svsh > ";
}

void addToTokenList(char* type, char* token, char* usage){
	if(strcmp(usage, "variable_name") == 0)
		token = strtok(token, " ");
	else if(strcmp(usage, "arg 0") == 0){
		usage = "cmd";
	}
    TOKENLIST* new_entry = malloc(sizeof(TOKENLIST));
    strncpy(new_entry->type, type, sizeof(new_entry->type));
    strncpy(new_entry->token, token, sizeof(new_entry->token));
    strncpy(new_entry->usage, usage, sizeof(new_entry->usage));
    new_entry->next  = tokenlist;
    tokenlist = new_entry;
}

void clearTokenList(){
	TOKENLIST* current = tokenlist;
	TOKENLIST* next_item;
	while(current != NULL){
		next_item = current->next;
		free(current);
		current = next_item;
	}
    tokenlist = NULL;
}

void printTokenList(){
        TOKENLIST* current = tokenlist;
        while(current != NULL){
                printf("Token Type: %15s\t Token: %15s\t Usage: %15s\n",current->type, current->token, current->usage);
                current = current->next;
        }
}

int getVar(char* varname, char* value, size_t length) {
#ifdef KERNEL_SUPPORT
    return syscall(__NR_GetVariable, varname, value, length) == 0;
#else
    VARLIST* current = varlist;
    while(current) {
        if(strncmp(current->variable, varname, LIMIT) == 0) {
            strncpy(value, current->value, length);
            return 1;
        }
        current = current->next;
    }
    return 0;
#endif
}

void addToVarList(char * variable, char * value){
#ifdef KERNEL_SUPPORT
    syscall(__NR_SaveVariable, variable, value);
#else
	VARLIST * current = varlist;
	int exists = 0;
	while(current !=NULL && !exists){
		if(strcmp(current->variable, variable) == 0){
			strncpy(current->value, value, LIMIT);
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
#endif
}

void printVarlist(){
#ifdef KERNEL_SUPPORT
    char varname[LIMIT], vardef[LIMIT];
    varname[0] = '\0';
    while(syscall(__NR_NextVariable, varname, varname, LIMIT, vardef, LIMIT)) {
        printf("%s = %s\n", varname, vardef);
    }
#else
	VARLIST * current = varlist;
	while(current != NULL){
		printf("%s = %s\n", current->variable, current->value);
		current = current->next;
	}
#endif
}

void addToJobList(pid_t pid, char* name) {
    pidlist_node* node = malloc(sizeof(pidlist_node));
    node->pid = pid;
    node->name = strdup(name);

    if(joblist.tail) {
        joblist.tail->next = node;
    }
    joblist.tail = node;

    if(!joblist.head) {
        joblist.head = node;
    }
    joblist.length++;
}

void removeJob(pidlist_node* job, pidlist_node* prev) {
    if(joblist.head == job) {
        joblist.head = job->next;
    }
    if(joblist.tail == job) {
        joblist.tail = prev;
    }
    if(prev) {
        prev->next = job->next;
    }
    free(job->name);
    free(job);
    joblist.length--;
}

/* kills ALL jobs */
void killJobs() {
    pidlist_node* current = joblist.head;
    while(current != NULL) {
        kill(current->pid, SIGKILL);
        current = current->next;
    }
}

void pprint_state(int state) {
    if(WIFEXITED(state)) {
        printf("[Exited: %d]", WEXITSTATUS(state));
    }
    if(WIFSIGNALED(state)) {
        printf("[Signaled: %d", WTERMSIG(state));
#ifdef WCOREDUMP
        if(WCOREDUMP(state)) {
            printf(" with core dump");
        }
#endif
        printf("]");
    }
    /* right now this can only happen if an external command is run to stop the
       process. There is no shell behavior to stop a process. */
    if(WIFSTOPPED(state)) {
        printf("[Stopped: %d]", WSTOPSIG(state));
    }
    if(WIFCONTINUED(state)) {
        printf("[Continued]");
    }
}

void printJobList() {
    pidlist_node* prev = NULL, *current = joblist.head;
    while(current != NULL) {
        int state;

        printf("%d: %s ", current->pid, current->name);
        pid_t state_changed = waitpid(current->pid, &state,
                                      WNOHANG | WUNTRACED | WCONTINUED);
        if(state_changed) {
            pprint_state(state);
        }
        printf("\n");

        /* remove a job from the list if it has terminated */
        if(state_changed && (WIFEXITED(state) || WIFSIGNALED(state))) {
            pidlist_node* job = current;
            current = current->next;
            removeJob(job, prev);
        } else {
            prev = current;
            current = current->next;
        }

    }
}

void builtInCmd(int command, char* string, char* variable){
	char cwd[1024];
	switch(command){
		case(BYE):
            killJobs();
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

void sub_vars(char* str) {
#ifdef KERNEL_SUPPORT
    char varname[LIMIT], varval[LIMIT];
    varname[0] = '\0';
    while(syscall(__NR_NextVariable, varname, varname, LIMIT, varval, LIMIT)) {
        strsubst(str, var->variable, var->value);
    }
#else
    VARLIST *var = varlist;
    while(var != NULL) {
        strsubst(str, var->variable, var->value);
        var = var->next;
    }
#endif
}

char** build_argv(ARGLIST* arglist) {
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
        sub_vars(argv[i]);
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

  /* read the output into a variable */
  read(fd[0], output, LIMIT);
  output[LIMIT-1] = '\0';
  addToVarList(varname, output);
  free(argv);
}

void runCommand(ARGLIST* arglist, int background) {
	pid_t pid;
	int state;

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
    if(background) {
        addToJobList(pid, argv[0]);
    } else {
        if(waitpid(pid, &state, 0) < 0){
            perror("WAITPID");
            kill(pid, SIGKILL);
        }
    }
	free(argv);
}

void printCommandPrompt(void){
    char stval[LIMIT];

    if(getVar("$ShowTokens", stval, LIMIT) &&
       strncmp("1", stval, LIMIT) == 0) {
        printTokenList();
    }
    clearTokenList();
	printf("%s", prompt);
}
