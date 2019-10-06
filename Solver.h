#ifndef SOLVER_H_
#define SOLVER_H_

#include <stdbool.h>
#include <stdio.h>
#include "SolverAux.h"
#include "gurobi_c.h"


/**
 * Solver Summary:
 *
 * implement all function used for solving the puzzle
 *
 * autofill                 - autofills cells in board that only have one solution
 * fillAllWithAutofill      - uses autofill again and again, in order to fill all cells with only one possible value.
 *                            stops only after autofill stops changing the puzzle.
 * validate                 - checks if the board has a solution using ILP
 * guess                    - given a puzzle and a number x, tries to find a solutions for the board using LP
 *                            every cell can be filed only with a value with a score >= x
 * generate                 - given a puzzle, randomly fills x empty cells, finds a solution for the puzzle,
 *                            and resets y cells to 0
 * hint                     - given a cell, print a solution for the cell (if such exist)
 * guess_hint               - given a cell, print all possible solutions for the cell, and their scores
 * numSolutions             - counts the number of solutions the given puzzle
 * countNumSolutions        - simulates the recursive backtracking algorithm
 * linearProgrammingManager - given a puzzle and a command, set all the parameters for the LP,
 *                            and performs necessary changes on the puzzle, depending on the command
 * linearProgramming        - performs the linear programming
 *                            return 1 if succeed, 0 if an error occurred and -1 if Gurobi failed
 * addConstraints           - given a Gurobi model, add to the model the constraints
 * addCellConstraints       - given a Gurobi model, add to the model the cell constraints
 * freeGRB                  - frees the GRBmodel and the GRBenv
 */

int autofill(sudoku* puzzle, operation mode, bool addToMovesList);

int fillAllWithAutofill(sudoku *puzzle);

int validate(sudoku* puzzle, bool print);

bool guess(sudoku* puzzle, float x);

bool generate(sudoku* puzzle, int x, int y);

bool hint(sudoku* puzzle, int x, int y);

bool guess_hint(sudoku* puzzle, int x, int y);

int numSolutions(sudoku* puzzle);

int countNumSolutions(sudoku* puzzle);

int linearProgrammingManager(sudoku *puzzle, operation op, data *pos, float x);

int linearProgramming(int ***arr, variable *vars, double *sol, double *obj, int n, int m, int size, char type);

int addCellConstraints(GRBmodel *model, GRBenv *env, int *ind, variable *vars, double *val, int size);

int addConstraints(GRBmodel *model, GRBenv *env, int *ind, variable *vars,int ***arr, double *val, int n, int m, int size);

void freeGRB(GRBmodel *model, GRBenv *env);

#endif /* SOLVER_H_ */