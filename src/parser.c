#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

#define TOKEN_BUFSIZE 64
#define SPACE_KIND_THINGS (c == ' ' || c == '\t' || c == '\n')

char **parse_line(char *line) {
	int bufsize = TOKEN_BUFSIZE;
	int position = 0;

	char **tokens = malloc(bufsize * sizeof(char*));

	if(!tokens){
		fprintf(stderr, "tuffshell: memory error!");
		exit(EXIT_FAILURE);
	}

	int in_single_quote = 0, in_double_quote = 0;
	char *read_ptr = line;
	char *write_ptr = line;
	char *token_start = NULL;

	while(*read_ptr != '\0'){
		char c = *read_ptr;

		if(!token_start && SPACE_KIND_THINGS){
			read_ptr++;
			continue;
		}
		
		if(SPACE_KIND_THINGS && !in_single_quote && !in_double_quote){
			*write_ptr = '\0';

			if(*token_start == '$'){
				char *env_value = getenv(token_start + 1);
				tokens[position] = (env_value != NULL) ? env_value : "";
			}
			else{
				tokens[position] = token_start;
			}
			position++;

			if(position >= bufsize){
				bufsize += TOKEN_BUFSIZE;
				tokens = realloc(tokens, bufsize * sizeof(char*));
			}

			token_start = NULL;
			write_ptr++;
		}

		else if(c == '\'' && !in_double_quote){
			in_single_quote = !in_single_quote;
			
			if(!token_start){
				token_start = write_ptr;
			}	
		}

		else if(c == '\"' && !in_single_quote){
			in_double_quote = !in_double_quote;

			if(!token_start){
				token_start = write_ptr;
			}
		}

		else{
			if(!token_start){
				token_start = write_ptr;	
			}
			*write_ptr = c;
			write_ptr++;
		}

		read_ptr++;
	}

	if(token_start){
		*write_ptr = '\0';
		if(token_start[0] == '$'){
			char *env_value = getenv(token_start + 1);
			tokens[position] = (env_value != NULL) ? env_value : "";
		}
		else{
			tokens[position] = token_start;
		}
		position++;
	}

	tokens[position] = NULL;

	return tokens;
}
