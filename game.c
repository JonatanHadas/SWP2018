#include "game.h"

#include "stdlib.h" /* for allocation functions */

Board* create_board(int cell_w, int cell_h){
	Board* new_board;
	int i; /* index for for loops */
	
	/* allocate board structure */
	new_board = malloc(sizeof(Board));
	if(new_board == NULL) return NULL; /* return null pointer on failure */
	
	new_board->cell_w = cell_w;
	new_board->cell_h = cell_h;
	
	/* allocate board memory: cell_w * cell_h rows and columns of integers (set by default to 0) */
	new_board->memory = calloc(cell_w * cell_w * cell_h * cell_h, sizeof(int));
	if(new_board->memory == NULL){ /* failure */
		free(new_board);
		return NULL;
	}
	
	/* allocate pointers to cell_w*cell_h rows */
	new_board->table = calloc(cell_w * cell_h, sizeof(int*));
	
	if(new_board->table == NULL){ /* failure */
		free(new_board->memory);
		free(new_board);
		return NULL;
	}
	
	/* set row pointers */
	for(i = 0; i<cell_w*cell_h; i++){
		/* i'th row starts at i*cell_w*cell_h (cell_w*cell_h cells in a row */
		new_board->table[i] = new_board->memory + i*cell_w*cell_h;
	}
	
	return new_board;
}

void free_board(Board* board){
	free(board->memory);
	free(board->table);
	free(board);
}

Board* copy_board(Board* board){
	Board* new_board;
	int x,y; /* for iterating over board */
	
	/* create board of same dimensions */
	new_board = create_board(board->cell_w, board->cell_h);
	
	if(new_board == NULL) return NULL; /* return null pointer on failure */
	
	for(x = 0; x<board->cell_w*board->cell_h; x++){ /* columns */
		for(y = 0; y<board->cell_w*board->cell_h; y++){ /* rows */
			new_board->table[y][x] = board->table[y][x]; /* copy values */
		}
	}
	
	return new_board;
}

