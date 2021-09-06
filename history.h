/**
 * @file
 *
 * Contains shell history data structures and retrieval functions.
 * Author: Rozita Teymourzadeh, Mathew Malensek
 * Date: 2020
 */



#ifndef _HISTORY_H_
#define _HISTORY_H_

#define MAX_HISTORY_SIZE  100


/** fork_list_struct
* data structure for each process and command
* @ param void
* @ return void
 */
struct fork_list_struct {
	struct fork_list_struct *prev_fork;
    struct fork_list_struct *next_fork;
    char *cmd;
    int index;
   // int limit;

};

/** hist_init
* to initialize struct 
* @ param struct limit
* @ return struct type 
 */
struct fork_list_struct *hist_init(unsigned int limit);

/** free_fork
* clear fork data
* @ param list of the previous struct 
* @ return void
 */
void free_fork(struct fork_list_struct* fork_list_previous);

/** history_destroy
* to destroy the history
* @ param void
* @ return void
 */
void hist_destroy(void);

/** adjust_list_size()
*  Limit the history size if it exceeds the limit
* @ param void
* @ return void
 */
void adjust_list_size();

/** hist_add
* to add the command into the history
* @ param command
* @ return void
 */
void hist_add(const char *);

/** find_digit
* to find relevant command using the command history number
* @ param num_str 
* @ return void
 */
void find_digit(char *num_str, char** command);

/** find_last_command
* to find relevant last command (first version)
* @ param command
* @ return void
 */
void find_last_command_test(char **cmd, unsigned int last_cmd_no);

/** find_last_command_test
* to find relevant last command (second version)
* @ param command
* @ return void
 */
void find_last_command(char **cmd);

/** find_alphabet
* to find relevant command using the command history and prefix
* @ param prefix 
* @ return void
 */
void find_alphabet(char *prefix, char **cmd);


/** hist_print
* to print history
* @ param void 
* @ return void
 */
void hist_print(void);

/** hist_search_index_prefix
* to search and find the specific commannd
* @ param command
* @ return void
 */
int hist_search_index_prefix(char *prefix);


/** hist_search_prefix
* to search prefix and find relevant command using the command history
* @ param prefix e.g hi 
* @ return command e.g. history
 */
const char *hist_search_prefix(char *);

/** hist_search_cnum
* to give command number and find relevant command using the command history number
* @ param command number
* @ return command
 */
const char *hist_search_cnum(int);

/** hist_last_cnum
* to find relevant command number using the command history number
* and return the command number
* @ param void 
* @ return command number
 */
unsigned int hist_last_cnum(void);

/** hist_search_upkey
* to search and find the specific commannd index
* @ param match_index
* @ param prefix
* @ return void
 */
unsigned int hist_search_upkey(unsigned int match_index, char *prefix);


/** hist_search_downkey
* to search and find the specific commannd index
* @ param match_index
* @ param prefix
* @ return void
 */
unsigned int hist_search_downkey(unsigned int match_index, char *prefix);

#endif
