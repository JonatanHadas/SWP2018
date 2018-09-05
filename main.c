#include "game_adv.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(){
	Game* game = create_game(2,2);
	Board* b;
	Board* s;
	
	srand(time(NULL));
		
	b = game->current_state->board;	
	add_state(game);
	
	printf("generating\n");
	s = generate(b, 5,7);
	if(s==b) printf("failed\n");
	if(s == NULL) printf("error\n");
	else replace_node_board(game->current_state,s);
	
	print_board(game,true);
	
	free_game(game);
	
	return 0;
}