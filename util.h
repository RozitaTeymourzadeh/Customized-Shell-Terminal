/**
 * @file
 *
 * Utility function to handle shell functionality
 * Author: Rozita Teymourzadeh, Mathew Malensek
 * Date: 2020
 * 
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdbool.h>
/**
 * struct command_line
 * 
 * command line data structure
 * 
  */
struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdout_file;
};

/*
 * execute_pipeline()
 * 
 * To execute pipeline command |
 * 
 * @ param cmds struct command_line *cmds - command struct
 * @ return status
  */
  
int execute_pipeline(struct command_line *cmds);

/**
 * env_var()
 * 
 * Expands environment variables 
 * Environment variable : identified by $ prefix, e.g., $SHELL
 * Expand environment variable in a string by resizing and inserting the value of the variable into the string.
 *
 * @ param str - The string with variables to expand
 * @ return char pointer to the newly-expanded and allocated string. 
  */
// char *env_var(char *str);


/**
 * cd_cmd()
 * cd linux command
 * @param tokens String array
 * @return status code - On success, zero is returned.  On error, -1 is returned
  */
int cd_cmd(char *tokens[]);

/**
 * env_cmd()
 *
 * Set envirment variable
 *
 * @param tokens    string array after tokenization
 * @param token_ptr total number of tokens in the array
 * @return status code - On success, zero is returned.  On error, -1 is returned
  */
int env_cmd(char *tokens[], int *token_ptr);

#endif