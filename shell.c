/**
 * @file
 *
 * Shell driver to handle shell functionality
 * 
 */
#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <assert.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"
#include "user.h"
#include "parser.h"

/* pre-defined value */
#define JOB_LIST_SIZE 10
#define BUFF 256

/* variable */
pid_t   pid_list[JOB_LIST_SIZE];
int     list_index;
char    *line_list[JOB_LIST_SIZE];
bool    command_exec;
int     command_no = 1;

char    username[LOGIN_NAME_MAX];
char    hostname[HOST_NAME_MAX];
char    cwd[PATH_MAX];

/**
* working_directory_cwd()
*
* Find project directory
* Convert CWD from full path to full path - home/username
* @ param: void
* @ return void 
 */
void working_directory_cwd() {
    int count = 0;
    int i;
    char*p;
    getcwd(cwd, PATH_MAX);
    for(i = 0; i < strlen(cwd); i++) {
        if(cwd[i] == '/') {
            count++;
        }
        if(count == 3) {
            p = cwd+i;
            strcpy(cwd, p);
            //LOG("CWD is %s\n", cwd);
            break;
        }
    }
}

/**
 * forward_job(int index) 
 * To forward behind task to infront after a background job filished
 * @ param index the completed job
 * @ param void
  */
void forward_job(int index) {
    for(; index < list_index; index++) {
        pid_list[index] = pid_list[index+1];
        line_list[index] = line_list[index+1];
    }
}

/** print_jobs
* function to print listed print job
* @ param void
* @ return void
 */
int print_jobs() {
    int ret = 0; 
    if (list_index == 0){
        ret = -1;
        return ret;
    }
    int i;
    for(i = 0; i < list_index; i++) {
        printf("%d %s\n", pid_list[i], line_list[i]);
    }
    return ret;
}

/**
 * sigint_handler
 *
 * Handler for sigint handler
 * @ param signal number
 * @ return void
 *
  */
void sigint_handler(int signo) {
    /* prompt information - library */
    set_emoji(POUTING);
    getlogin_r(username, LOGIN_NAME_MAX);
    gethostname(hostname, HOST_NAME_MAX);
    getcwd(cwd, PATH_MAX);
    /* Remove /home/user/ */
    working_directory_cwd();
    if(isatty(STDIN_FILENO)) {
        printf("\n");
        fflush(stdout);
        if(!command_exec) {
            read_command();
        }
    }
}

/**
 * sigchild_handler(int signo) 
 *
 * Handler for child signal
 * @ param signal number
 * @ param return
  */
void sigchild_handler(int signo) {

    int status;
    pid_t child = waitpid(-1, &status, WNOHANG);

    int i;
    for(i = 0; i < list_index; i++) {
        if(pid_list[i] == child) {
            free(line_list[i]);
            forward_job(i);
            list_index--;
            break;
        }
    }
}

/**
 * sigchild_handler(int signo) 
 *
 * Handler for child signal
 * @ param signal number
 * @ param return
  */
void reset_up_down_key(){
    set_key_cmd_no (0);
}


/**
 * history() 
 * To find the command type 
 * @ param line string array of commands
 * @ Return void
  */
void history(char *line) {
    char* cmd = malloc(sizeof(char));
    /* to find digit */
    if(line[1] > '/' && line[1] < ':') {
        find_digit(line+1, &cmd);
    }
    /* to find alphabet */
    if(line[1] > '`' && line[1] < '{') {
        find_alphabet(line+1, &cmd);
    }
    /* to find !! */
    if(line[1] == '!') {
        find_last_command(&cmd);
    }
    /* cmd not found */
    if(cmd != NULL) {
        strcpy(line, cmd);
    }
    free(cmd);
}

/**
 * pipe parser 
 *
 * Handler pipe "|" parse the pipe
 * @ param list of parsed token
 * @ param number of token
 * @ param command 
 * @ param output flag 
 * @ return void
  */
void pipe_parser(char *tokens[], int token_num, struct command_line *cmds, bool output){
    int i = -1;
    int cmds_index = 0;

    char *curr_tok = NULL;
    if(!output) {
        while(i < token_num) { /*if no output */
            if(curr_tok == NULL) { /* if command is not end */
                char **cmd_tokens = tokens+(i+1);
                cmds[cmds_index].tokens = cmd_tokens;
                cmds[cmds_index].stdout_pipe = true;
                cmds[cmds_index].stdout_file = NULL;
                cmds_index++;
            }
            i++;
            curr_tok = tokens[i];
        }
        cmds[cmds_index-1].stdout_pipe = false;
    } else {
        while(i < (token_num-2)) { /* if output needed */
            if(curr_tok == NULL) { /* if command is not end */
                char **cmd_tokens = tokens+(i+1);
                cmds[cmds_index].tokens = cmd_tokens;
                cmds[cmds_index].stdout_pipe = true;
                cmds[cmds_index].stdout_file = NULL;
                cmds_index++;
            }
            i++;
            curr_tok = tokens[i];
        }
        
        cmds[cmds_index-1].stdout_pipe = false; /* change the last struct boolean to false since no more command */
        cmds[cmds_index-1].stdout_file = tokens[token_num-1]; /* Give the output file name to the last struct */
    }
}

/**
 * main 
 *
 * Shell driver 
 * @ param void
 * @ param status code 
  */
int main(void)
{
    
    /* initialization */
    init_ui();

    /* variable */
    set_emoji(HAPPY); 
    command_exec = false;

    char    *line = NULL;
    size_t  line_size = 0;

    /* Handle signal */
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchild_handler);

    /* Calculate username and path */
    get_user();
    get_hostname();
    get_home_dir();

    while (true) {
        command_exec = false;
        line = NULL;
        ssize_t command_size = 0;

        /* To support scripting and interactive mode */
        if (isatty(STDIN_FILENO)) {
            //printf("stdin is a TTY; entering interactive mode\n");
            line = read_command();
            if (line != NULL) {
                command_size = strlen(line);
                line[command_size] = '\n';
                line[command_size + 1] = '\0';
            }  else {
                break;
            }
        } else {
            command_size = getline(&line, &line_size, stdin); /* to get command size*/
            if (command_size == -1) {
                free(line);
                break;
            }
        }
        reset_up_down_key();
        /* clear command line  */
        if (line == NULL) {
            continue;
        }

        /* check for find ! and print last history */
        if(strncmp(line, "!", 1) == 0) {
            history(line);
        }
        if(strlen(line) > 1){ //\n size should be greater than 1 for /n 
            //LOG("Adding: %s\n", line);
            hist_add(line);
            fflush(stdout);
            set_cmd_no(command_no++);
        } else {
            fprintf(stderr, "crash: event not found: <NULL>\n");            
            set_emoji(POUTING);
        }
        
        char    *tokens[4096];
        int     total_pipe = 0;
        int     token_num = 0;
        bool    output = false;
        bool    background = false;
        int     ampersand = -1;
        char    *line_str;

        /* parser to pars command */
        char line_parse[strlen(line)];
        strcpy(line_parse, line);
        /* parsing line */
        line_parser(line_parse, tokens, &total_pipe, &token_num, &output, &background, &ampersand);
        
        /* No command */
        if(tokens[0] == NULL) {
            free(line);
            set_emoji(POUTING);
            continue;
        }
        struct command_line cmds[total_pipe + 1]; 
        /* parsing pipe */
        pipe_parser(tokens, token_num, cmds, output);
        
        /* setenv task */
        if(strcmp(tokens[0], "history") == 0) {
            set_emoji(BATH);
            hist_print();
            free(line);
            continue;
        }
        /* clean task */
        if(strcmp(tokens[0], "clean") == 0) {
            set_emoji(CRYING);
            hist_destroy();
            continue;
        }
        /* job task */
        if(strcmp(tokens[0], "jobs") == 0){
            set_emoji(KISSING);
            int ret = -2;
            ret = print_jobs();
            if (ret == -1){
                set_emoji(POUTING);
            }
            free(line);
            continue;
        }
        /* exit task */
        if(strcmp(tokens[0], "exit") == 0) {
            hist_destroy();
            exit(0);
        }
        /* cd task */
        if(strcmp(tokens[0], "cd") == 0) {
            set_emoji(ROLLING);
            int ret = -2;
            ret = cd_cmd(tokens);
            if (ret == -1){
                set_emoji(POUTING);
            }
            free(line);
            continue;
        }
        /* setenv task */
        if(strcmp(tokens[0], "setenv") == 0) {
            set_emoji(HEART);
            int ret = -2;
            ret = env_cmd(tokens, &token_num);
            if (ret == -1){
                set_emoji(POUTING);
            }
            free(line);
            continue;
        }
        command_exec = true;
        int status = 0;
        set_emoji(MONEY);
        pid_t child = fork();
        if (child == 0){
            /* I am child */
    
                status = execute_pipeline(cmds);
                fflush(stdout);
                fclose(stdin);
                if (status == -1){
                    fprintf(stderr, "crash: event not found:%s\n", line);
                    set_emoji(POUTING);
                }

        } else if (child == -1) {
            perror("fork");
        } else {
            /* I am parent*/
            /* if (& is NOT there) */
            if(!background){
                waitpid(child, &status, 0);
            } else {

                /*update joblist */
                pid_list[list_index] = child;
                line[strlen(line)-1] = '\0';
                line_str = malloc(strlen(line)*sizeof(char));
                strcpy(line_str, line);
                line_list[list_index] = line_str;
                list_index++;
            }
        }
        free(line);
    }
    hist_destroy();  
    return 0;
}
