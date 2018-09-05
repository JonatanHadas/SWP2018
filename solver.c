#include "solver.h"

#include <stdlib.h> /* malloc */

bool count_solutions(Board* board, int* number){
	int empty_num; /* number of empty places */
	
	/*
	each simulated recursive call gets the next empty position as parameters
	
	these will be saved to x_stack and y_stack (x and y coordinates)
	*/
	int* x_stack;
	int* y_stack;
	int stack_head = 0; /* position of stack head */
	int count = 0;  /* solution counter */
	
	
	if(check_board(board)) {
		number = 0;
		return true;
	} /* board is erronous */
	
	x_stack = calloc(board->cell_w * board->cell_h * board->cell_w * board->cell_h, sizeof(int));
	if(x_stack == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		return false;
	}
	
	y_stack = calloc(board->cell_w * board->cell_h * board->cell_w * board->cell_h, sizeof(int));
	if(y_stack == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		free(x_stack);
		return false;
	}
	
	/* positions on the stack always remain the same, so we will calculate them before the recursion */
	empty_num = count_empty_places(board, x_stack, y_stack);
		
	while(stack_head >= 0){ /* as long as stack is not empty */
		if(stack_head == empty_num){
			count++; /* board full */
			stack_head--; /* go back */
		}
		else{
			/* increase current position */
			board->table[y_stack[stack_head]][x_stack[stack_head]] ++;
			
			
			if(board->table[y_stack[stack_head]][x_stack[stack_head]] > board->cell_w * board->cell_h){
				/* all options for position were checked */
				board->table[y_stack[stack_head]][x_stack[stack_head]] = 0; /* reset as empty */
				stack_head--; /* go back */
			}
			else{
				if(!check_position(board, x_stack[stack_head], y_stack[stack_head])){
					/* if no errors were created, go to next position */
					stack_head++;
				}
				/* otherwise next iteration will check next option for this position */
			}
		}
	}
	
	free(x_stack);
	free(y_stack);
	
	number = count;
	return true; /* success */
}

Board* solve(Board* board){
	Board* new_board;
	
	int empty_num; /* number of empty places */
	
	/*
	each simulated recursive call gets the next empty position as parameters
	
	these will be saved to x_stack and y_stack (x and y coordinates)
	*/
	int* x_stack;
	int* y_stack;
	int stack_head = 0; /* position of stack head */
	
	if(check_board(board)) return board; /* board is erronous */
	new_board = copy_board(board); /* copy_board */
	
	if(new_board == NULL) return NULL;
	
	x_stack = calloc(board->cell_w * board->cell_h * board->cell_w * board->cell_h, sizeof(int));
	if(x_stack == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		free_board(new_board);
		return NULL;
	}
	y_stack = calloc(board->cell_w * board->cell_h * board->cell_w * board->cell_h, sizeof(int));
	if(y_stack == NULL){
		fprintf(stderr,"Error: calloc has failed\n");
		free(x_stack);
		free_board(new_board);
		return NULL;
	}
	

	/* positions on the stack always remain the same, so we will calculate them before the recursion */
	empty_num = count_empty_places(new_board, x_stack, y_stack);
	
	while(stack_head >= 0){ /* as long as stack is not empty */
		if(stack_head == empty_num){
			free(x_stack);
			free(y_stack);
			return new_board;
		}
		else{
			/* increase current position */
			new_board->table[y_stack[stack_head]][x_stack[stack_head]] ++;
			
			
			if(new_board->table[y_stack[stack_head]][x_stack[stack_head]] > board->cell_w * board->cell_h){
				/* all options for position were checked */
				new_board->table[y_stack[stack_head]][x_stack[stack_head]] = 0; /* reset as empty */
				stack_head--; /* go back */
			}
			else{
				if(!check_position(new_board, x_stack[stack_head], y_stack[stack_head])){
					/* if no errors were created, go to next position */
					stack_head++;
				}
				/* otherwise next iteration will check next option for this position */
			}
		}
	}
	
	/* no solution found */
	free(x_stack);
	free(y_stack);
	free_board(new_board);
	return board;
}
