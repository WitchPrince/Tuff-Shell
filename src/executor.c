#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "executor.h"
#include "builtins.h"
#include "parser.h"

int run_tuff_functions(char **args){
	char *home = getenv("HOME");
	if(home == NULL){
		return -1;
	}

	char functions[1024];
	snprintf(functions, sizeof(functions), "%s/.config/tuffshell/functions/%s.tuff", home, args[0]);
	FILE *file = fopen(functions, "r");

	if(file == NULL) return -1;

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char **func_args;

	while((read = getline(&line, &len, file)) != EOF){
		func_args = parse_line(line);
		if(func_args[0] != NULL){
			launch(func_args);
		}
		free(func_args);
	}	

	free(line);
	fclose(file);

	return 1;
}

int launch(char **args){
	pid_t pid, wpid;
	int status;
	int builtin_status;
	int function_status;

	if(args[0] == NULL){
		return 1;
	}

	builtin_status = execute_builtin(args);
	if(builtin_status != -1){
		return builtin_status;
	}

	function_status = run_tuff_functions(args);
	if(function_status != -1){
		return function_status;
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
