#ifndef _GAME_ADVANCE_H
#define _GAME_ADVANCE_H

/*
advanced game module
this module contains advanced game function such as:
autofill, hint finding etc
*/

#include "solver.h"


/*
generates board by adding "add" random leagal values, solving, then removing cells until "remaining" cells remain

on failure, returns NULL
*/
Board* generate(Board* b, int add, int remaining);

/*
tries to print hint for given board on given position

if no hint can be made, prints message explaining why
*/
void hint(Board* b, int x, int y);

/*
given a non erronous board, returns a copy with obvious values added
*/
Board* autofill(Board* b);

#endif