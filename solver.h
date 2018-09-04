#ifndef _SOLVER_H
#define _SOLVER_H
/*
solver module
contains functions for solving sudoku boards
*/

#include "game.h"

/*
generates a solved copy of given board, returns null pointer if no solution exists
on failure returns same pointer
on allocation error returns null
*/
Board* solve(Board* board);

/*
returns number of possible solutions to given board
*/
int count_solutions(Board* board);

#endif