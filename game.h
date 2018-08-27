#ifndef _GAME_H
#define _GAME_H
/*
game module, contains functions and structures for saving game state
*/

#include <stdbool.h> /* boolean type */

/*
structure for saving a game board
*/
typedef struct sudoku_board{
	int cell_w, cell_h; /* cell width and height */
	/* note board is cell_h cells wide and cell_w cells high */
	int* memory; /* memory to keep board: row by row */
	int** table; /* array of pointers to rows */
}Board;

/*
creates board with given dimensions
*/
Board* create_board(int cell_w, int cell_h);
/*
frees board memory
*/
void free_board(Board* board);
/*
creates copy of given board
*/
Board* copy_board(Board* board);

/*
prints given board

marks errors if mark_errors!=0
*/
void print_board(Board* board, int mark_errors);

/*
check if position is erroneous
returns 1 if it is and 0 if it isn't
*/
bool check_position(Board* board, int x, int y);

/*
check if any position is erroneous in given board
*/
bool check_board(Board* board);

/*
prints all changes made to go from first board to second
*/
void print_changes(Board* first, Board* second);


/*
doubly linked list node containing a board;
*/
typedef struct board_linked_node{
	struct board_linked_node* next;
	struct board_linked_node* prev;
	Board* board;
} BoardListNode;

/*
create a single board node with a null board pointer
*/
BoardListNode* create_board_node();
/*
free a board node, and all board nodes after it
*/
void free_board_list(BoardListNode* node);


/*
state of a sudoku game, intc
*/
typedef struct game{
	BoardListNode* undo_list_head; /* start of game */
	BoardListNode* undo_list_tail; /* last state */
	BoardListNode* current_state; /* current state of game */
	
	int* memory; /* memory saving whether cells are fixed */
	int** fixed; /* array of pointers to rows in memory*/
} Game;

/*
create game with given cell dimensions and a single state in list
*/
Game* create_game(int cell_w, int cell_h);

/*
frees game with it's entire undo list
*/
void free_game(Game* game);

/*
adds new state to end of undo list, identical to last
*/
void add_state(Game* game);

/*
save game state to file

if all_fixed is true: save all non-empty cells as fixed
*/
void save_board(Game* game, char* filename, int all_fixed);
/*
load game state from file

if use_fixed is false: does not load whether cells are fixed
*/
Game* load_board(char* filename, int use_fixed);



#endif