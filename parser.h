#ifndef _PARSER_H
#define _PARSER_H
/*
parser module, contains functions to get commands from terminal
*/

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
gets command from command line,
given the game mode (init, solve or edit)
returns type of command
outputs integer parameters (if any) to num_param
outputs a string parameter (if any) to str_param
*/
CommandType get_command(GameMode mode, int* num_param, char* str_param);

#endif