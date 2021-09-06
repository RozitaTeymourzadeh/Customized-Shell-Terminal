/**
 * @file
 *
 * Utility function to handle shell functionality
 * Author: Rozita Teymourzadeh, Mathew Malensek
 * Date: 2020
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h> 

#include "user.h" 

static char *emoji; // private variable
static int command_number; // private variable



/** set_emoji
* to set emoji
* @ param emoji
* @ return void
 */
void set_emoji (char *new_status)
{
  emoji = new_status;
}

/** get_emoji
* to get emoji
* @ param void
* @ return emoji
 */
char* get_emoji (void)
{
  return emoji;
}

/** set_cmd_no
* to set command number
* @ param new_cmd_no command number
* @ return void
 */
void set_cmd_no (int new_cmd_no)
{
  command_number = new_cmd_no;
}

/** get_cmd_no
* to get cmd_no
* @ param void
* @ return cmd_no
 */
int get_cmd_no (void)
{
  return command_number;
}

/** get_user
* function to get a user info
* @ param void
* @ return void
 */
void get_user() {
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);
	if (pw) {
		strcpy(USERNAME, pw->pw_name);
	}
}

/** get_hostname
* function to get a hostname
* @ param void
* @ return void
 */
void get_hostname() {
	char buff[BUFF];
	int hostname;

	hostname = gethostname(buff, sizeof(buff));
	if (hostname != -1) {
		strcpy(HOSTNAME, buff);
	} else {
		perror("Hostname is invalid.");
	}
}

/** get_home_dir
* function to get a home directory
* @ param void
* @ return void
 */
void get_home_dir() {
	struct passwd *pw = getpwuid(getuid());
	strcpy(HOME, pw->pw_dir);
}

