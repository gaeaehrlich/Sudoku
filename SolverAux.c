#include "SolverAux.h"

void forward(int N, int *col, int *row){
	if(*col == N-1){
		*col = 0;
		*row += 1;
	}
	else{
		*col += 1;
	}
}

void nextUninitializedCell(sudoku* puzzle, int *col, int *row) {
	int N = dim(puzzle);
	cell** board = puzzle->board;
	while(*row < N && board[*col][*row].val != 0) {
		forward(N, col, row);
	}
}

void updateLocation(data d, int *col, int *row){
	*col = d.col;
	*row = d.row;
}

int*** creat3DArrayAndInitializeVars(sudoku *puzzle, variable *vars) {
	int N = dim(puzzle);
	cell **board = puzzle -> board;
	int i, j, k, count = 0;

	int ***arr = (int***)malloc(N*sizeof(int**));
	if(arr == NULL) { fatalExit(); }

	for(i = 0; i < N; i++) {
		arr[i] = (int**)malloc(N*sizeof(int*));
		if(arr[i] == NULL) { fatalExit(); }

		for(j = 0; j < N; j++) {
			arr[i][j] =(int*)malloc(N*sizeof(int));
			if(arr[i][j] == NULL) { fatalExit(); }
			for(k = 0; k < N; k++) {

				if(validLocation(puzzle, i, j, k + 1, false) && board[i][j].val == 0) {
					setVariable(vars + count, i, j, k + 1);
					arr[i][j][k] = count++;
				}
				else { arr[i][j][k] = -1; }
			}
		}
	}
	return arr;
}

void free3DArray(int*** arr, int N) {
	int i, j;
	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++){
			free(arr[i][j]);
		}
		free(arr[i]);
	}
	free(arr);
}

int setIndexCol(int ***arr, int *ind, int N, int val, int col) {
	int count = 0;
	int i, pos;
	for(i = 0; i < N; i++) {
		if((pos = arr[col][i][val]) != -1){
			ind[count++] = pos;
		}
	}
	return count;
}

int setIndexRow(int ***arr, int *ind, int N, int val, int row) {
	int count = 0;
	int i, pos;
	for(i = 0; i < N; i++) {
		if((pos = arr[i][row][val]) != -1){
			ind[count++] = pos;
		}
	}
	return count;
}

int setIndexBlock(int ***arr, int *ind, int n, int m, int val, int block) {
	data d;
	int count = 0;
	int i, j, pos;
	blockCorner(n, m, block, &d);
	for(i = 0; i < n; i++) {
		for( j = 0; j < m; j++) {
			if((pos = arr[i + d.col][j + d.row][val]) != -1){
				ind[count++] = pos;
			}
		}
	}
	return count;
}


int setIndexCell(variable *vars, int *ind, int size, int offset) {
	int count = 0;
	data d = vars[offset].d;
	while(offset < size && equal(vars[offset].d, d)){
		ind[count++] = offset++;
	}
	return count;
}

int countNumVarsForLP(sudoku *puzzle) {
	int i, j, k;
	int N = dim(puzzle);
	int count = 0;
	cell **board = puzzle ->board;

	for(i = 0; i < N; i++){
		for(j = 0; j < N; j++) {
			if(board[i][j].val == 0) {
				k = countLegalValsForCell(puzzle, i, j);
				if(k == 0) {return 0; } /* The board has no solution */
				count += k;
			}
		}
	}
	return count;
}

bool restartRandomVal(sudoku *puzzle, int col, int row) {
	int i, *list;
	int num = countLegalValsForCell(puzzle, col, row);
	cell **board = puzzle -> board;

	if(num == 0) { return false; }
	list = legalValsForCell(puzzle, col, row);
	i = rand() % num;
	board[col][row].val = list[i];
	free(list);
	return true;
}

bool intializeCells(sudoku *puzzle, int x) {
	data *list = emptyCellsList(puzzle);
	int i, count = 0, size = numEmptyCells(puzzle);
	data d;
	while(count < x) {
		i = rand() % size;
		d = list[i];
		if(!restartRandomVal(puzzle, d.col, d.row)) {
			free(list);
			return false;
		}
		size--;
		list[i] = list[size];
		count++;
	}
	free(list);
	return true;
}


void resetCells(sudoku *puzzle, int y) { /* asumes y < nonEmptyCells*/
	data *list = nonEmptyCellsList(puzzle);
	int i, count = 0, size = numNonEmptyCells(puzzle);
	data d;
	while(count < y) {
		i = rand() % size;
		d = list[i];
		puzzle -> board[d.col][d.row].val = 0;
		size--;
		list[i] = list[size];
		count++;
	}
	free(list);
}
/*
void resetCells(cell **board, int numCellsToReset, int N) {  Non empty cells list?
	int col, row, count = 0;
	while(count < numCellsToReset) {
		col = rand() % N;
		row = rand() % N;
		if(board[col][row].val != 0) {
			board[col][row].val = 0;
			count ++;
		}
	}
}
*/

int findCellValDerermenistic(variable *vars, double *sol, int offset, int size) {
	int i;
	for(i = offset; i < size; i++){
		if(sol[i] == 1) {
			return vars[i].val;
		}
	}
	return -1;
}

void updateBoardDerermenistic(sudoku *puzzle, variable *vars, double *sol, int size) {
	int i, val;
	cell **board = puzzle -> board;
	data d;
	for(i = 0; i < size; i++) {
		if(sol[i] == 1) {
			d = vars[i].d;
			val = vars[i].val;
			board[d.col][d.row].val = val;
		}
	}
}

double* setObj(variable *vars, int size, int n, int m) {
	int i, j;
	double *obj = (double*)malloc(size*sizeof(double));
	if(obj == NULL) { fatalExit(); }

	for(i = 0; i < size; i++) {
		obj[i] = rand()%(n*m) + 1;
		for(j = 0; j < i; j++) {
			if(!variableMatch(vars[i], vars[j], n, m)) {
				obj[i] = 2*obj[i];
				obj[j] = 2*obj[j];
			}
		}
	}
	return obj;
}

int findOptionsForCell(sudoku *puzzle, int *optionsVals, variable *vars, double *sol, double *options
		, int numVarsCell, double threshold) {
	/* vars and sol are starting from our specific cell */
	int col = vars -> d.col, row = vars -> d.row;
	int i, count = 0;
	int numLegalVals = countLegalValsForCell(puzzle, col, row);

	for(i = 0; i < numVarsCell && numLegalVals > 0; i++ ) {
		if(validLocation(puzzle, col, row, vars[i].val, false)) {
			if(sol[i] >= threshold && sol[i] > 0) {
				optionsVals[count] = vars[i].val;
				options[count] = sol[i];
				count++;
			}
			numLegalVals--;
		}
	}
	/* If all legal values left got score of zero */
	if(count == 0 && threshold == 0) {
		for(i = 0; i < numVarsCell; i++) {
			if(validLocation(puzzle, col, row, vars[i].val, false)) {
				optionsVals[count] = vars[i].val;
				options[count] = 1;
				count++;
			}

		}
	}
	return count;
}

int guessCellVal(sudoku *puzzle, variable *vars, double *sol, int numVarsCell, double threshold) {
	/* vars and sol are starting from our specific cell */
	int guess, i = 0;
	int *optionsVals = (int*)malloc(numVarsCell*sizeof(int));
	double max, randomNum, *options = (double*)malloc(numVarsCell*sizeof(double));
	int len = findOptionsForCell(puzzle, optionsVals, vars, sol, options, numVarsCell, threshold);


	if(optionsVals == NULL || options == NULL) { fatalExit();}
	if(len == 0) {
		free(options);
		free(optionsVals);
		return 0;
	}
	toSumArray(options, len);
	max = options[len - 1];
	randomNum = generateRandmNum(max);
	while(options[i] < randomNum) {
		i++;
	}
	guess = optionsVals[i];
	free(options);
	free(optionsVals);
	return guess;
}


void updateBoardNonDetermenistic(sudoku *puzzle, variable *vars, double *sol, int size, double threshold) {
	int i, offset = 0;
	int numVarsCell, numCells = numEmptyCells(puzzle);
	data d, *list = emptyCellsList(puzzle);
	cell **board = puzzle -> board;

	for(i = 0; i < numCells; i++) {
		d = list[i];
		numVarsCell = numVarsForCell(vars + offset, size - offset);
		if(countLegalValsForCell(puzzle, d.col, d.row) > 0) {
			board[d.col][d.row].val = guessCellVal(puzzle, vars + offset, sol + offset, numVarsCell, threshold);
		}
		offset += numVarsCell;
	}
	free(list);
}

void printOddsForCell(sudoku *puzzle, variable *vars, double *sol, int size) {
	/* vars and sol are starting from our specific cell, size is adjusted */
	int col = vars -> d.col, row = vars -> d.row;
	int i, numVarsCell = numVarsForCell(vars, size);
	int *optionsVals = (int*)malloc(numVarsCell*sizeof(int));
	double *options = (double*)malloc(numVarsCell*sizeof(double));
	int len = findOptionsForCell(puzzle, optionsVals, vars, sol, options, numVarsCell, 0);

	if(optionsVals == NULL || options == NULL) { fatalExit();}
	if(len == 0) {
		printf("GUESS_HINT failed: The options for cell <%d,%d> are:\n", col + 1, row + 1);
	}
	else {
		printf("The options for cell <%d,%d> are:\n", col + 1, row + 1);
		for(i = 0; i < len - 1; i++) {
			printf("%d : %f , ", optionsVals[i], options[i]);
		}
		printf("%d : %f\n", optionsVals[len - 1], options[len - 1]);
	}

	free(optionsVals);
	free(options);
}

void freeLParams(int ***arr, variable *vars, double *sol, double *obj, int N) {
	free3DArray(arr, N);
	free(vars);
	free(sol);
	free(obj);
}
