#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	size_t len = 0;
	ssize_t read;
	char **args;
	int status;

	do{
		args = parse_line("pwd");
		launch(args);
		printf("tuffshell> ");
		read = getline(&line, &len, stdin);

		if(read == -1) {
			printf("\n");
			break;
		}

		args = parse_line(line);
		status = launch(args);

		printf("\n");

		free(args);
	} while(status);

	free(line);
}

int main() {
	load_config();
	shell_loop();
	return EXIT_SUCCESS;
}
