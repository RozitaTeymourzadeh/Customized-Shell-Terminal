/**
 * @file
 *
 * Utility function to handle history
 * Author: Rozita Teymourzadeh, Mathew Malensek
 * Date: 2020
 * 
  */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "history.h"


struct fork_list_struct *nexth_fork = NULL;
struct fork_list_struct *prevt_fork = NULL;
unsigned int list_size = 0;


/** hist_init
* to initialize struct 
* @ param struct limit
* @ return struct type 
 */
/* Checked */
struct fork_list_struct *hist_init(unsigned int limit) 
{
    // TODO: set up history data structures, with 'limit' being the maximum
    // number of entries maintained.
    struct fork_list_struct *new_fork = (struct fork_list_struct*)malloc(sizeof(struct fork_list_struct));
    new_fork->next_fork = NULL;
    new_fork->prev_fork = NULL;
    new_fork->index = list_size+1;
    new_fork->cmd = malloc((limit + 1)*sizeof(char));

  return new_fork;
}


/** free_fork
* clear fork data
* @ param list of the previous struct 
* @ return void
 */
/* Checked */
void free_fork(struct fork_list_struct* fork_list_previous) {
  free(fork_list_previous->cmd);
  free(fork_list_previous);
}

/** history_destroy
* to destroy the history
* @ param void
* @ return void
 */
/* Checked */
void hist_destroy(void)
{
  struct fork_list_struct* fork_p = nexth_fork;
  nexth_fork = NULL;
  prevt_fork = NULL;

  while(fork_p != NULL){
    struct fork_list_struct* tmp = fork_p;
    fork_p = fork_p->next_fork;
    if(tmp->next_fork != NULL)
        tmp->next_fork->prev_fork = tmp->prev_fork;
    tmp->next_fork = NULL;
    free_fork(tmp);
  }
}

/** adjust_list_size()
*  Limit the history size if it exceeds the limit
* @ param void
* @ return void
 */
/* Checked */
void adjust_list_size() {
  struct fork_list_struct *temp = prevt_fork;
  temp->prev_fork->next_fork = NULL;
  prevt_fork = temp->prev_fork;
  free_fork(temp);
}

/** hist_add
* to add the command into the history
* @ param command
* @ return void
 */
/* Checked */
void hist_add(const char *cmd)
{    
  if(nexth_fork == prevt_fork && nexth_fork == NULL) {
    struct fork_list_struct* new_fork = hist_init((unsigned int)strlen(cmd));
    strcpy(new_fork->cmd, cmd);
    nexth_fork = new_fork;
    prevt_fork = new_fork;
    list_size++;
    fflush(stdout);
    return;
  } else {
    struct fork_list_struct* new_fork = hist_init((unsigned int)strlen(cmd));
    strcpy(new_fork->cmd, cmd);
    new_fork->next_fork = nexth_fork;
    nexth_fork->prev_fork = new_fork;
    nexth_fork = new_fork;
    list_size++;
    if(list_size > MAX_HISTORY_SIZE) {
      adjust_list_size();
    }
    fflush(stdout);
    return;
  }
}

/** find_digit
* to find relevant command using the command history number
* @ param num_str 
* @ return void
 */
/* Checked */
void find_digit(char *num_str, char** cmd){
  struct fork_list_struct* curr_fork = nexth_fork;
  int num = atoi(num_str);
  while(curr_fork != NULL) {
    if(curr_fork->index == num) {
      *cmd = realloc(*cmd, strlen(curr_fork->cmd)*sizeof(char));
      strcpy(*cmd, curr_fork->cmd);
      return;
    }
    curr_fork = curr_fork->next_fork;
  }
}

/** find_last_command_test
* to find relevant last command (second version)
* @ param cmd
* @ return void
 */
/* Checked */
void find_last_command(char **cmd) {
  *cmd = realloc(*cmd, strlen(nexth_fork->cmd)*sizeof(char));
  strcpy(*cmd, nexth_fork->cmd);
  return;
}

/** find_alphabet
* to find relevant command using the command history and prefix
* @ param prefix 
* @ return void
 */
/* Checked */
void find_alphabet(char *prefix, char **cmd){
  struct fork_list_struct* curr_fork = nexth_fork;
  while (curr_fork != NULL) {
    if(strncmp(curr_fork->cmd, prefix, strlen(prefix)-1) == 0) {
      *cmd = realloc(*cmd, strlen(curr_fork->cmd)*sizeof(char));
      strcpy(*cmd, curr_fork->cmd);
      return;
    }
    curr_fork = curr_fork->next_fork;
  }
}

/** hist_print
* to print history
* @ param void 
* @ return void
 */
/* Checked */
void hist_print(void)
{
  struct fork_list_struct* curr_fork = prevt_fork;
  while (curr_fork != NULL) {
    printf("%d %s", curr_fork->index, curr_fork->cmd);
    curr_fork = curr_fork->prev_fork;
  }
}

/** find_last_command
* to find relevant last command (first version)
* @ param command
* @ return void
 */
void find_last_command_test(char **cmd, unsigned int last_cmd_no) {
    *cmd = realloc(*cmd, strlen(nexth_fork->cmd)*sizeof(char));
    nexth_fork->index = last_cmd_no;
    strcpy(*cmd, nexth_fork->cmd);
}

/** hist_search_index_prefix
* to search and find the specific commannd
* @ param command
* @ return void
 */
int hist_search_index_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
  struct fork_list_struct* curr_fork = nexth_fork;
  while (curr_fork != NULL) {
    if(strncmp(curr_fork->cmd, prefix, strlen(prefix)-1) == 0) {
      //cmd = realloc(cmd, strlen(curr_fork->cmd)*sizeof(char));
      int match_index = curr_fork->index;
      return match_index;
    }
    curr_fork = curr_fork->next_fork;
    //LOG("next: %d\n", curr_fork->index);
    //LOG("next: %s\n", curr_fork->cmd);
  }
  return -1;
}

/** hist_search_upkey
* to search and find the specific commannd index
* @ param match_index
* @ param prefix
* @ return void
 */
unsigned int hist_search_upkey(unsigned int match_index, char *prefix)
{
  //char *cmd;
  struct fork_list_struct* curr_fork = nexth_fork;
  //LOG("prefix: %s\n", prefix);
  //LOG("match_index before calculation: %d\n", match_index);
  while (curr_fork != NULL) {
    if(strncmp(curr_fork->cmd, prefix, strlen(prefix)-1) == 0){ //&& curr_fork->index <= (int)match_index) {
      //cmd = realloc(cmd, strlen(curr_fork->cmd)*sizeof(char));
      match_index = (unsigned int)curr_fork->index;
      //LOG("match_index after calculation: %d\n", match_index);
      //LOG("cmd after calculation: %s\n", curr_fork->cmd);
      return match_index;
    }
    curr_fork = curr_fork->next_fork;
    //LOG("next: %d\n", curr_fork->index);
    //LOG("next: %s\n", curr_fork->cmd);
  }
  return 0;
}

/** hist_search_downkey
* to search and find the specific commannd index
* @ param match_index
* @ param prefix
* @ return void
 */
unsigned int hist_search_downkey(unsigned int match_index, char *prefix)
{
  //char *cmd;
  struct fork_list_struct* curr_fork = nexth_fork;
  //LOG("prefix: %s\n", prefix);
  //LOG("match_index before calculation: %d\n", match_index);
  while (curr_fork != NULL) {
    if(strncmp(curr_fork->cmd, prefix, strlen(prefix)-1) == 0){// && curr_fork->index >= (int)match_index) {
      //cmd = realloc(cmd, strlen(curr_fork->cmd)*sizeof(char));

      match_index = (unsigned int)curr_fork->index;
      //LOG("match_index after calculation: %d\n", match_index);
      //LOG("cmd after calculation: %s\n", curr_fork->cmd);
      return match_index;
    }
    curr_fork = curr_fork->next_fork;
    //LOG("next: %d\n", curr_fork->index);
    //LOG("next: %s\n", curr_fork->cmd);
  }
  return 0;
}

/** hist_search_prefix
* to search prefix and find relevant command using the command history
* @ param prefix e.g hi 
* @ return command e.g. history
 */
const char *hist_search_prefix(char *prefix)
{
    // TODO: Retrieves the most recent command starting with 'prefix', or NULL
    // if no match found.
  char *cmd;
  const char *const_cmd;
  struct fork_list_struct* curr_fork = nexth_fork;
  while (curr_fork != NULL) {
    if(strncmp(curr_fork->cmd, prefix, strlen(prefix)-1) == 0) {
      //cmd = realloc(cmd, strlen(curr_fork->cmd)*sizeof(char));
      cmd = curr_fork->cmd;
      const_cmd = cmd;
      return const_cmd;
    }
    curr_fork = curr_fork->next_fork;
  }
  return NULL;
}

/** hist_search_cnum
* to give command number and find relevant command using the command history number
* @ param command number
* @ return command
 */
const char *hist_search_cnum(int command_number)
{
    // TODO: Retrieves a particular command number. Return NULL if no match
    // found.

  // char* cmd = 0;
  

  struct fork_list_struct* curr_fork = {0};
  // cmd = realloc(cmd, strlen(curr_fork->cmd)*sizeof(char));
  // if(cmd == NULL){
  //   free(cmd);
  //   return NULL;
  // }

  curr_fork = nexth_fork;

  while(curr_fork != NULL) {
    if(curr_fork->index == command_number) {
      // cmd = realloc(cmd, strlen(curr_fork->cmd)*sizeof(char));
      //char* cmd_copy = cmd;
      return curr_fork->cmd;;
    }
    curr_fork = curr_fork->next_fork;
  }
  // free(cmd);
  return NULL;
}

/** hist_last_cnum
* to find relevant command number using the command history number
* and return the command number
* @ param void 
* @ return command number
 */
unsigned int hist_last_cnum(void)
{
    // TODO: Retrieve the most recent command number.
    unsigned int index;
    char* cmd = malloc(sizeof(char));
    char* cmd_copy = cmd;
    if(cmd == NULL){
        free(cmd_copy);
        return 0;
    }
    cmd = realloc(cmd, strlen(nexth_fork->cmd)*sizeof(char));
    char* cmd_copy_2 = cmd;
    if(cmd == NULL){
        free(cmd_copy_2);
        return 0;
    }

    index = nexth_fork->index;
    free(cmd_copy_2);
    return index;
}
