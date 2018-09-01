#include "game.h"

#include <stdio.h>
#include <stdlib.h> /* for allocation functions */

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


bool check_position(Board* board, int x, int y){
	int ix,iy; /* for iterating over board */
	int cell_x, cell_y;/* position of cell of x,y */
	
	if( board->table[y][x] == 0) return false; /* no errors in empty cell */
	
	/* check row */ 
	for(ix = 0; ix < board->cell_w*board->cell_h; ix++){
		if(board->table[y][x] == board->table[y][ix] && x != ix){
			return true; /* error found */
		}
	}

	
	/* check column */ 
	for(iy = 0; iy < board->cell_w*board->cell_h; iy++){
		if(board->table[y][x] == board->table[iy][x] && y != iy){
			return true; /* error found */
		}
	}
	
	cell_x = (x / board->cell_w) * board->cell_w; /* round x to closest cell_w multiple below */
	cell_y = (y / board->cell_h) * board->cell_h; /* round y to closest cell_h multiple below */
	
	/* check cell */
	for(ix = cell_x; ix < cell_x + board->cell_w; ix++){
		for(iy = cell_y; iy < cell_y + board->cell_h; iy++){
			if(board->table[y][x] == board->table[iy][ix] && !(x == ix && y == iy)){
				return true; /* error found */
			}
		}
	}
	
	return false; /* no errors found */
}

bool check_board(Board* board){
	int x,y; /* position in board */
	
	for(x = 0; x < board->cell_w*board->cell_h; x++){
		for(y = 0; y < board->cell_w*board->cell_h; y++){
			if(check_position(board, x, y)){
				return true; /* error found */
			}
		}
	}
	
	return false; /* no errors found */
}

/*
print a change in the board given coordinates and values

value at coordinate x,y is changed from z1 to z2
*/
void print_change(int x, int y, int z1, int z2, ChangeType t){
	switch(t){
	case CHANGE_UNDO:
		printf("Undo %d,%d: from %d to %d\n", x,y,z1,z2);
		break;
	case CHANGE_REDO:
		printf("Redo %d,%d: from %d to %d\n", x,y,z1,z2);
		break;
	case CHANGE_SET:
		printf("Cell <%d,%d> set to %d\n", x,y,z2);
		break;
	}
}

void print_changes(Board* first, Board* second, ChangeType t){
	int x,y; /* position on board */
	
	/* go over board */
	for(x = 0; x < first->cell_w*first->cell_h; x++){
		for(y = 0; y < first->cell_w*first->cell_h; y++){
			if(first->table[y][x] != second->table[y][x]){ /* changed */
				print_change(x,y,first->table[y][x],second->table[y][x], t);
			}
		}
	}
}

BoardListNode* create_board_node(){
	BoardListNode* node; /* new node */
	
	node = malloc(sizeof(BoardListNode()));
	
	if(node == NULL) return NULL; /* return null pointer on failure */
	
	node->next = node->prev = NULL; /* initialize values */
	node->board = NULL;
	return node;
}

void free_board_list(BoardListNode* node){
	BoardListNode* next; /* next node */
	
	/* erase pointer to here */
	if(node->prev) node->prev->next = NULL; 
	
	while(node){ /* loop until end of list */
		next = node->next; /* save next node */
		if(node -> board) free_board(node->board); /* delete board if there is one */
		free(node); /* delete current node */
		node = next; /* go to next node */
	}
}
