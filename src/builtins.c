#include "builtins.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int cd(char **args){
	if(args[1] == NULL){
		fprintf(stderr, "tuffshell: 'cd' komutunun argumani eksik!\n");
	}

	else{
		if(chdir(args[1]) != 0){
			perror("tuffshell");
		}
	}

	return 1;
}

int shell_exit(char **args){
	(void)args;
	return 0;
}

int execute_builtin(char **args){
	if(args[0] == NULL){
		return 1;
	}

	if(strcmp(args[0], "cd") == 0){
		return cd(args);
	}

	if(strcmp(args[0], "exit") == 0){
		return shell_exit(args);
	}

	return -1;
}
