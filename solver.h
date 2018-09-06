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
on error returns null
*/
Board* solve(Board* board);

/*
outputs number of possible solutions to given board to "number"
returns whether succeded
also prints gurobi errors
*/
bool count_solutions(Board* board, int* number);

#endif