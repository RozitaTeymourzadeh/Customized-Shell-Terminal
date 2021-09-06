/**
 * @file
 *
 * Utility function to handle shell functionality
 * Author: Rozita Teymourzadeh, Mathew Malensek
 * Date: 2020
 * 
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>


#include "util.h"

/**
 * cd_cmd()
 * cd linux command
 * @param tokens String array
 * @return status code - On success, zero is returned.  On error, -1 is returned
  */
int cd_cmd(char *tokens[]) {
	char *homedir = getenv("HOME");
    int ret = -2;
	if(tokens[1] == NULL){
		ret = chdir(homedir);
        if (ret == -1){
            //printf("chdir: no such file or diretory: %s\n", tokens[1]);
        } 
	} else {
		ret = chdir(tokens[1]);
        if (ret == -1){
            //printf("chdir: no such file or diretory: %s\n", tokens[1]);
        } 
	}
    return ret;
}

/**
 * env_var()
 * 
 * Expands environment variables 
 * Environment variable : identified by $ prefix, e.g., $SHELL
 * Expand environment variable in a string by resizing and inserting the value of the variable into the string.
 *
 * @param char *str - The string with variables to expand
 * @return char pointer to the newly-expanded and allocated string. 
  */
int env_cmd(char *tokens[], int *token_ptr) {
    int ret = -2;
	if(*token_ptr == 3) {
		ret = setenv(tokens[1], tokens[2], 1);
        if (ret == -1){
            printf("env_cmd: error in memory or null pointer: %s\n", tokens[2]);
        } 
	}
    return ret;
}

/**
 * execute_pipeline()
 * 
 * To execute pipeline command |
 * 
 * @ param cmds struct command_line *cmds - command struct
 * @ return status
  */
int execute_pipeline(struct command_line *cmds) {
	int status = 0; 
    if (!cmds->stdout_pipe) {  
        if (cmds->stdout_file != NULL) {
            int fd = open(cmds->stdout_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd == -1) {
                perror("open file");
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (execvp(cmds->tokens[0], cmds->tokens) == -1) {
        	//printf(cmds->tokens[0]);
            status = -1;
            //perror("execvp");
            //printf("crash: no such file or diretory: %s\n", cmds->tokens[0]);
        }
        return status;
    } else {
        int fd[2];
        if (pipe(fd) == -1) {
            perror("pipe");
            return status;
        }
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
        } else if (pid == 0) {
            /* Child */
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            if (execvp(cmds->tokens[0], cmds->tokens) == -1) {
            //printf(cmds->tokens[0]);
            status = -1;
            //perror("execvp");
            //printf("crash: no such file or diretory: %s\n", cmds->tokens[0]);
            }
            close(fd[1]);
        } else {
            /* Parent */
            dup2(fd[0], STDIN_FILENO);
            close(fd[1]);
            execute_pipeline(cmds + 1);
            close(fd[0]);
        }
    }
    fflush(stdout);
    return status;
}

