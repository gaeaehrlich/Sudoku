#ifndef GAMEAUX_H_
#define GAMEAUX_H_

#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "structures.h"
#include "Parser.h"
#include "MainAux.h"

/**
 * MainAux Summary:
 *
 * implement all auxiliary functions for game:
 *
 * freeSudoku          - frees allocated sudoku
 * freeMovesList       - frees allocated moves list
 * createEmptySudoku   - created default sudoku
 * isLegalFixedCell    - checks if values of fixed cells is legal
 * fillCell            - fills cell with given value
 * findCol             - calculates column of fixed value
 * findRow             - calculates row of fixed cell
 * handleDot           - interprets dot written in file
 * fillCellsFromFile   - reads file to get board values
 * readBoardDim        - reads file to get board dim
 * initializeMovesList - initialized sudoku's moves list
 * getPuzzleFromFile   - reads sudoku from file
 * writeToFile         - writes board to file
 * unFix               - sets all cells to be un-fixed
 * finishUndo		   - finishes undo function
 *
 */


void freeSudoku(sudoku* puzzle, int prevn, int prevm);

void freeMovesList(node* moves);

void createEmptySudoku(sudoku* puzzle, operation mode, int prevn, int prevm);

bool isLegalFixedCell(sudoku* puzzle, int col, int row);

void fillCell(sudoku* puzzle, int val, int i, int j);

int findCol(int i, int N);

int findRow(int i, int j);

bool handleDot(sudoku* puzzle, char prev, int i, int j);

bool fillCellsFromFile(sudoku* puzzle, FILE *file);

bool readBoardDim(sudoku* puzzle, FILE *file);

cell** copyBoard(cell** board, int n, int m);

void initializeMovesList(sudoku* puzzle);

bool getPuzzleFromFile(sudoku* puzzle, FILE *file, operation mode);

bool writeToFile(sudoku* puzzle, FILE* file, operation mode);

void unFix(sudoku* puzzle);

bool finishUndo(sudoku* puzzle, list* currChange, operation mode, bool printMsg);

#endif /* GAMEAUX_H_ */
