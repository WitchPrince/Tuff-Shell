#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

#define TOKEN_BUFSIZE 64

char **parse_line(char *line) {
	int bufsize = TOKEN_BUFSIZE;
	int position = 0;

	char **tokens = malloc(bufsize * sizeof(char*));
	char *token;

	if(!tokens){
		fprintf(stderr, "tuffshell: bellek ayirma hatasi\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, TOKEN_DELIMETERS);

	while(token != NULL){
		tokens[position] = token;
		position++;

		if(position >= bufsize){
			bufsize += TOKEN_BUFSIZE;
			tokens = realloc(tokens, bufsize * sizeof(char*));

			if(!tokens){
				fprintf(stderr, "tuffshell: bellek ayirma hatasi\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TOKEN_DELIMETERS);
	}

	tokens[position] = NULL;
	return tokens;
}
