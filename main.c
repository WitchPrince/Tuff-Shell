#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "makros.h"
#include "src/parser.h"
#include "src/executor.h"

void load_config(){
	char *home = getenv("HOME");
	if(home == NULL){
		return;
	}
	
	char config_path[1024];
	snprintf(config_path, sizeof(config_path), "%s/.config/tuffshell/tuff-conf", home);

	FILE *file = fopen(config_path, "r");
	if(file == NULL){
		return;
	}

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char **args;

	while((read = getline(&line, &len, file)) != -1){
		args = parse_line(line);
		if(args[0] != NULL){
			launch(args);
		}
		free(args);
	}

	free(line);
	fclose(file);
}

void shell_loop() {
	char *line = NULL;
	char **args;
	char cwd[1024];
	char prompt[2048];

	do{
		if(getcwd(cwd, sizeof(cwd)) != NULL){
			snprintf(prompt, sizeof(prompt), "%s%s\n%stuffshell> %s", PROMPT_COLOR_CYAN, cwd, PROMPT_COLOR_GREEN, PROMPT_COLOR_RESET);
		}
		else{
			snprintf(prompt, sizeof(prompt), "%stuffshell> %s", PROMPT_COLOR_GREEN, PROMPT_COLOR_RESET);
		}

		line = readline(prompt);

		if(line == NULL){
			printf("\n");
			break;
		}

		if(line[0] != '\0'){
			add_history(line);
		}

		args = parse_line(line);
		launch(args);
		printf("\n");

		free(args);
		free(line);
	
	} while(1);
}

int main() {
	load_config();
	shell_loop();
	return EXIT_SUCCESS;
}
