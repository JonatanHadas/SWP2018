#include "parser.h"

#include <stdio.h>
#include <stdlib.h> /* for atoi */
#include <ctype.h> /* character handling */
#include <string.h>

/*
returns whether given command type is valid in given game mode
*/
bool is_valid(CommandType comm, GameMode mode){
	switch(comm){
	case CMD_SOLVE:
	case CMD_EDIT:
	case CMD_EXIT:
		return true; /* these commands are always valid */
		
	case CMD_MARK_ERRS:
	case CMD_HINT:
	case CMD_AUTOFILL:
		return mode == MODE_SOLVE; /* these commands are valid only in solve mode */
		
	case CMD_GENERATE:
		return mode == MODE_EDIT; /* these commands are valid only in edit mode */

	case CMD_PRINT:
	case CMD_SET:
	case CMD_VALIDATE:
	case CMD_UNDO:
	case CMD_REDO:
	case CMD_SAVE:
	case CMD_COUNT_SOLUTIONS:
	case CMD_RESET:
		return mode != MODE_INIT; /* these commands are valid in solve and edit modes */
	
	default: /* should never be reached */
		return false;
	}
}

/*
returns pointer to first nne whitespace character in given str
*/
char* skip_blank(char* str){
	while(isblank(*str)) str++;
	return str;
}

bool get_int_param(char* str, int* param){
	char* pos; /* position in str */
	
	for(pos = str; *pos; pos++) if(!isdigit(*pos)) return false; /* can convert if all characters are digits */
	
	*param = atoi(str); /* set output */
	return true; /* success */
}

/*
gets line from stdin into "str", that does not exceed MAX_COMMAND_LENGTH

returns 0 on failure (EOF already reached)
returns -1 if line is too long
returns 1 on success

newline char is removed

str must be at least 2 characters longer than MAX_COMMAND_LENGTH
*/
int get_line(char* str){
	if(fgets(str, MAX_COMMAND_LENGTH+2, stdin)){
		int len; /* length */
		for(len = 0; str[len]; len++){
			if(str[len] == '\n'){ /* if new line found then line is short enough */
				str[len] = '\0'; /* remove newline */
				return 1; /* success */
			}
		}
		/* if no new line was found, either line is too long or eof was reached */
		if(len > MAX_COMMAND_LENGTH) return -1; /* line was too long */
		return 1; /* success */
	}
	return 0; /* fgets failed (EOF reached) */
}

/*
returns whether "str" starts with the command "comm"

after "comm", "str" must have a blank character, or end
*/
bool compare_command(char* str, const char* comm){
	int comm_len = strlen(comm); /* length of comm */
	if( 0 != strncmp(str, comm, comm_len) ) return false; /* str does not match comm */
	
	/* by this point it is known that str starts with comm */
	if( str[comm_len] == '\0') return true; /* it ends there */
	if( isblank(str[comm_len]) ) return true; /* comm is followed by blank char */
	
	return false; /* does not match */
}

/*
copies src to dst until blank or null char is reached

returns position where it stopped
*/
char* copy_until_blank(char* src, char* dst){
	while(*src && !isblank(*src)) *dst++ = *src++; /* copy and advance */
	*dst = '\0';
	return src;
}

#define COMMAND_NUM 15

/* all commands */
CommandType commands[COMMAND_NUM] = {
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
	CMD_EXIT};

/* all commands as text */
const char* command_texts[COMMAND_NUM] = {
	"solve",
	"edit",
	"mark_errors",
	"print_board",
	"set",
	"validate",
	"generate",
	"undo",
	"redo",
	"save",
	"hint",
	"num_solutions",
	"autofill",
	"reset",
	"exit"};
/* possible number of paramters for each command  */
int min_param_nums[COMMAND_NUM] = {1,0,1,0,3,0,2,0,0,1,2,0,0,0,0};
int max_param_nums[COMMAND_NUM] = {1,1,1,0,3,0,2,0,0,1,2,0,0,0,0};
	
CommandType get_command(GameMode mode, char** params, int* param_num){
	char command[MAX_COMMAND_LENGTH + 2]; /* extra place for nullchar and newline*/
	
	while(true){
		char* str = command; /* current position in command string */
		int line_code; /* get_line's return code */
		
		printf("Enter your command:\n");
		
		if((line_code = get_line(command))){
			/* no error */
			str = skip_blank(str);
			
			if(str[0] == '\0') continue; /* blank line */
			
			if(line_code > 0){ /* line is short enough */
				/* check all possible command names */
				int i;
				for(i = 0; i<COMMAND_NUM; i++){
					if(is_valid(commands[i], mode) && compare_command(str, command_texts[i])){
						/* parse parameters */
						str += strlen(command_texts[i]); /* go to end of command */
						for(*param_num = 0; *param_num<max_param_nums[i];(*param_num)++){ /* iterate through all possible parameters */
							char* next_pos; /* position at end of argument */
							
							str = skip_blank(str); /* skip blank char */
							next_pos = copy_until_blank(str, params[(*param_num)]); /* copy parameter and increase number */
							if(next_pos == str){ /* no more parameters */
								break;
							}
							str = next_pos; /* advance str */
						}
						if(*param_num >= min_param_nums[i])  return commands[i]; /* command is completely valid */
						break; /* only one command can be matched, no need to check others */
					}
				}
				
				/* if no command matches than command is invalid */
			}
			
			printf("ERROR: invalid command\n"); /* line found, but command could not be recognized */
		}
		else{ /* eof reached or error encountered */
			if(ferror(stdin)){
				/* error in input reading */
				printf("Error in function fgets\n");
				/* exit */
			}
			else{
				/* EOF reached */
			}
		}
	}
	
	/*  */
	return CMD_EXIT;
}

