/**
 * @file
 *
 * Utility function to handle getting info from the user
 * Author: Rozita Teymourzadeh, Mathew Malensek
 * Date: 2020
 * 
 */

#ifndef _USER_H_
#define _USER_H_


#define _PROGRAM_NAME "whoami"
#define BUFF 		256
#define POUTING 	"\U0001F621"
#define KISSING 	"\U0001F61A"
#define HAPPY  		"\U0001F64B"
#define CRYING  	"\U0001F630"
#define BATH  		"\U0001F6C0"
#define ROLLING  	"\U0001F923"
#define HEART  		"\U0001F970"
#define MONEY 		"\U0001F911"

/** 
* define variable 
 */
char HOME[BUFF];
char USERNAME[BUFF];
char HOSTNAME[BUFF];




/** get_user
* function to get a user info
* @ param void
* @ return void
 */
void get_user();

/** get_hostname
* function to get a hostname
* @ param void
* @ return void
 */
void get_hostname();

/** get_home_dir
* function to get a home directory
* @ param void
* @ return void
 */
void get_home_dir();

/** set_emoji
* to set emoji
* @ param emoji
* @ return void
 */
void set_emoji (char *new_status);


/** get_emoji
* to get emoji
* @ param void
* @ return emoji
 */
char* get_emoji (void);

/** set_cmd_no
* to set command number
* @ param new_cmd_no command number
* @ return void
 */
void set_cmd_no (int new_cmd_no);

/** get_cmd_no
* to get cmd_no
* @ param void
* @ return cmd_no
 */
int get_cmd_no (void);

#endif