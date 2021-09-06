/**
 * @file
 *
 * parser function to handle parsing job 
 * Author: Rozita Teymourzadeh, Mathew Malensek
 * Date: 2020
 * 
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdbool.h>

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
void line_parser(char *line, char *tokens[], int *pipe_ptr, int *token_ptr, bool *redirection, bool *background, int *ampersand);

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
char *next_token(char **str_ptr, const char *delim);


#endif