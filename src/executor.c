#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "executor.h"
#include "builtins.h"

int launch(char **args){
	pid_t pid, wpid;
	int status;
	int builtin_status;

	if(args[0] == NULL){
		return 1;
	}

	builtin_status = execute_builtin(args);
	if(builtin_status != -1){
		return builtin_status;
	}

	pid = fork();

	if(pid == 0){
		if(execvp(args[0], args) == -1){
			perror("tuffshell");
		}	

		exit(EXIT_FAILURE);
	}

	else if(pid < 0){
		perror("tuffshell");
	}

	else{
		do{
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}
