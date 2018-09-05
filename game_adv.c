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
		printf("%d %d: %d\n", x,y,count_legal_values(board,x,y,values));
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