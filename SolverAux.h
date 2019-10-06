#ifndef SOLVERAUX_H_
#define SOLVERAUX_H_

#include "MainAux.h"
#include <time.h>

/**
 * SolverAux Summary:
 *
 * implement all auxiliary functions for solver:
 *
 * forward                       - given a cell, finds the next cell
 * nextUninitializedCell         - given a cell, finds the next cell with value 0
 * updateLocation                - updates data to given values (representing a cell)
 * creat3DArrayAndInitializeVars - creates a 3D array used for the linear programming and initialize the "vars"
 *                                 array, also used for the linear programming.
 * free3DArray                   - free the 3D array created for the linear programming
 * setIndexCol                   - given the Gurobi "ind" array, a column and a value between 0 and N, set
 *                                 the "ind" array for a constraint on the given column and value
 * setIndexRow                   - given the Gurobi "ind" array, a row and a value between 0 and N, set
 *                                 the "ind" array for a constraint on the given row and value
 * setIndexBlock                 - given the Gurobi "ind" array a block and a value between 0 and N, set
 *                                 the "ind" array for a constraint on the given block and value
 * setIndexCell                  - given the Gurobi "ind" array, a cell and a value between 0 and N, set
 *                                 the "ind" array for a constraint on the given cell and value
 * countNumVarsForLP             - given a puzzle, returns the number of variables we need for Gurobi
 * restartRandomVal              - given a cell, fills the cell with a random valid value (if such exists).
 *                                 return true if succeed, else false
 * intializeCells                - given a puzzle and a number x, fills x random cells with a random valid
 *                                 number. return true if succeed, else false
 * resetCells                    - given a puzzle and a number y, reset y random to 0
 *                                 return true if succeed, else false
 * findCellValDerermenistic      - using the Gurobi output, finds value for a given cell (for HINT)
 * updateBoardDerermenistic      - unsing the Gurobi output, fills all empty cells (for GENERATE)
 * initializeVtype               - sets vtype array with a given char
 * findOptionsForCell            - using Gurobi output and a threshold, set to arrays: optionsVals and options
 *                                 such that in the i'th location: optionsVal keeps a possible solutions,
 *                                 and options keeps the odds of this solutions being true;
 * guessCellVal                  - using Gurobi output, guesses a solutions for a given cell
 * updateBoardNonDetermenistic   - using Gurobi output, fills empty cells with random vals with a score grater
 *                                 or equal to a given threshold (for GUESS)
 * printOddsForCell              - using Gurobi output, prints all possible solutions of a given cell and their scores
 * freeLParams                   - frees allocated Gurobi memory (GRBmodel and GRBenv)
 *
 * setObj                        - sets the Gurobi "obj" array with the following logic:
 *                                 first, every variable is given a random number between 1 and N
 *                                 later, for every variable, we multiply each cell with 2 to the
 *                                 power of s, when s is the number of "collisions" with other variables
 *                                 why?
 *                                 without giving different coefficients to our variables, Gurobi will give every
 *                                 one the score of 1/N, and we don't want that. we wan't to force Gurobi to give some
 *                                 variables large scores, and others small scores. when the board is empty, we have
 *                                 no way to choose this coefficients deterministically - hence the first step.
 *                                 also, we do want to give smaller values to coefficients not likely to be part
 *                                 of many solutions to the board - hence the second step.
 *
 */

void forward(int N, int *col, int *row);

void nextUninitializedCell(sudoku* puzzle, int *col, int *row);

void updateLocation(data d, int *col, int *row);

int*** creat3DArrayAndInitializeVars(sudoku *puzzle, variable *vars);

void free3DArray(int*** arr, int N);

int setIndexCol(int ***arr, int *ind, int N, int val, int col);

int setIndexRow(int ***arr, int *ind, int N, int val, int row);

int setIndexBlock(int ***arr, int *ind, int n, int m, int val, int block);

int setIndexCell(variable *vars, int *ind, int size, int offset);

int countNumVarsForLP(sudoku *puzzle);

bool restartRandomVal(sudoku *puzzle, int col, int row);

bool intializeCells(sudoku *puzzle, int x);

void resetCells(sudoku *puzzle, int y);

int findCellValDerermenistic(variable *vars, double *sol, int offset, int size);

void updateBoardDerermenistic(sudoku *puzzle, variable *vars, double *sol, int size);

double* setObj(variable *vars, int size, int n, int m);

int findOptionsForCell(sudoku *puzzle, int *optionsVals, variable *vars, double *sol, double *options
		, int numVarsCell, double threshold);

int guessCellVal(sudoku *puzzle, variable *vars, double *sol, int len, double threshold);

void updateBoardNonDetermenistic(sudoku *puzzle, variable *vars, double *sol, int size, double threshold);

void printOddsForCell(sudoku *puzzle, variable *vars, double *sol, int size);

void freeLParams(int ***arr, variable *vars, double *sol, double *obj, int N);

#endif /* SOLVERAUX_H_ */
