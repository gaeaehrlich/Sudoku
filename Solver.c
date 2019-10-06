#include "Solver.h"

int autofill(sudoku* puzzle, operation mode, bool addToMovesList) {
	int i, j, countChanges = 0, n = puzzle -> n, m = puzzle ->m;
	int singleVal;
	cell** originalBoard = copyBoard(puzzle -> board, n, m);
	int* values = malloc((n*m + 1)*sizeof(int));
	if(values == NULL) fatalExit();
	if(isErroneous(puzzle)) {
		if(addToMovesList) {
			printf("AUTOFILL failed: The board is erroneous and therefore is not solvable.\n");
		}
		free(values);
		freeBoard(originalBoard, n, m);
		return -1;
	}
	if(addToMovesList) addEmptyMove(puzzle);
	for(i = 0; i < n*m; i++) {
		for(j = 0; j < n*m; j++) {
			if(puzzle -> board[i][j].isFixed || puzzle -> board[i][j].val > 0) continue;
			findPossibleValuesCell(originalBoard, n, m, values, i, j);
			singleVal = isSingleValue(values, n*m + 1);
			if(singleVal) {
				setCell(puzzle, i, j, singleVal);
				if(addToMovesList) {
					addChange(puzzle, i, j, singleVal, 0);
					printf("Autofilling cell <%d,%d> to %d.\n", i+1, j+1, singleVal);
				}
				countChanges++;
			}
		}
	}
	if(addToMovesList) {
		print(*puzzle, mode);
	}
	free(values);
	freeBoard(originalBoard, n, m);
	return countChanges;
}

int fillAllWithAutofill(sudoku *puzzle) {
	int changes = 1;
	while(changes > 0) {
		changes = autofill(puzzle, 0, false);
	}
	return changes;
}

int validate(sudoku* puzzle, bool print) {
	sudoku *copy;
	int succ;
	if(isErroneous(puzzle)) {
		if(print) {
			printf("VALIDATION failed: The board is erroneous and therefore is not solvable.\n");
		}
		return false;
	}
	copy = copySudoku(puzzle);
	succ = fillAllWithAutofill(copy);
	if(succ != -1) {
		if(isSolved(copy)) {
			succ = 1;
		}
		else {
			succ = linearProgrammingManager(copy, VALIDATE, NULL, 0);
		}
	}

	if(print) {
		switch(succ) {
		case 1:
			printf("VALIDATION passed: The board is solvable\n");
			break;
		case 0:
			printf("VALIDATION failed: An error occurred.\n");
			break;
		case -1:
			printf("VALIDATION failed: The board is not solvable.\n");
			break;
		}
	}

	freeSudokuCopy(copy);
	return succ;
}

bool guess(sudoku* puzzle, float x) {
	sudoku *copy;
	int succ;

	if(isErroneous(puzzle)) {
		printf("GUESS failed: The board is erroneous and therefore is not solvable.\n");
		return false;
	}

	copy = copySudoku(puzzle);
	succ = fillAllWithAutofill(copy);
	if(succ != -1) {
		if(isSolved(copy)) { succ = 1; }
		else { succ = linearProgrammingManager(copy, GUESS, NULL, x); }
	}

	if(succ == 1) {
		addBoardToMovesList(puzzle, copy);
	}
	else {
		printf("GUESS failed: An error occurred.\n");
	}
	freeSudokuCopy(copy);
	return succ > 0;
}


bool generate(sudoku* puzzle, int x, int y) {
	int succ;
	int i, N = dim(puzzle);
	int numCellsToReset = N*N - y;
	sudoku *copy;

	if(numEmptyCells(puzzle) < x) {
		printf("GENERATE failed: The board does not have %d empty cells.\n", x);
		return false;
	}
	if(numCellsToReset < 0) {
		printf("GENERATE failed: The board does not have %d cells to reset.\n",y);
		return false;
	}

	if(isErroneous(puzzle)) {
		printf("GENERATE failed: The board is erroneous and therefore is not solvable.\n");
		return false;
	}
	succ = validate(puzzle, false);
	if(succ == -1) {
		printf("GENERATE failed: The board is not solvable.\n");
		return false;
	}
	else if(succ == 0) {
		printf("GENERATE failed: An error occurred.\n");
		return false;
	}

	copy = copySudoku(puzzle);
	for(i = 0; i < 1000; i++) {
		succ = intializeCells(copy, x);
		if(succ) {
			succ = fillAllWithAutofill(copy);
			if(succ != -1) {
				if(isSolved(copy)) { succ = 1; }
				else { succ = linearProgrammingManager(copy, GENERATE, NULL, 0); }
			}

			if(succ == 1) {
				resetCells(copy, numCellsToReset);
				addBoardToMovesList(puzzle, copy);
				freeSudokuCopy(copy);
				return true;
			}
		}
		freeBoard(copy -> board, puzzle -> n, puzzle -> m);
		copy -> board = copyBoard(puzzle -> board, puzzle -> n, puzzle -> m);
	}
	printf("GENERATE failed: An error occurred.\n");
	freeSudokuCopy(copy);
	return false;
}

bool hint(sudoku* puzzle, int x, int y) {
	int succ;
	data d;
	sudoku *copy;

	if(isErroneous(puzzle)) {
		printf("HINT failed: The board is erroneous and therefore is not solvable.\n");
		return false;
	}
	if(puzzle -> board[x][y].isFixed) {
		printf("HINT failed: Cell <%d,%d> is fixed.\n", x + 1, y + 1);
		return false;
	}
	if(puzzle -> board[x][y].val > 0) {
		printf("HINT failed: Cell <%d,%d> already contains a value.\n", x + 1, y + 1);
		return false;
	}

	setData(&d, x, y);
	copy = copySudoku(puzzle);
	succ = fillAllWithAutofill(copy);
	if(succ != -1) {
		if(isSolved(copy)) { succ = copy -> board[x][y].val; }
		else { succ = linearProgrammingManager(copy, HINT, &d, 0); }
	}
	freeSudokuCopy(copy);

	if(succ == -1) { /* same as using validate(copy, false), but more efficient */
		printf("HINT failed: The board is not solvable.\n");
	}
	else if(succ == 0) {
		printf("HINT failed: An error occurred.\n");
	}
	else {
		printf("Hint: set cell <%d,%d> to %d\n", x + 1, y + 1, succ);
	}
	return succ > 0;
}


bool guess_hint(sudoku* puzzle, int x, int y) {
	sudoku *copy;
	int succ;
	data d;
	if(isErroneous(puzzle)) {
		printf("GUESS_HINT failed: The board is erroneous and therefore is not solvable.\n");
		return false;
	}
	if(puzzle -> board[x][y].isFixed) {
		printf("GUESS_HINT failed: Cell <%d,%d> is fixed.\n", x + 1, y + 1);
		return false;
	}
	if(puzzle -> board[x][y].val > 0) {
		printf("GUESS_HINT failed: Cell <%d,%d> already contains a value.\n", x + 1, y + 1);
		return false;
	}

	setData(&d, x, y);
	copy = copySudoku(puzzle);
	succ = fillAllWithAutofill(copy);
	if(succ != -1) {
		if(copy -> board[x][y].val > 0) {
			succ = 1;
			printf("The options for cell <%d,%d> are:\n", x + 1, y + 1);
			printf("%d : %f\n", copy -> board[x][y].val, 1.0);
		}
		else {
			succ = linearProgrammingManager(copy, GUESS_HINT, &d, 0);
		}
	}

	if(succ != 1) {
		printf("GUESS_HINT failed: An error occurred.\n");
	}
	freeSudokuCopy(copy);
	return succ > 0;
}


int numSolutions(sudoku* puzzle) {
	int num = 0;
	if(isErroneous(puzzle)) {
		printf("Board is erroneous and therefore there are no solutions.\n");
		return 0;
	}
	if(validate(puzzle, false)) {
		num = countNumSolutions(puzzle);
	}
	printf("The number of solutions to this board is %d.\n", num);
	return num;
}

int countNumSolutions(sudoku *puzzle){
	int val;
	int N = dim(puzzle);
	int counter = 0, col = 0, row = 0;
	cell** board = puzzle -> board;
	data d;
	stack stk;

	if(N == 0) { return -1; } /* "ass cover" */

	initialize(&stk);
	nextUninitializedCell(puzzle, &col, &row);
	setData(&d, col, row);
	push(d, &stk);

	while(!empty(&stk)){
		if(row == N){ /* there are no more uninitialized cells - we found a solution for the board */
			counter++;
			pop(&stk);
			if(!empty(&stk)){
				d = top(&stk);
				updateLocation(d, &col, &row);
				board[col][row].val++;
			}
			continue;
		}

		val = board[col][row].val;

		if(val > N){ /* we checked all values for this specific cell in this specific context*/
			board[col][row].val = 0; /* returning the cell to its former empty state */
			pop(&stk);
			if(!empty(&stk)){
				d = top(&stk);
				updateLocation(d, &col, &row);
				board[col][row].val++;
			}
		}
		else if(val != 0 && validLocation(puzzle, col, row, val, false)){ /* time to move up to the next uninitialzed cell */
			nextUninitializedCell(puzzle, &col, &row);
			setData(&d, col, row);
			push(d, &stk);
		}
		else{ /* finished with this value (for now). moving up to the next! */
			board[col][row].val++;
		}

	}
	return counter;

}


int linearProgrammingManager(sudoku *puzzle, operation op, data *pos, float threshold) {
	int offset, output = true;
	char type = GRB_BINARY;
	variable *vars;
	double *sol, *obj = NULL;
	int error, ***arr, size = countNumVarsForLP(puzzle);
	if(size == 0) { return -1; }

	vars = (variable*)malloc(size*sizeof(variable));
	sol = (double*)malloc(size*sizeof(double));
	if(vars == NULL || sol == NULL) { fatalExit(); }
	arr = creat3DArrayAndInitializeVars(puzzle, vars);

	if(op == GUESS || op == GUESS_HINT) {
		type = GRB_CONTINUOUS;
		obj = setObj(vars, size, puzzle -> n, puzzle -> m);
	}

	error = linearProgramming(arr, vars, sol, obj, puzzle -> n, puzzle -> m, size, type);
	if(error < 1) {
		freeLParams(arr, vars, sol, obj, dim(puzzle));
		return error;
	}

	switch(op){
	case HINT:
		offset = getOffsetForCellInVarsList(pos->col, pos->row, arr, dim(puzzle));
		if(offset == -1) { output = 0; } /*cell is not empty*/
		else { output = findCellValDerermenistic(vars, sol, offset, size); }
		break;
	case GUESS_HINT:
		offset = getOffsetForCellInVarsList(pos->col, pos->row, arr, dim(puzzle));
		printOddsForCell(puzzle, vars + offset, sol + offset, size - offset);
		break;
	case GENERATE:
		updateBoardDerermenistic(puzzle, vars, sol, size);
		break;
	case GUESS:
		updateBoardNonDetermenistic(puzzle, vars, sol, size, threshold); /* fix later */
		break;
	default:
		output = error;
		break;
	}

	freeLParams(arr, vars, sol, obj, dim(puzzle));
	return output;
}


int linearProgramming(int ***arr, variable *vars, double *sol, double *obj, int n, int m, int size, char type) {
	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;
	int       error = 0;
	int       ind[size];
	double    val[size];
	char      vtype[size];
    int       optimstatus;

    error = GRBloadenv(&env, NULL);
    if(error) {
    	if(env != NULL) { GRBfreeenv(env); }
    	return 0;
    }

    error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
    if(error) {
    	GRBfreeenv(env);
    	return 0;
    }

    error = GRBnewmodel(env, &model, "lp", 0, NULL, NULL, NULL, NULL, NULL);
    if(error) {
    	GRBfreeenv(env);
    	if(model != NULL) { GRBfreemodel(model);}
    	return 0;
    }

    setCharArray(vtype, size, type);

    error = GRBaddvars(model, size, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
	if(error) {
		freeGRB(model, env);
	    return 0;
	}
    /* Change objective sense to minimization*/
    error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);
	if(error) {
		freeGRB(model, env);
	    return 0;
	}

    error = GRBupdatemodel(model);
	if(error) {
		freeGRB(model, env);
	    return 0;
	}

    setDoubleArray(val, size, 1);

	error = addConstraints(model, env, ind, vars, arr, val, n, m, size);
	if(error) {
		freeGRB(model, env);
	    return 0;
	}

    error = GRBoptimize(model);
    if(error) {
    	freeGRB(model, env);
    	return -1;
    }

    error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
    if(error) {
    	freeGRB(model, env);
    	return -1;
    }

    error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, size, sol);
    if(error) {
    	freeGRB(model, env);
    	return -1;
    }

    freeGRB(model, env);
    if(optimstatus == GRB_OPTIMAL) { return 1; }
    return -1;
}

int addCellConstraints(GRBmodel *model, GRBenv *env, int *ind, variable *vars, double *val, int size) {
    int num_var, error, offset = 0;
	while(offset < size) {
    	num_var = setIndexCell(vars, ind, size, offset);
	    error = GRBaddconstr(model, num_var, ind, val, GRB_EQUAL, 1.0, NULL);
	    if(error) { break; }
	    offset += num_var;
    }
	return error;
}

int addConstraints(GRBmodel *model, GRBenv *env, int *ind, variable *vars,int ***arr,
		double *val, int n, int m, int size) {
	int i, j, N = n*m, num_var, error;
	for(i = 0; i < N ; i++) {
	    	for(j = 0; j < N; j++) {
	    		/* col constraint*/
	    		num_var = setIndexCol(arr, ind, N, i, j);
			    if(num_var > 0) {
			    	error = GRBaddconstr(model, num_var, ind, val, GRB_EQUAL, 1.0, NULL);
			  	    if(error) { return 0; }
			    }
	    		/* row constraint*/
			    num_var = setIndexRow(arr, ind, N, i, j);
			    if(num_var > 0) {
			    	error = GRBaddconstr(model, num_var, ind, val, GRB_EQUAL, 1.0, NULL);
			  	    if(error) { return 0; }
			    }
	    		/* block constraint*/
			    num_var = setIndexBlock(arr, ind, n, m, i, j);
			    if(num_var > 0) {
			    	error = GRBaddconstr(model, num_var, ind, val, GRB_EQUAL, 1.0, NULL);
			  	    if(error) { return 0; }
			    }
	    	}
	    }
    /*cell constraints*/
	return addCellConstraints(model, env, ind, vars, val, size);
}

void freeGRB(GRBmodel *model, GRBenv *env) {
    GRBfreemodel(model);
    GRBfreeenv(env);
}