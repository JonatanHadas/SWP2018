#include "game.h"

#include <stdio.h>
#include <stdlib.h> /* for allocation functions */

Board* create_board(int cell_w, int cell_h){
	Board* new_board;
	int i; /* index for for loops */
	
	/* allocate board structure */
	new_board = malloc(sizeof(Board));
	if(new_board == NULL){
		fprintf(stderr,"Error: malloc has failed\n");
		return NULL; /* return null pointer on failure */
	}
	
	new_board->cell_w = cell_w;
	new_board->cell_h = cell_h;
	
	/* allocate board memory: cell_w * cell_h rows and columns of integers (set by default to 0) */
	new_board->memory = calloc(cell_w * cell_w * cell_h * cell_h, sizeof(int));
	if(new_board->memory == NULL){ /* failure */
		fprintf(stderr,"Error: calloc has failed\n");
		free(new_board);
		return NULL;
	}
	
	/* allocate pointers to cell_w*cell_h rows */
	new_board->table = calloc(cell_w * cell_h, sizeof(int*));
	
	if(new_board->table == NULL){ /* failure */
		fprintf(stderr,"Error: calloc has failed\n");
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
	char s1[3],s2[3]; /* strings for z1,z2 respectively */
	sprintf(s1,"%d",z1); /* convert */
	if(z1==0){s1[0] = '_'; s1[1] = '\0';} /* _ for empty cell */
	sprintf(s2,"%d",z2); /* convert */
	if(z2==0){s2[0] = '_'; s2[1] = '\0';} /* _ for empty cell */
	switch(t){
	case CHANGE_UNDO:
		printf("Undo %d,%d: from %s to %s\n", x+1,y+1,s1,s2);
		break;
	case CHANGE_REDO:
		printf("Redo %d,%d: from %s to %s\n", x+1,y+1,s1,s2);
		break;
	case CHANGE_SET:
		printf("Cell <%d,%d> set to %d\n", x+1,y+1,z2);
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

int count_empty_places(Board* board, int* xs, int* ys){
	int count=0,x,y;
	
	/* go over board */
	for(y = 0; y < board->cell_w * board->cell_h; y++){
		for(x = 0; x < board->cell_w * board->cell_h; x++){
			if(board->table[y][x] == 0){
				if(xs != NULL && ys != NULL){
					xs[count] = x;
					ys[count] = y;
				}
				count++;
			}
		}
	}
	
	return count;
}

int count_legal_values(Board* board, int x, int y, int* z){
	int count = 0,num;
	
	if(board->table[y][x] != 0){ /* position not empty */
		return -1;
	}
	/* go over all numbers */
	for(num=1; num <= board->cell_w*board->cell_h; num++){
		board->table[y][x] = num;
		if(! check_position(board,x,y)){ /* check if value is legal */
			z[count] = num;
			count++;
		}
	}
	board->table[y][x] = 0; /* reset back to empty */
	return count;
}

BoardListNode* create_board_node(){
	BoardListNode* node; /* new node */
	
	node = malloc(sizeof(BoardListNode));
	
	if(node == NULL){
		fprintf(stderr,"Error: malloc has failed\n");
		return NULL; /* return null pointer on failure */
	}
	
	node->next = node->prev = NULL; /* initialize values */
	node->board = NULL;
	return node;
}

void free_board_list(BoardListNode* node){
	BoardListNode* next; /* next node */
	
	/* erase next pointer of previous node if it still leads here */
	if(node->prev && node->prev->next == node) node->prev->next = NULL; 
	
	while(node){ /* loop until end of list */
		next = node->next; /* save next node */
		if(node -> board) free_board(node->board); /* delete board if there is one */
		free(node); /* delete current node */
		node = next; /* go to next node */
	}
}

void replace_node_board(BoardListNode* node, Board* board){
	if(node -> board && node->board!=board) free_board(node->board); /* delete old board if it is not the same one */
	node->board = board;
}


Game* create_game(int cell_w, int cell_h){
	Game* game;
	int i;
	
	/* allocate game structure */
	game = malloc(sizeof(Game));
	
	if(game==NULL){
		fprintf(stderr,"Error: malloc has failed\n");
		return NULL; /* return NULL on failure */
	}
	
	/* create list of one state */
	game->undo_list_head = game->undo_list_tail = game->current_state = create_board_node();
	
	if(game->undo_list_head == NULL){
		free(game);
		return NULL;
	}
	
	game->current_state->board = create_board(cell_w, cell_h);
	
	if(game->current_state->board == NULL){
		free_board_list(game->undo_list_head);
		free(game);
		return NULL;
	}
	
	/* allocate memory for fixed cells */
	game->memory = calloc(cell_w*cell_w*cell_h*cell_h, sizeof(bool));
	
	if(game->memory == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		free_board_list(game->undo_list_head);
		free(game);
		return NULL;
	}
	
	/* allocate cell_w*cell_h row pointers */
	game->fixed = calloc(cell_w*cell_h, sizeof(bool*));
	
	if(game->fixed == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		free(game->memory);
		free_board_list(game->undo_list_head);
		free(game);
		return NULL;
	}
	
	/* set row pointers */
	for(i = 0; i<cell_w*cell_h; i++){
		/* i'th row starts at i*cell_w*cell_h (cell_w*cell_h cells in a row */
		game->fixed[i] = game->memory + i*cell_w*cell_h;
	}
	
	return game;
	
}

void free_game(Game* game){
	free(game->fixed);
	free(game->memory);
	free_board_list(game->undo_list_head); /* erase all undo list */
	free(game);
}

bool add_state(Game* game){
	/* create new node */
	game->undo_list_tail->next = create_board_node();
	
	if(game->undo_list_tail->next == NULL) return false; /* unsuccessful */
	
	/* connect node back to tail of undo list */
	game->undo_list_tail->next->prev = game->undo_list_tail;

	/* copy last board to new state */
	game->undo_list_tail->next->board = copy_board(game->undo_list_tail->board);
	
	if(game->undo_list_tail->next->board == NULL){ /* failure */
		free_board_list(game->undo_list_tail->next); /* delete new node */
		game->undo_list_tail->next = NULL;
		return false;
	}
	
	
	/* mode tail of undo list to new node */
	game->undo_list_tail = game->undo_list_tail->next;
	
	
	return true;
}

void print_seperator_line(Game* game){
	int cell_w,cell_h,i;
	
	/* save cell width and height (for convenience and readability) */
	cell_w = game->current_state->board->cell_w;
	cell_h = game->current_state->board->cell_h;
	
	for(i = 0; i<(4*cell_w + 1)*cell_h + 1; i++) printf("-"); /* cell_h cells 4*cell_w wide with 1 character separators */
	
	printf("\n");
}

void print_board(Game* game, bool mark_errors){
	Board* board = game->current_state->board; /* get board */
	int x,y; /* position index within cell*/
	int cell_x, cell_y; /* cell index */
	
	print_seperator_line(game);
	
	for(cell_y = 0; cell_y < board->cell_w; cell_y++){ /* board is cell_w cells high */
		for(y = 0; y < board->cell_h; y++){ /* cell_h rows in a cell */
			printf("|"); /* separator */
			
			for(cell_x = 0; cell_x < board->cell_h; cell_x++){ /* board is cell_h cell wide */
				for(x = 0; x < board->cell_w; x++){ /* cell_w columns in a cell */
					int global_x = x + board->cell_w * cell_x; /* global coordinates of current position */
					int global_y = y + board->cell_h * cell_y;
					
					char type = ' '; /* specifies cell type (.fixed, *erronous,  regular) */
					
					if(game->fixed[global_y][global_x]){
						type = '.'; /* fixed */
					}
					/* if erronous and not fixed: mark erronous */
					if(type == ' ' && mark_errors && check_position(board, global_x, global_y)){
						type = '*';
					}
					printf(" ");
					if(board->table[global_y][global_x] != 0){
						printf("%2d",board->table[global_y][global_x]); /* print number */
					}
					else{
						printf("  "); /* empty cell */
					}
					printf("%c", type);
				}
				printf("|");				
			}
			
			printf("\n"); /* end of line */
		}
		print_seperator_line(game);
	}
}

bool save_board(Game* game, char* filename, bool all_fixed){
	Board* board = game->current_state->board; /* for convenience */
	FILE* file = fopen(filename,"w");
	int x,y;
	
	if(file == NULL) return false; /* unsuccessful in opening file */
	
	/* save cell size */
	fprintf(file, "%d %d\n", board->cell_h, board->cell_w);
	
	/* go over rows */
	for(y = 0; y < board->cell_w * board->cell_h; y++){
		/* go over columns */
		for(x = 0; x < board->cell_w * board->cell_h; x++){
			if(x != 0) fprintf(file, " "); /* separator space */
			fprintf(file, "%d", board->table[y][x]);
			/* if all_fixed is true, position is marked fixed if it is not empty */
			if((all_fixed && board->table[y][x]!=0) || game->fixed[y][x]) fprintf(file, "."); /* mark fixed cells */
		}
		fprintf(file, "\n"); /* end line */
	}
	
	
	fclose(file);
	return true;
}

Game* load_board(char* filename, bool use_fixed){
	Game* game;
	int cell_w, cell_h,pos;
	FILE* file = fopen(filename, "r");
	
	if(file == NULL){
		fprintf(stderr, "Error: File doesn't exist or cannot be opened\n");
		return NULL; /* unsuccessful in opening file */
	}
	
	if(fscanf(file, "%d%d", &cell_h, &cell_w) != 2){
		fprintf(stderr, "Error: fscanf has failed\n");
		fclose(file);
		return NULL;
	}

	game = create_game(cell_w, cell_h);
	
	if(game == NULL){
		fclose(file);
		return NULL; /* unsuccessful allocation */
	}
	
	
	/* saving oreder is same as oreder in memory */
	for(pos = 0; pos < cell_w * cell_h * cell_w * cell_h; pos++){
		char fixed_marker;
		if(fscanf(file,"%d%c", (game->current_state->board->memory) + pos, &fixed_marker) != 2){ /* get number and character after it (could be fixed marker) */
			fprintf(stderr, "Error: fscanf has failed\n");
			free_game(game);
			fclose(file);
			return NULL;
		}
		
		if(use_fixed && fixed_marker == '.') game->memory[pos] = true; /* mark position as fixed */ 
	}
	
	return game;
}

int get_game_size(Game* game){
	Board* b = game->current_state->board;
	return b->cell_w * b->cell_h;
}

