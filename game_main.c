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
	state->mark_errors = true;
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
bool get_bool(char* str, bool* out){
	int num;
	if(! get_int_param(str, &num) || num < 0 || num > 1){
		fprintf(stderr, "Error: the value should be 0 or 1\n");
		return false;
	}
	*out = (num == 1); /* 1 for true, 0 for false */
	return true;
}

bool try_set(GameState* state, int x, int y, int z){
	if(state->game->fixed[y][x]){
		printf("Error: cell is fixed\n");
	}
	else{
		/* clear undo list beyond current position */
		if(state->game->current_state->next) free_board_list(state->game->current_state->next);
		state->game->undo_list_tail = state->game->current_state;
		
		if(! add_state(state->game)){
			return true; /* error */
		}
		state->game->current_state = state->game->current_state->next; /* advance one state */
		
		/* set position */
		state->game->current_state->board->table[y][x] = z;
				
		print_game(state);
		check_win(state);
	}
	
	return false;
}

bool try_undo(GameState* state){
	if(state->game->current_state->prev){ /* if previous state exists can undo */
		state->game->current_state = state->game->current_state->prev; /* move back */
		print_game(state); /* print board */
		print_changes(
				state->game->current_state->next->board,
				state->game->current_state->board,
				CHANGE_UNDO); /* print changes */
	}
	else{
		fprintf(stderr, "Error: no moves to undo\n");
	}
	return false;
}
bool try_redo(GameState* state){
	if(state->game->current_state->next){ /* if next state exists can redo */
		state->game->current_state = state->game->current_state->next; /* move back */
		print_game(state); /* print board */
		print_changes(
				state->game->current_state->prev->board,
				state->game->current_state->board,
				CHANGE_REDO); /* print changes */
	}
	else{
		fprintf(stderr, "Error: no moves to redo\n");
	}
	return false;
}

void reset(GameState* state){
	state->game->current_state = state->game->undo_list_tail = state->game->undo_list_head; /* shrink undo list to first state */
	
	if(state->game->current_state->next) free_board_list(state->game->current_state->next); /* clear rest of list */
	
	printf("Board reset\n");
}

bool validate(GameState* state){
	Board* solution;
	if(check_board(state->game->current_state->board)){
		fprintf(stderr, "Error: board contains erroneous values\n");
		return false;
	}
	solution = solve(state->game->current_state->board);
	if(solution == NULL) return true;
	if(solution == state->game->current_state->board){
		printf("Validation failed: board is unsolvable\n");
	}
	else{
		free_board(solution);
		printf("Validation passed: board is solvable\n");
	}
	return false;
}

bool save_game(GameState* state, char* filename){
	Board* solution;
	if(state->mode == MODE_EDIT){
		if(check_board(state->game->current_state->board)){
			fprintf(stderr,"Error: board contains erroneous values\n");
			return false;
		}
		solution = solve(state->game->current_state->board);
		if(solution == NULL) return true;
		if(solution == state->game->current_state->board){
			/* solution board is not a new board, and shouldn't be free'd */
			fprintf(stderr, "Error: board validation failed\n");
			return false;
		}
		free_board(solution); /* free solution board */
	}
	if(save_board(state->game, filename, state->mode == MODE_EDIT)){
		printf("Saved to: %s\n", filename);
	}
	else{
		fprintf(stderr, "Error: File cannot be created or modified\n");
	}
	return false;
}

bool print_solution_num(GameState* state){
	int sol_num; /* number of solutions */
	if(check_board(state->game->current_state->board)){
		fprintf(stderr, "Error: board contains erroneous values\n");
		return false;
	}
	if(! count_solutions(state->game->current_state->board, &sol_num)){
		return true; /* error */
	}
	printf("Number of solutions: %d\n", sol_num);
	if(sol_num == 1){
		printf("This is a good board!\n");
	}
	else if(sol_num > 1){
		printf("The puzzle has more than 1 solution, try editing it further\n");
	}
	return false;
}

bool try_generate(GameState* state, int add, int remain){
	int N = get_game_size(state->game);
	Board* new;
	if(count_empty_places(state->game->current_state->board, NULL, NULL) != N*N){ /* board not empty */
		fprintf(stderr, "Error: board is not empty\n");
		return false;
	}
	/* generate board */
	new = generate(state->game->current_state->board, add, remain);
	if(new == NULL) return true; /* error */
	if(new == state->game->current_state->board){
		fprintf(stderr, "Error: puzzle generator failed\n");
		return false;
	}
	/* clear undo list beyond current position */
	if(state->game->current_state->next) free_board_list(state->game->current_state->next);
	state->game->undo_list_tail = state->game->current_state;
	
	if(! add_state(state->game)){
		free_board(new);
		return true; /* error */
	}
	state->game->current_state = state->game->current_state->next; /* advance one state */
	
	
	replace_node_board(state->game->current_state,new);
	print_game(state);
	return false;
}

bool try_autofill(GameState* state){
	Board* new;
	
	if(check_board(state->game->current_state->board)){
		fprintf(stderr, "Error: board contains erroneous values\n");
		return false;
	}
	
	new = autofill(state->game->current_state->board);
	if(new == NULL) return true; /* error */
	if(new == state->game->current_state->board) return false; /* no changes */

	/* clear undo list beyond current position */
	if(state->game->current_state->next) free_board_list(state->game->current_state->next);
	state->game->undo_list_tail = state->game->current_state;
	
	if(! add_state(state->game)){
		free_board(new);
		return true; /* error */
	}
	state->game->current_state = state->game->current_state->next; /* advance one state */
	
	replace_node_board(state->game->current_state,new);
	/* print changes */
	print_changes(
			state->game->current_state->prev->board,
			state->game->current_state->board,
			CHANGE_SET);
	print_game(state);
	check_win(state); /* check for end condition */
	return false;
}

void check_win(GameState* state){
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