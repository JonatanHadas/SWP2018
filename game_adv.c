#include "game_adv.h"

#include <stdlib.h> /* malloc */

#include <stdio.h>

Board* autofill(Board* board){
	Board* new_board;
	int* values; /* legal values for position */
	int x,y; /* for loops */
	int N = board->cell_w * board->cell_h;
	int num_changes = 0;
	
	values = calloc(N,sizeof(int));
	if(values == NULL) return NULL;
	
	new_board = copy_board(board);
	if(new_board == NULL){
		free(values);
		return NULL;
	}
	
	/* go over board */
	for(x=0;x<N;x++) for(y=0;y<N;y++){
		if(count_legal_values(board,x,y,values) == 1){ /* only one value */
			num_changes++;
			new_board->table[y][x] = values[0]; /* set the legal value to new board */
		}
	}
	
	
	if(num_changes == 0){
		free_board(new_board);
		new_board = board; /* return the original board if no changes */
	}
	
	free(values);
	return new_board;
}

void hint(Game* g, int x, int y){
	Board* board = g->current_state->board; /* get currnet board */
	Board* sol; /* solution */
	
	/* check for immediate errors */
	if(check_board(board)){
		fprintf(stderr, "Error: board contains erroneous values\n");
		return;
	}
	/* cell must be empty and non fixed */
	if(g->fixed[y][x]){
		fprintf(stderr, "Error: cell is fixed\n");
		return;
	}
	
	if(board->table[y][x] != 0){
		fprintf(stderr, "Error: cell already contains a value\n");
		return;
	}
	sol = solve(board);
	if(sol == NULL){
		return; /* some error in solution */
	}
	if(sol == board){ /* unsolvable */
		fprintf(stderr, "Error: board is unsolvable\n");
		return;
	}
	
	printf("Hint: set cell to %d\n", sol->table[y][x]); /* print hint */
	
	free_board(sol);
}

