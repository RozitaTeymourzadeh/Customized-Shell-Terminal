/**
 * @file
 *
 * Utility function to handle shell functionality
 * 
 */
#include <stdio.h>
#include <readline/readline.h>
#include <locale.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "user.h"
#include "util.h"
#include "parser.h"

/* -- Private function forward declarations -- */
static int readline_init(void);
static int key_up(int count, int key);
static int key_down(int count, int key);
static char **command_completion(const char *text, int start, int end);
char *command_generator(const char *text, int state);


static char prompt_str1[80] = "";
static char prompt_str2[80] = "-->";
static char *character_names[100] = {"history", "jobs", "exit", "cd"};
static unsigned int cmd_no_keys; 

int next_match_index_up = 0;
char current_dir[PATH_MAX];
char short_path[PATH_MAX];
bool found = false;
unsigned int match_index_in = 0;
unsigned int last_command_no = 0;

DIR *directory;
static char *sub_path = NULL;
static char *ver_tok = NULL;
static char* pPath = NULL;
static int list_index;



/**
 * init_ui
 *
 * To initialise user interface
 * 
 * @ param void
 * @ return void
  */
void init_ui(void)
{
    LOGP("Initializing UI...\n");


    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");

    rl_startup_hook = readline_init;
    get_cmd_no ();
    get_user();
    get_hostname();
    get_home_dir();
    set_emoji(HAPPY); 
    prompt_line1();
}

/** set_key_cmd_no
* to set command number
* @ param new_cmd_no command number
* @ return void
 */
void set_key_cmd_no (unsigned int new_cmd_no)
{
  cmd_no_keys = new_cmd_no;
}

/** get_key_cmd_no
* to get cmd_no
* @ param void
* @ return cmd_no
 */
unsigned int get_key_cmd_no (void)
{
  return cmd_no_keys;
}

/**
 * prompt_line1
 *
 * To provide second line of the command line
 * 
 * @ param void
 * @ return void
  */
char *prompt_line1(void) 
{   
    print_prompt();
    return prompt_str1;
}

/**
 * prompt_line2
 *
 * To provide first line of the command line
 * 
 * @ param void
 * @ return void
  */
char *prompt_line2(void) 
{
    return prompt_str2;
}

/**
 * read_command
 *
 * To provide 2 lines of command printed for shell
 * 
 * @ param void
 * @ return void
  */
char *read_command()
{
    puts(prompt_line1());
    return readline(prompt_line2());
}

/**
 * readline_init
 *
 * To find key changes (up/down) and tab completion
 * 
 * @ param void
 * @ return command
  */
int readline_init(void)
{
    rl_bind_keyseq("\\e[A", key_up);
    rl_bind_keyseq("\\e[B", key_down);
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    rl_attempted_completion_function = command_completion;
    return 0;
}

/**
 * key_up
 *
 * To find key changes (up)
 * 
 * @ param count
 * @ param key
 * @ return int
  */
int key_up(int count, int key)
{
    /* Modify the command entry text: */
    char  *line = NULL;
    const char* const_cmd = NULL;
    cmd_no_keys = get_key_cmd_no();
    if(cmd_no_keys == 0){
        if(hist_last_cnum() == 0){
            printf("No history is available!");
            return 0;
        }
        unsigned int temp = hist_last_cnum() + 1;
        set_key_cmd_no(temp);
        cmd_no_keys = get_key_cmd_no();
        LOG("only once: '%u'\n", cmd_no_keys);
    }



    if (strlen(rl_line_buffer) != 0){
        //LOG("rl_line_buffer: %s\n", rl_line_buffer);
        if (match_index_in == 0){
            match_index_in = hist_last_cnum();
        }
        line = rl_line_buffer;
        size_t loc = strcspn(line, "\n");
        line[loc] = '\0';
        //LOG("line: %s\n", line);
        unsigned int match_index_out = hist_search_downkey(match_index_in, line);
        if (match_index_out != 0)
        {
            //LOG("match_index_out: %d\n", match_index_out);
            set_key_cmd_no(match_index_out+1);
            LOG("Match happend!! new cmd_no_key is: %u\n", cmd_no_keys);
            cmd_no_keys = match_index_out;
            match_index_in = match_index_out;
        }
    }


    cmd_no_keys = cmd_no_keys - 1;
    //LOG("--: '%d'\n", cmd_no_keys);
    if (cmd_no_keys <= hist_last_cnum() && cmd_no_keys > 1){
        set_key_cmd_no(cmd_no_keys);
        //LOG("--: '%d'\n", cmd_no_keys);
    } else if (cmd_no_keys <= 1) {
        set_key_cmd_no(1);
        //LOG("set to Min: '%d'\n", cmd_no_keys);
    } else if (cmd_no_keys > hist_last_cnum()){
        set_key_cmd_no(hist_last_cnum());
        //LOG("set to max: '%d'\n", cmd_no_keys);
    }
    const_cmd = hist_search_cnum(get_key_cmd_no());
    char *fix = strdup(const_cmd);
    size_t newline = strcspn(fix, "\n");
    fix[newline] = '\0';
    LOG("Final conmmand no for this up key: '%u' , '%s'\n", get_key_cmd_no(), fix);
    /* Modify the command entry text: */ //"User pressed 'down' key"
    rl_replace_line(fix, 1);
    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    return 0;
}

/**
 * key_down
 *
 * To find key changes (down)
 * 
 * @ param count
 * @ param key
 * @ return int
  */
int key_down(int count, int key)
{
    char  *line = NULL;
    const char* const_cmd = NULL;
    cmd_no_keys = get_key_cmd_no();
    if(cmd_no_keys == 0){
        if(hist_last_cnum() == 0){
            printf("No history is available!");
            return 0;
        }
        unsigned int temp = hist_last_cnum() - 1;
        set_key_cmd_no(temp);
        cmd_no_keys = get_key_cmd_no();
        LOG("only once: '%u'\n", cmd_no_keys);
    }

    if (strlen(rl_line_buffer) != 0){
        //LOG("rl_line_buffer: %s\n", rl_line_buffer);
        if (match_index_in == 0){
            match_index_in = hist_last_cnum();
        }
        line = rl_line_buffer;
        size_t loc = strcspn(line, "\n");
        line[loc] = '\0';
         //LOG("line: %s\n", line);
        unsigned int match_index_out = hist_search_downkey(match_index_in, line);
        if (match_index_out != 0)
        {
            //LOG("match_index_out: %d\n", match_index_out);
            set_key_cmd_no(match_index_out-1);
            LOG("Match happend!! new cmd_no_key is: %u\n", cmd_no_keys);
            cmd_no_keys = match_index_out;
            match_index_in = match_index_out;
        }
    }

    
    cmd_no_keys = cmd_no_keys + 1;
    LOG("++: '%u'\n", cmd_no_keys);
    if (cmd_no_keys <= hist_last_cnum() && cmd_no_keys > 1){
        set_key_cmd_no(cmd_no_keys);
        //LOG("++: '%d'\n", cmd_no_keys);
    } else if (cmd_no_keys <= 1) {
        set_key_cmd_no(1);
        //LOG("set to min: '%d'\n", cmd_no_keys);
    } else if (cmd_no_keys > hist_last_cnum()){
        set_key_cmd_no(hist_last_cnum());
        //LOG("set to max: '%d'\n", cmd_no_keys);
    }
    const_cmd = hist_search_cnum(get_key_cmd_no());
    char *fix = strdup(const_cmd);
    size_t newline = strcspn(fix, "\n");
    fix[newline] = '\0';
    LOG("Final conmmand no for this down key: '%u' , '%s'\n", get_key_cmd_no(), fix);
    /* Modify the command entry text: */ //"User pressed 'down' key"
    rl_replace_line(fix, 1);
    /* Move the cursor to the end of the line: */
    rl_point = rl_end;
    return 0;
}

/**
 * command_completion
 *
 * To find key changes (down)
 * 
 * @ param text for tab completion
 * @ param start of the command
 * @ return end of command
  */
char **command_completion(const char *text, int start, int end)
{
    /* Tell readline that if we don't find a suitable completion, it should fall
     * back on its built-in filename completion. */
    rl_attempted_completion_over = 0;

    return rl_completion_matches(text, command_generator);
}

/** next_autocmp_dir
*  To open the file for autocompletion.
* @ param void
* @ return bool
 */
bool next_autocmp_dir(void)
{
    while ((sub_path = next_token(&ver_tok, ":")) != NULL) {
        LOG("sub: %s\n", sub_path);
        if ((directory = opendir(sub_path)) == NULL) {
            perror("opendir");
            continue;
        } else {
            return true;
        }
    }
    return false;
}

/** builtin_autocomplete
*  Builtin command like job, cd, history and exit to be used for auto completion.
* @ param const char *text
* @ return char
 */
char *builtin_autocomplete(const char *text, int state)
{
    for ( ; list_index < 4; ++list_index) {
        if (strncmp(text, character_names[list_index], strlen(text)) == 0) {
            return strdup(character_names[list_index++]);
        }
    }
    return NULL;
} 


/**
 * This function is called repeatedly by the readline library to build a list of
 * possible completions. It returns one match per function call. Once there are
 * no more completions available, it returns NULL.
 */
char *command_generator(const char *text, int state)
{
    if (state == 0) {
        directory = NULL;
        char *path = getenv("PATH");
        if (path == NULL) {
            return NULL;
        }
        ver_tok = strdup(path);
        pPath = ver_tok;
        list_index = 0;
    }

    char *builtin = builtin_autocomplete(text, state);
    if (builtin != NULL) {
        return builtin;
    }

    while (true) {
        if (directory == NULL) {
            if (next_autocmp_dir() == false) {
                break;
            }
        }

		struct dirent *entry;
		while ((entry = readdir(directory)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 
                    || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            if (strncmp(text, entry->d_name, strlen(text)) == 0) {
                return strdup(entry->d_name);
            }
		}

		closedir(directory);
        directory = NULL;
    }

    free(ver_tok);
    return NULL;
}



/**
 * Prser_home 
 * Parses the home directory to find the name of the sub-directory. 
 * Needed for printing the prompt
 * @ param line - path to parse
 * @ param void
  */
void parse_home(char *line) {
    if (strstr(line, USERNAME)) {
        int size = strlen(line);
        int temp = size - strlen(strstr(line, USERNAME));
        int start = strcspn(strstr(line, USERNAME), "/") + temp;

        for (int i = start; i < size; i++) {
            int len = strlen(short_path);
            short_path[len] = line[i];
            short_path[len + 1] = '\0';
        }
    }
}

/**
 * check_path 
 * Checks to see if the path is in the home directory
 * @ param void
 * @ param void
  */
void check_path() {
    found = false;
    char full_path[PATH_MAX];
    memset(full_path, 0, PATH_MAX);
    strcpy(full_path, getcwd(current_dir, sizeof(current_dir)));

    DIR *dir = opendir(getcwd(current_dir, sizeof(current_dir)));

    if (dir != NULL) {
        if (strncmp(full_path, HOME, strlen(HOME)) == 0) {
            found = true;
            memset(short_path, 0, PATH_MAX);
            parse_home(full_path);
        }
    }
    closedir(dir);
}


/** print_prompt
* function to print the starter command
* @ param void
* @ return void
 */
void print_prompt(void) {
    char *buf = calloc(80, sizeof(char));
    //static char buf[80] = {0};
    found = false;
    check_path();


    if (found){
        //memset(&buf[0], 0, sizeof(buf));
        //printf(buf,"[%s]-[%d]-[%s@%s:~%s]\n ", get_emoji(), get_cmd_no(), USERNAME, HOSTNAME, short_path);
        strcat(buf, "[");
        strcat(buf, get_emoji());
        strcat(buf, "]-");
        strcat(buf, "\0");

        char no_str[10];
        sprintf(no_str,"%d", get_cmd_no());
        strcat(buf, "[");
        strcat(buf, no_str);
        strcat(buf, "]-");
        strcat(buf, "\0");

        strcat(buf, "[");
        strcat(buf, USERNAME);
        strcat(buf, "@");
        strcat(buf, "\0");

        strcat(buf, HOSTNAME);
        strcat(buf, ":~");
        strcat(buf, "\0");

        strcat(buf, short_path);
        strcat(buf, "]");
        strcat(buf, "\0");

    } else {
        //memset(&buf[0], 0, sizeof(buf));
        getcwd(current_dir, sizeof(current_dir));
        if (strcmp(current_dir, HOME) == 0) {
            //printf("[%s]-[%d]-[%s@%s:~]\n ", get_emoji(), get_cmd_no(), USERNAME, HOSTNAME);
            strcat(buf, "[");
            strcat(buf, get_emoji());
            strcat(buf, "]-");
            strcat(buf, "\0");

            char no_str[10];
            sprintf(no_str,"%d", get_cmd_no());
            strcat(buf, "[");
            strcat(buf, no_str);
            strcat(buf, "]-");
            strcat(buf, "\0");

            strcat(buf, "[");
            strcat(buf, USERNAME);
            strcat(buf, "@");
            strcat(buf, "\0");

            strcat(buf, HOSTNAME);
            strcat(buf, ":~]");
            strcat(buf, "\0");

        } else {
            //memset(&buf[0], 0, sizeof(buf));
            //printf("[%s]-[%d]-[%s@%s:%s]\n ", get_emoji(), get_cmd_no(), USERNAME, HOSTNAME, current_dir);
            strcat(buf, "[");
            strcat(buf, get_emoji());
            strcat(buf, "]-");
            strcat(buf, "\0");

            char no_str[10];
            sprintf(no_str,"%d", get_cmd_no());
            strcat(buf, "[");
            strcat(buf, no_str);
            strcat(buf, "]-");
            strcat(buf, "\0");

            strcat(buf, "[");
            strcat(buf, USERNAME);
            strcat(buf, "@");
            strcat(buf, "\0");

            strcat(buf, HOSTNAME);
            strcat(buf, ":");
            strcat(buf, "\0");

            strcat(buf, current_dir);
            strcat(buf, "]");
            strcat(buf, "\0");
        }
    }

    strcpy(prompt_str1, buf);
    free(buf);
 
    return;
}
