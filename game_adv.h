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

on error, returns NULL
on failure, returns "b"

assumes board is empty
*/
Board* generate(Board* b, int add, int remaining);

/*
tries to print hint for given board on given position

if no hint can be made, prints message explaining why

return true of fatal error
*/
bool hint(Game* g, int x, int y);

/*
given a non erronous board, returns a copy with obvious values added
on error (in allocation) returns NULL

if no changes are made, returns board
*/
Board* autofill(Board* b);

#endif