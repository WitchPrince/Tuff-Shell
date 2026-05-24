#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "makros.h"
#include "src/parser.h"
#include "src/executor.h"

void sigint_handler(int sig){
	(void)sig;
	printf("\n");

	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void load_start(){
	char *home = getenv("HOME");
	if(home == NULL){
		return;
	}
	
	char config_path[1024];
	snprintf(config_path, sizeof(config_path), "%s/.config/tuffshell/tuff-start", home);

	FILE *file = fopen(config_path, "r");
	if(file == NULL){
		file = fopen(config_path, "w");
		if(file == NULL){
			fprintf(stderr, "Start file isn't exit and we couldn't create a new one.");
			return;
		}

		printf("File couln't found, so we created a new fresh one! You can directly type 'tuff-config' for go to file directory.");
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

	signal(SIGINT, sigint_handler);
	signal(SIGTSTP, SIG_IGN);

	load_start();
	shell_loop();
	return EXIT_SUCCESS;
}
