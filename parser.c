/**
 * @file
 *
 * Utility function to handle shell functionality
 
 * 
 */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


/**
 * line_parser()
 *
 * Parse the command line into array of strings
 *
 * @param line        line of command
 * @param tokens      string array
 * @param pipe_ptr    num of pipe
 * @param token_ptr   num of strings in array
 * @param redirection boolean, whether need redirection or not 
 * @param background  boolean, whether need background job or not
 * @param ampersand   index of the '&'
  */
char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    char quotes[] = "\'\"";
    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);
    size_t quote_start = strcspn(*str_ptr, quotes);

    if (quote_start < tok_end){
        quotes[0] = *(*str_ptr + quote_start);
        quotes[1] = '\0';
        size_t offset = quote_start + 1;
        size_t quote_end = strcspn(*str_ptr + offset, quotes) + offset;
        tok_end = strcspn(*str_ptr + quote_end, delim) + quote_end - tok_start;
    }
    if (tok_end  == 0) { /* Zero length token. endof tokenizing. */
        *str_ptr = NULL;
        return NULL;
    }
    char *current_ptr = *str_ptr + tok_start; 
    *str_ptr += tok_start + tok_end; 
    if (**str_ptr == '\0') { 
        *str_ptr = NULL;
    } else { 
        **str_ptr = '\0';
        (*str_ptr)++;
    }
    return current_ptr;
}

/**
 * next_token()
 * 
 * To retrieves the next token from a string.
 *
 * 
 * @param str_ptr: maintains context in the string.
 * @param delim: the set of characters to use as delimiters
 * @returns char pointer to the next token in the string
  */
void line_parser(char *line, char *tokens[], int *pipe_ptr, int *token_ptr, bool *redirection, bool *background, int *ampersand) {

	char *next_tok = line;
	char *curr_tok;
	int i = 0;
	int max_token_no = 4095;

	while(i < max_token_no && ((curr_tok = next_token(&next_tok, " \t\r\n\"\'")) != NULL)) {
		if(strncmp(curr_tok, "#", 1) == 0) {
			tokens[i] = (char *) 0;
			break;
		} else if(strcmp(curr_tok, "|") == 0) {
			(*pipe_ptr)++;
			tokens[i] = (char *) 0;
		} else if(strcmp(curr_tok, ">") == 0) {
			*redirection = true;
			tokens[i] = (char *) 0;
		} else {
			tokens[i] = curr_tok;
		}
		if(strncmp(curr_tok, "$", 1) == 0) {
			*ampersand = i;
		}
		i++;
	}
	tokens[i] = (char *) 0;
	(*token_ptr) = i;
	if (i > 0 && strcmp(tokens[i-1], "&") == 0) {
		*background = true;
		tokens[i-1] = (char *) 0;
		(*token_ptr) = i - 1;
	}
}