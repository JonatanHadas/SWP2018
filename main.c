#include "game_main.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(){
	GameState state;
	
	char* params[MAX_PARAM_NUM]; /* parameters for command */
	int x,y,z; /* integer parameters */
	int param_num; /* number of parameters */ 
	
	/* controls */
	bool error = false;
	bool finished = false;
	
	
	int i;
	
	for(i=0; i<MAX_PARAM_NUM; i++){
		params[i] = calloc(MAX_COMMAND_LENGTH, sizeof(char));
		if(params[i] == NULL){
			if(! error) fprintf(stderr, "Error: calloc has failed\n");
			error = true;
		}
	}
	
	set_init(&state);
	
	while(!(error || finished)){
		int N = state.game ? get_game_size(state.game) : 0; /* get game size */
		switch(get_command(state.mode, params, &param_num)){
		case CMD_SOLVE:
			if(open_solve(&state, params[0])) error = true;
			break;
		case CMD_EDIT:
			if(param_num == 1) {if(open_edit(&state, params[0])) error = true;}
			else {if(open_default(&state)) error = true;}
			break;
		case CMD_EXIT:
			finished = true;
			break;
		case CMD_MARK_ERRS:
			get_bool(params[0],&state.mark_errors); /* set mark_errors to input */
			break;
		case CMD_HINT:
			if(get_num_lim(params[0], &x, 1, N) && get_num_lim(params[1], &y, 1, N)){
				if(hint(state.game, x-1, y-1)) error = true;
			}
			break;
		case CMD_AUTOFILL:
			if(try_autofill(&state)) error = true;
			break;
		case CMD_GENERATE:
			if(get_num_lim(params[0], &x, 1, N*N) && get_num_lim(params[1], &y, 1, N*N)){
				try_generate(&state, x, y);
			}
			break;
		case CMD_PRINT:
			print_game(&state);
			break;
		case CMD_SET:
			if(get_num_lim(params[0],&x,1,N) && get_num_lim(params[1], &y,1,N) && get_num_lim(params[2], &z,0,N)){
				if(try_set(&state, x-1, y-1, z)) error = true;
			}
			break;
		case CMD_VALIDATE:
			if(validate(&state)) error = true;
			break;
		case CMD_UNDO:
			if(try_undo(&state)) error = true;
			break;
		case CMD_REDO:
			if(try_redo(&state)) error = true;
			break;
		case CMD_SAVE:
			if(save_game(&state, params[0])) error = true;
			break;
		case CMD_COUNT_SOLUTIONS:
			if(print_solution_num(&state)) error = true;
			break;
		case CMD_RESET:
			reset(&state);
			break;
		default: /* should never be reached */
			error = true;
			break;
		}
	}
	
	if(state.game) free_game(state.game); /* free game if necessary */
	/* free parameter memory */
	for(i=0; i<MAX_PARAM_NUM; i++){
		if(params[i] != NULL) free(params[i]); /* free only if allocation is successful */
	}
	
	printf("Exiting...\n");
	
	return error; /* return code 0 means no errors */
}