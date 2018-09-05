#include "game_main.h"
#include "parser.h"

#include <stdlib.h>
#include <stdio.h>

void print_game(GameState* state){
	print_board(state->game, state->mark_errors || state->mode == MODE_EDIT);
}

void set_init(GameState* state){
	state->game = NULL;
	state->mode = MODE_INIT;
	state->mark_errors = false;
}

bool open_solve(GameState* state, char* filename){
	Game* new_game = load_board(filename, true); /* load with fixed posiitons */
	if(new_game == NULL) return false; /* error in file reading */
	
	if(state->game) free_game(state->game); /* delete old game */
	state->game = new_game;
	state->mode = MODE_SOLVE;
	return false;
}

bool open_edit(GameState* state, char* filename){
	Game* new_game = load_board(filename, false); /* load with fixed posiitons */
	if(new_game == NULL) return false; /* error in file reading */
	
	if(state->game) free_game(state->game);
	state->game = new_game;
	state->mode = MODE_EDIT;
	return false;
}

#define DEFAULT_GAME_SIZE 3

bool open_default(GameState* state){
	Game* new_game = create_game(DEFAULT_GAME_SIZE,DEFAULT_GAME_SIZE); /* load with fixed posiitons */
	if(new_game == NULL) return true; /* allocation error */
	
	if(state->game) free_game(state->game);
	state->game = new_game;
	state->mode = MODE_EDIT;
	return false;
}

bool get_num_lim(char* str, int* out, int lower, int upper){
	if(! get_int_param(str, out) || *out < lower || *out > upper){
		fprintf(stderr, "Error: value not in range %d-%d\n", lower, upper);
		return false;
	}
	return true;
}

bool try_set(GameState* state, int x, int y, int z){
	x--;y--; /* convert coordinates to start from 0 */
	if(state->game->fixed[y][x]){
		printf("Error: cell is fixed");
	}
	else{
		/* clear undo list beyond current position */
		if(state->game->current_state->next) free_board_list(state->game->current_state->next);
		
		if(! add_state(state->game)){
			return true; /* error */
		}
		state->game->current_state = state->game->current_state->next; /* advance one state */
		
		/* set position */
		state->game->current_state->board->table[y][x] = z;
				
		print_game(state);
		if(state->mode == MODE_SOLVE && count_empty_places(state->game->current_state->board, NULL, NULL) == 0){
			/* full board */
			if(check_board(state->game->current_state->board)){
				printf("Puzzle solution erroneous\n");
			}
			else{
				printf("Puzzle solved successfully\n");
				free_game(state->game); /* claer game and set to init */
				state->game = NULL;
				state->mode = MODE_INIT;
			}
		}
	}
	
	return false;
}
