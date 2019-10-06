#ifndef MAINAUX_H_
#define MAINAUX_H_

#include "structures.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "SPBufferset.h"

/**
 * MainAux Summary:
 *
 * implement all auxiliary functions for all source files
 *
 * Functions for structures:
 * 		setData     - sets given data to given values
 * 		setVariable - sets given variable to given values
 * 		push        - adds element (data) to the top of the stack
 * 		pop         - returns and deletes top element on stack
 *		top         - returns the top of the stack
 *	 	empty       - returns true iff the stack is empty
 *		initialize  - initializes default values to stack
 * 		equal       - returns true iff the given datas are equal
 *
 * Functions for moves list:
 * 		creatChange         - creates change made in a move
 *		addChange           - adds change to the list of changes
 * 		addMove             - adds move and change to the moves list
 * 		addEmptyMove        - adds an empty move to the moves list
 * 		freeChangesList     - frees allocated changes list
 * 		freeNextMoves       - frees allocated moves list
 * 		addBoardToMovesList - calculates difference between boards and adds changes to move
 *
 *
 * Functions on game board:
 * 		setCell          - sets value of given cell to a given number
 * 		createEmptyBoard - allocates new board in given dim
 * 		copyBoard        - returns copy of board
 * 		copySusoku       - returns copy of sudoku
 * 		freeSudokuCopy   - frees sudoku copy (pointer)
 * 		freeBoard        - frees allocated board
 * 		unFix            - sets all cells to be un-fixed
 *
 * Functions for validation:
 * 		initializeValuesArr   - initializes array of values
 * 		findPossibleValCell   - finds all possible values for a given cell
 * 		isSingleValue         - checks if there is only one possible single value for cell
 * 		handlePrevErrCell     - handles errors after changing cell's value
 * 		validLocationCol      - checks if a value is legal in given cell column-wise
 * 		validLocationRow      - checks if a value is legal in given cell row-wise
 * 		validLocationBlock    - checks if a value is legal in given cell block-wise
 * 		validLocation         - checks if a value is legal in given cell
 * 		isSolved              - checks is board is legally solved
 * 		isErroneous           - checks if there is an erroneous cell in board
 * 		countLegalValsForCell - returns the number of legal values for given cell
 * 		legalValsForCell      - returns array of possible legal values for given cell
 *
 * General:
 * 		dim                        - returns n*m
 * 		print                      - prints the board
 * 		blockCorner                - finds the block corner coordinates of a given cell
 * 		blockNum                   - finds the block number coordinates of a given cell
 * 		fatalExit                  - exits program unsafely due to a fatal error
 * 		numEmptyCells              - returns the number of empty cells in the puzzle
 * 		numNonEmptyCells           - returns the number of non empty cells in the puzzle
 * 		emptyCellsList             - returns a list of all empty cells in the puzzle
 * 		nonEmptyCellsList          - returns a list of all non empty cells in the puzzle
 * 		setDoubleArray             - inserts given double array given value
 * 		setCharArray               - inserts given char array given value
 * 		variableMatch              - for 2 given variables, returns if both can be in the same board
 * 		generateRandmNum           - returns random number in range 0 to given parameter
 * 		numVarsForCell             - given array of variables, returns the number variables with same data
 * 		                             as the first variable in the array.
 * 		                             [same result as using countLegalValsForCell, but faster]
 * 		toSumArray                 - takes a given double* array and turns it into it's own array of partial sums.
 * 		getOffsetForCellInVarsList - given a data, finds the index of the first variable with same data at "vars" array
 *
 */

void initialize(stack *stk);

void push(data d, stack *stk);

data pop(stack *stk);

data top(stack *stk);

bool empty(stack *stk);

bool equal(data d1, data d2);

void setData(data *d, int col, int row);

void setVariable(variable *var, int col, int row, int val);

int dim(sudoku *puzzle);

void print(sudoku puzzle, operation mode);

void creatChange(list* change, int col, int row, int currVal, int prevVal);

void addChange(sudoku* puzzle, int col, int row, int currVal, int prevVal);

void addMove(sudoku* puzzle, int col, int row, int currVal, int prevVal);

void addEmptyMove(sudoku* puzzle);

void freeChangesList(list* changes);

void freeNextMoves(node* moves);

bool validLocationCol(sudoku* puzzle, int x, int y, int z, bool changeErr);

bool validLocationRow(sudoku* puzzle, int x, int y, int z, bool changeErr);

bool validLocationBlock(sudoku* puzzle, int x, int y, int z, bool changeErr);

bool validLocation(sudoku* puzzle, int x, int y, int z, bool changeErr);

bool isSolved(sudoku* puzzle);

bool isErroneous(sudoku *puzzle);

int numEmptyCells(sudoku* puzzle);

int numNonEmptyCells(sudoku* puzzle);

data* emptyCellsList(sudoku* puzzle);

data* nonEmptyCellsList(sudoku* puzzle);

void setDoubleArray(double *array, int size, double val);

void setCharArray(char *array, int size, char val);

/*did not use it, but will keep it just in case */
void blockCorner(int n, int m, int block, data *d);

int blockNum(int n, int m, data d); /* indexed the blocks in the puzzle */

bool variableMatch(variable v1, variable v2, int n, int m);

double generateRandmNum(double max);

cell** createEmptyBoard(int n, int m);

cell** copyBoard(cell** board, int n, int m);

sudoku* copySudoku(sudoku *source);

void freeSudokuCopy(sudoku *source);

void freeBoard(cell** board, int n, int m);

int countLegalValsForCell(sudoku *puzzle, int x, int y);

int* legalValsForCell(sudoku *puzzle, int x, int y);

int numVarsForCell(variable *vars, int len);

void addBoardToMovesList(sudoku *puzzle, sudoku *copy);

void toSumArray(double *options, int size);

void fatalExit();

int getOffsetForCellInVarsList(int col, int row, int ***arr, int N);

void setCell(sudoku* puzzle, int x, int y, int z);

void initializeValArr(int* arr, int dim);

void findPossibleValuesCell(cell** board, int n, int m, int* values, int col, int row);

int isSingleValue(int* arr, int size);

void handlePrevErrCell(sudoku* puzzle, int x, int y, int prevVal);

void unFix(sudoku* puzzle);

#endif /* MAINAUX_H_ */
