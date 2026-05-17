#include <stdio.h>
#include <stdlib.h>
#include "src/parser.h"
#include "src/executor.h"
#include "src/builtins.h"

void shell_loop() {
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char **args;
	int status;

	do{
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
	shell_loop();
	return EXIT_SUCCESS;
}
