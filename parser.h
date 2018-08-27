#ifndef _PARSER_H
#define _PARSER_H
/*
parser module, contains functions to get commands from terminal
*/

#define MAX_COMMAND_LENGTH 256
#define MAX_PARAM_NUM 3

typedef enum game_mode_enum{
	MODE_INIT,
	MODE_SOLVE,
	MODE_EDIT
} GameMode;

/*
possible types of commands
*/
typedef enum command_type_enum{
	CMD_SOLVE,
	CMD_EDIT,
	CMD_MARK_ERRS,
	CMD_PRINT,
	CMD_SET,
	CMD_VALIDATE,
	CMD_GENERATE,
	CMD_UNDO,
	CMD_REDO,
	CMD_SAVE,
	CMD_HINT,
	CMD_COUNT_SOLUTIONS,
	CMD_AUTOFILL,
	CMD_RESET,
	CMD_EXIT
} CommandType;


/*
extracts an integer from "str" into "param",
returns whether succeeded
*/
bool get_int_param(char* str, int* param){

/*
gets command from command line,
given the game mode (init, solve or edit)
returns type of command
outputs parameters (if any) to params
output number of parameters to param_num
*/
CommandType get_command(GameMode mode, char** params, int* param_num);

#endif