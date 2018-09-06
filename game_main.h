#ifndef _GAME_MAIN_H
#define _GAME_MAIN_H

/*
game mainloop module, implements gameplay and user interaction
*/

#include <stdbool.h>
#include "game_adv.h"
#include "parser.h"

/*
read int from str to out, in given limits (lower-upper)
return whether successful
if not print error message
*/
bool get_num_lim(char* str, int* out, int lower, int upper);

/*
read boolean from str to out
return whether successful
if not print error message
*/
bool get_bool(char* str, bool* out);

/*
this structure contains all game information
*/
typedef struct game_state_struct{
	GameMode mode;
	Game* game;
	bool mark_errors;
} GameState;


/*
initialize given game state
*/
void set_init(GameState* state);

/*
open game in solve mode
given game state to use, and filename

returns true if fatal error occurred
*/
bool open_solve(GameState* state, char* filename);

/*
open game in edit mode
given game state to use, and filename

returns true if fatal error occurred
*/
bool open_edit(GameState* state, char* filename);

/*
open default game in edit mode on given game state

returns true if fatal error occurred
*/
bool open_default(GameState* state);

/*
print board of given game state
*/
void print_game(GameState* state);

/*
validate current state of game state and print results

returns true if fatal error occurred
*/
bool validate(GameState* state);

/*
prints number of solutions or error message if error occures

returns true on fatal error
*/
bool print_solution_num(GameState* state);

/*
tries to set position on given game

x,y (coordiantes) and z (value) are given as integers

returns true if fatal error occurred

prints board on success

if board is complete go back to init mode
*/
bool try_set(GameState* state, int x, int y, int z);

/*
tries to undo/redo one move in given game state
if successful prints board and changes, else prints error message

returns true on fatal error
*/
bool try_undo(GameState* state);
bool try_redo(GameState* state);

/*
resets game to first state
*/
void reset(GameState* state);

/*
saves board to given file
on edit mode saves only valid boards

returns true on fatal error
*/
bool save_game(GameState* state, char* filename);

/*
if board is empty generate puzzle
print all errors

adds "add" random values, solves, removes all but "remain" values

on fatal error return true
*/
bool try_generate(GameState* state, int add, int remain);

/*
tries to autofill board

on fatal error return true
*/
bool try_autofill(GameState* state);

#endif