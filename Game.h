#ifndef GAME_H_
#define GAME_H_

#include "GameAux.h"
#include "Solver.h"
#include "Game.h"

/**
 * Game Summary:
 *
 * implement all function used for playing the puzzle
 *
 * play           - plays the game
 * executeCommand - calls function to execute command as inserted by user
 * solve          - uploads board from file
 * edit           - uploads board from file or initializing default board
 * markErrors     - changes the mark_errors bit
 * set            - sets index value as inserted by user
 * undo           - undoing last move
 * redo           - redoing next move
 * save           - saves board to given path
 * reset          - resets the game
 * exiFunc        - terminates the game
 */

void play();

bool executeCommand(sudoku* puzzle, command cmd, operation mode);

bool solve(sudoku* puzzle, char* fileName, operation mode);

bool edit(sudoku* puzzle, char* fileName, operation mode);

void markErrors(sudoku* puzzle, int mark);

void set(sudoku* puzzle, int x, int y, int z, operation mode, bool addToMovesList);

bool undo(sudoku* puzzle, operation mode, bool printMsg);

void redo(sudoku* puzzle, operation mode);

bool save(sudoku* puzzle, operation mode, char* fileName);

void reset(sudoku* puzzle, operation mode);

void exitFunc(sudoku* puzzle, operation mode);

#endif /* GAME_H_ */
