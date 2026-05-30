#include "builtins.h"
#include "executor.h"
#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int shell_export(char **args){
	if(args[1] == NULL){
		fprintf(stderr, "tuffshell: 'export' komutu arguman bekliyor, syntax: 'export VARIABLE=VALUE'\n");
		return 1;
	}

	char *equal_sign = strchr(args[1], '=');

	if(equal_sign != NULL){
		*equal_sign = '\0';

		char *name = args[1];
		char *value = equal_sign + 1;

		if(setenv(name, value, 1) != 0){
			perror("tuffshell export error");
		}
		
	}
	else{
		fprintf(stderr, "tuffshell: export formati hatali. '=' isareti bulunamadi.\n");
	}
	return 1;
}

int cd(char **args){
	char *home = getenv("HOME");
	char path[1024];
	
	if(args[1] == NULL){
		snprintf(path, sizeof(path), "%s", home);
		if(chdir(path) != 0){
			perror("tuffshell");
		}
	}

	else{
		char raw_path[1024];
		raw_path[0] = '\0';

		int i = 1;

		while(args[i] != NULL){
			int j = 0;	

			while(args[i][j] != '\0'){
				if(args[i][j] != '\\'){
					int len = strlen(raw_path);
					raw_path[len] = args[i][j];
					raw_path[len+1] = '\0';
				}
				j++;
			}

			if(args[i+1] != NULL){
				strcat(raw_path, " ");
			}	
			i++;
		}

		if(raw_path[0] == '~'){
			snprintf(path, sizeof(path), "%s%s", home, raw_path + 1);
		}
		else{
			snprintf(path, sizeof(path), raw_path);
		}

		if(chdir(path) != 0){
			perror("tuffshell");
		}

	}

	return 1;
}

int shell_exit(char **args){
	(void)args;
	return 0;
}

int shell_function(char **args){
	char *home = getenv("HOME");
	char func_path[1024];

	if(strcmp(args[1], "list") == 0){
		snprintf(func_path, sizeof(func_path), "ls %s/.config/tuffshell/functions", home);
		char **args2 = parse_line(func_path);
		launch(args2);
		return 1;
	}

	if(strcmp(args[1], "rm") == 0){
		snprintf(func_path, sizeof(func_path), "%s/.config/tuffshell/functions/%s.tuff", home, args[2]);
		FILE *func = fopen(func_path, "r");
		if(func == NULL){
			fprintf(stderr, "Silinmeye çalışılan fonksiyon mevcut değil!");
			return -1;
		}

		remove(func_path);
		printf("Function successfully deleted!");
	}

	else{
		snprintf(func_path, sizeof(func_path), "%s/.config/tuffshell/functions/%s.tuff", home, args[1]);
		FILE *func = fopen(func_path, "r");

		if(func != NULL){
			fprintf(stderr, "A function named '%s' already exist!", args[1]);
			return -1;
		}

		func = fopen(func_path, "w");
		if(func == NULL){
			fprintf(stderr, "Function file couldn't created!");
			return -1;
		}

		int i = 2;
		while(args[i] != NULL){
			if(strcmp(args[i], "end") == 0) break;

			fprintf(func, "%s ", args[i]);
			i++;
		}
		
		printf("Function created! You can use it from now on!");
		fclose(func);
	}


	return 1;
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

	if(strcmp(args[0], "export") == 0){
		return shell_export(args);
	}

	if(strcmp(args[0], "function") == 0){
		return shell_function(args);
	}

	return -1;
}
