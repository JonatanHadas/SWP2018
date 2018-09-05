#include "game_adv.h"

#include <stdlib.h> /* malloc, rand */

#include <stdio.h>

Board* autofill(Board* board){
	Board* new_board;
	int* values; /* legal values for position */
	int x,y; /* for loops */
	int N = board->cell_w * board->cell_h;
	int num_changes = 0;
	
	values = calloc(N,sizeof(int));
	if(values == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		return NULL;
	}
	
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

bool hint(Game* g, int x, int y){
	Board* board = g->current_state->board; /* get currnet board */
	Board* sol; /* solution */
	
	/* check for immediate errors */
	if(check_board(board)){
		fprintf(stderr, "Error: board contains erroneous values\n");
		return false;
	}
	/* cell must be empty and non fixed */
	if(g->fixed[y][x]){
		fprintf(stderr, "Error: cell is fixed\n");
		return false;
	}
	
	if(board->table[y][x] != 0){
		fprintf(stderr, "Error: cell already contains a value\n");
		return false;
	}
	sol = solve(board);
	if(sol == NULL){
		return true; /* some error in solution */
	}
	if(sol == board){ /* unsolvable */
		fprintf(stderr, "Error: board is unsolvable\n");
		return false;
	}
	
	printf("Hint: set cell to %d\n", sol->table[y][x]); /* print hint */
	
	free_board(sol);
	return false;
}

/*
given an array "arr", of length "len"

randomly choose "num" values in a certain oreder, and move them to begining of "arr"
*/
void random_choose(int* arr, int len, int num){
	int i;
	for(i = 0; i<num; i++){
		int temp, ind;
		/* choose position which was not chosen yet */
		ind = i + rand() % (len-i); /* random number in range i,...,len-1 */
		/* swap with i'th position */
		temp = arr[ind];
		arr[ind] = arr[i];
		arr[i] = temp;
	}
}

/*
number of attempts to generate board
*/
#define MAX_GEN_ATTEMPTS 1000

Board* generate(Board* b, int add, int remaining){
	Board* new_board, *sol;
	int* positions; /* array of positions in board memory */
	int N = b->cell_w*b->cell_h;
	int i, attempt;
	int* values; /* legal values for positions */
	
	/*
	allocate memory
	*/
	values = calloc(N,sizeof(int));
	if(values == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		return NULL;
	}

	positions = calloc(N*N,sizeof(int));
	if(positions == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		free(values);
		return NULL;
	}
	
	new_board = copy_board(b);
	if(new_board == NULL){
		free(values);
		free(positions);
		return NULL;
	}
	
	/* positions in board memory are 0,...,N*N-1 */
	for(i = 0;i<N*N;i++) positions[i]=i;
	
	for(attempt = 0; attempt < MAX_GEN_ATTEMPTS; attempt++){
		
		for(i=0; i<N*N;i++) new_board->memory[i] = 0; /* clear board */
		
		/* choose positions to set */
		random_choose(positions, N*N, add);
		
		/* set chosen positions */
		for(i=0; i<add;i++){
			int x,y,num_legal;
			
			x = positions[i] % N;
			y = positions[i] / N;
			
			num_legal = count_legal_values(new_board, x,y, values);
			
			if(num_legal == 0){
				break; /* attempt failed: no legal value */
			}
			
			/* set position to legal value */
			new_board->memory[positions[i]] = values[rand() % num_legal];
			
		}
		if(i<add) break; /* attempt failed: a position had no legal value */
		
		sol = solve(new_board);
		if(sol == NULL){ /* error */
			free(values);
			free(positions);
			free_board(new_board);
			return NULL;
		}
		
		if(sol != new_board){ /* success */
			free(values);
			free_board(new_board);
			
			/* choose remaining positions */
			random_choose(positions, N*N, remaining);
			/* go over all other positions and clear them */
			for(i=remaining; i<N*N; i++) sol->memory[positions[i]] = 0;
			
			free(positions);
			return sol;
		}
	}
	
	
	/* failed */
	free(values);
	free(positions);
	free_board(new_board);
	
	return b;	
}
