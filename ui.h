/**
 * @file
 *
 * Text-based UI functionality. These functions are primarily concerned with
 * interacting with the readline library.
 * Author: Rozita Teymourzadeh, Mathew Malensek
 * Date: 2020
  */

#ifndef _UI_H_
#define _UI_H_


/**
 * find_history_by_down_key
 *
 * To find next command trigered by down arrow key
 * 
 * @ param void
 * @ return command
  */
const char* find_history_by_down_key();

/**
 * find_history_by_up_key
 *
 * To find next command trigered by up arrow key
 * 
 * @ param void
 * @ return command
  */
const char* find_history_by_up_key();

/**
 * init_ui
 *
 * To initialise user interface
 * 
 * @ param void
 * @ return void
  */
void init_ui(void);

/**
 * prompt_line1
 *
 * To provide second line of the command line
 * 
 * @ param void
 * @ return void
  */
char *prompt_line1(void);

/**
 * read_command
 *
 * To provide 2 lines of command printed for shell
 * 
 * @ param void
 * @ return void
  */
char *read_command();

/**
 * check_path 
 * Checks to see if the path is in the home directory
 * @ param void
 * @ param void
  */
void check_path();

/** print_prompt
* function to print the starter command
* @ param void
* @ return void
 */
//void print_prompt();
void print_prompt(void);

/**
 * Parse_home 
 * Parses the home directory to find the name of the sub-directory. 
 * Needed for printing the prompt
 * @ param line - path to parse
 * @ param void
  */
void parse_home(char *line);

/** set_key_cmd_no
* to set command number
* @ param new_cmd_no command number
* @ return void
 */
void set_key_cmd_no (unsigned int new_cmd_no);

/** get_key_cmd_no
* to get cmd_no
* @ param void
* @ return cmd_no
 */
unsigned int get_key_cmd_no (void);

#endif
