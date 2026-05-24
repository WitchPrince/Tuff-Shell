#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
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

	//color activate for ls command, wasn't necessary but sometimes I'm forgetting what I saw is a file or a directory lol
	if(strcmp(args[0], "ls") == 0){
		int i = 0;

		while(args[i] != NULL){
			i++;
		}

		args[i] = "-l";
		args[i+1] = "--color=auto";
		args[i+2] = NULL;
	}

	builtin_status = execute_builtin(args);
	if(builtin_status != -1){
		return builtin_status;
	}

	function_status = run_tuff_functions(args);
	if(function_status != -1){
		return function_status;
	}

	int i = 0;
	int pipe_index = -1;
	while(args[i] != NULL){
		if(strcmp(args[i], "|") == 0){
			pipe_index = i;
			break;
		}
		i++;
	}

	if(pipe_index != -1){
		args[pipe_index] = NULL;
		char **args2 = &args[pipe_index + 1];

		int pipefd[2];
		if(pipe(pipefd) == -1){
			perror("tuffshell pipe hatasi");
			return 1;
		}

		pid_t pid1, pid2;
		pid1 = fork();

		if(pid1 == 0){
			signal(SIGINT, SIG_DFL);
			signal(SIGTSTP, SIG_DFL);

			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);

			if(execvp(args[0], args) == -1){
				perror("tuffshell komut 1");
				exit(EXIT_FAILURE);
			}
		}

		pid2 = fork();
		if(pid2 == 0){
			signal(SIGINT, SIG_DFL);
			signal(SIGTSTP, SIG_DFL);

			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);

			if(execvp(args2[0], args2) == -1){
				perror("tuffshell komut 2");
				exit(EXIT_FAILURE);
			}
		}

		close(pipefd[0]);
		close(pipefd[1]);

		waitpid(pid1, NULL, 0);
		waitpid(pid2, NULL, 0);

		return 1;
	}

	pid = fork();

	if(pid == 0){
		signal(SIGINT, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);

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
