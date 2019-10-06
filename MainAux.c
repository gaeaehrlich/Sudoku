
#include "MainAux.h"

void initialize(stack *stk) {
	stk -> cnt = 0;
	stk -> top = NULL;
}

void push(data d, stack *stk) {
	elem *p = NULL;
	p = (elem*)malloc(sizeof(elem));
	if(p == NULL) fatalExit();
	p -> d = d;
	p -> next = stk -> top;
	stk -> top = p;
	stk -> cnt ++;
}
data pop(stack *stk) {
	data d;
	elem *p = NULL;
	p = stk -> top;
	d = p -> d;
	stk -> top = (stk -> top) -> next;
	stk -> cnt --;
	free(p);
	return d;
}

data top(stack *stk) {
	return (stk -> top -> d);
}

bool empty(stack *stk) {
	return !(stk -> cnt);
}

bool equal(data d1, data d2) {
	return d1.col == d2.col && d1.row == d2.row;
}

void setData(data *d, int col, int row){
	if(d != NULL) {
		d -> col = col;
		d -> row = row;
	}
}


void setVariable(variable *var, int col, int row, int val) {
	if(var != NULL) {
		var -> d.col = col;
		var -> d.row = row;
		var -> val = val;
	}
}

bool validLocationCol(sudoku* puzzle, int x, int y, int z, bool changeErr) {
	int i, N = dim(puzzle);
	bool isLegal = true, isCurrCell;
	for(i = 0; i < N; i++) {
		isCurrCell = i == y;
		if(!isCurrCell && puzzle -> board[x][i].val == z) {
			isLegal =  false;
			if(changeErr && puzzle -> board[x][i].isFixed == false) {
				puzzle -> board[x][i].err = 1;
			}
		}
	}
	return isLegal;
}

bool validLocationRow(sudoku* puzzle, int x, int y, int z, bool changeErr) {
	int i, N = dim(puzzle);
	bool isLegal = true, isCurrCell;
	for(i = 0; i < N; i++) {
		isCurrCell = i == x;
		if(!isCurrCell && puzzle -> board[i][y].val == z) {
			isLegal =  false;
			if(changeErr && puzzle -> board[i][y].isFixed == false) {
				puzzle -> board[i][y].err = 1;
			}
		}
	}
	return isLegal;
}

bool validLocationBlock(sudoku* puzzle, int x, int y, int z, bool changeErr) {
	int i, j, rowStart = (x/puzzle -> n)*puzzle -> n, colStart = (y/puzzle -> m)*puzzle -> m;
	bool isLegal = true, isCurrCell;
	for(i = colStart; i < colStart + puzzle -> m; i++) {
		for(j = rowStart; j < rowStart + puzzle -> n; j++) {
			isCurrCell = j == x && i == y;
			if(!isCurrCell && puzzle -> board[j][i].val == z) {
				isLegal =  false;
				if(changeErr && puzzle -> board[j][i].isFixed == false) {
					puzzle -> board[j][i].err = 1;
				}
			}
		}
	}
	return isLegal;
}

bool validLocation(sudoku* puzzle, int x, int y, int z, bool changeErr) {
	bool validBlock, validRow, validCol;
	if(z == 0) {
		return true;
	}
	validBlock = validLocationBlock(puzzle, x, y, z, changeErr);
	validRow = validLocationRow(puzzle, x, y, z, changeErr);
	validCol = validLocationCol(puzzle, x, y, z, changeErr);
	return validBlock && validRow && validCol;
}

bool isSolved(sudoku* puzzle) {
	int i, j, N = dim(puzzle);
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			if(puzzle -> board[i][j].val == 0) {
				return false;
			}
		}
	}
	return !isErroneous(puzzle);
}

bool isErroneous(sudoku* puzzle) {
	int i, j, N = dim(puzzle);
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			if(puzzle -> board[i][j].err == 1) {
				return true;
			}
		}
	}
	return false;
}

int numEmptyCells(sudoku* puzzle) {
	int i, j, count = 0;
	int N = dim(puzzle);
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			if(puzzle -> board[i][j].val == 0) {
				count++;
			}
		}
	}
	return count;
}

int numNonEmptyCells(sudoku* puzzle) {
	return (dim(puzzle)*dim(puzzle)) - numEmptyCells(puzzle);
}

data* emptyCellsList(sudoku* puzzle) {
	int size = numEmptyCells(puzzle);
	int i, j, count = 0;
	int N = dim(puzzle);
	data *list = (data*)malloc(size*sizeof(data));
	if(list == NULL) fatalExit();
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			if(puzzle -> board[i][j].val == 0) {
				setData(list + count, i, j);
				count++;
			}
		}
	}
	return list;
}

data* nonEmptyCellsList(sudoku* puzzle) {
	int size = numNonEmptyCells(puzzle);
	int i, j, count = 0;
	int N = dim(puzzle);
	data *list = (data*)malloc(size*sizeof(data));
	if(list == NULL) fatalExit();
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			if(puzzle -> board[i][j].val != 0) {
				setData(list + count, i, j);
				count++;
			}
		}
	}
	return list;
}

void print(sudoku puzzle, operation mode) {
	int i, j, k, n = puzzle.n, m = puzzle.m, len = 4*n*m + m + 1;
	char* seperator = malloc((len + 1)*sizeof(char));
	if(seperator == NULL) fatalExit();
	for(i = 0; i < (4*n*m + m + 1); i++) {
		seperator[i] = '-';
	}
	seperator[len] = '\0';

	printf("%s\n",seperator);
	for(i = 0; i < n; i++) {
		for(j = 0; j < m; j++) {
			for(k = 0; k < n*m; k++) {
				if(k % n == 0) {
					printf("|");
				}
				if(puzzle.board[k][i*m+j].isFixed) {
					printf(" %2d.",puzzle.board[k][i*m+j].val);
				}
				else if((mode == EDIT || (mode == SOLVE && puzzle.mark_errors == 1)) && puzzle.board[k][i*m+j].err == 1) {
					printf(" %2d*",puzzle.board[k][i*m+j].val);
				}

				else if(puzzle.board[k][i*m+j].val != 0) {
					printf(" %2d ",puzzle.board[k][i*m+j].val);
				}
				else {
					printf("    ");
				}
			}
			printf("|\n");
		}
		printf("%s\n",seperator);
	}
	free(seperator);
}


/*change is added to beginning of list*/
void addChange(sudoku* puzzle, int col, int row, int currVal, int prevVal) {
	list* change = (list*)malloc(sizeof(list));
	if(change == NULL) fatalExit();
	creatChange(change, col, row, currVal, prevVal);
	if(puzzle -> moves ->changes != NULL) {
		puzzle -> moves -> changes -> prev = change;
		change -> next = puzzle -> moves -> changes;
	}
	puzzle -> moves -> changes = change;
}

void creatChange(list* change, int col, int row, int currVal, int prevVal) {
	change -> col = col;
	change -> row = row;
	change -> currVal = currVal;
	change -> prevVal = prevVal;
	change -> next = NULL;
	change -> prev = NULL;
}


void addMove(sudoku* puzzle, int col, int row, int currVal, int prevVal) {
	addEmptyMove(puzzle);
	addChange(puzzle, col, row, currVal, prevVal);
}


void addEmptyMove(sudoku* puzzle) {
	node* newMove = (node*)malloc(sizeof(node));
	if(newMove == NULL) fatalExit();
	freeNextMoves(puzzle -> moves -> next);
	newMove -> changes = NULL;
	newMove -> prev = puzzle -> moves;
	newMove -> next = NULL;
	newMove -> first = false;
	puzzle -> moves -> next = newMove;
	puzzle -> moves = puzzle -> moves -> next;
}


void freeChangesList(list* changes) {
	list* curr = changes;
	list* next = NULL;
	if(curr == NULL) return; /*there are no moves*/
	while(curr -> prev != NULL) { /*going to the beginning*/
		curr = curr -> prev;
	}
	while(curr != NULL) {
		next = curr -> next;
		free(curr);
		curr = next;
	}
}

void freeNextMoves(node* moves) {
	node* curr = moves;
	node* next = NULL;
	while(curr != NULL) {
		next = curr -> next;
		freeChangesList(curr -> changes);
		free(curr);
		curr = next;
	}
}


void setDoubleArray(double *array, int size, double val) {
	int i;
	for(i = 0; i < size; i++){
		array[i] = val;
	}
}

void setCharArray(char *array, int size, char type) {
	int i;
	for(i = 0; i < size; i++) {
		array[i] = type;
	}
}

int dim(sudoku *puzzle) {
	return puzzle -> m * puzzle -> n;
}

void blockCorner(int n, int m, int block, data *d){
	d -> col = (block%m)*n;
	d -> row = (block/m)*m;
}

int blockNum(int n, int m, data d) {
	return (d.row/m)*m + (d.col/n);
}

bool variableMatch(variable v1, variable v2, int n, int m) {
	int col1 = v1.d.col, col2 = v2.d.col;
	int row1 = v1.d.row, row2 = v2.d.row;
	return !(v1.val == v2.val && (col1 == col2 || row1 == row2 || blockNum(n, m, v1.d) == blockNum(n, m, v2.d)));
}

double generateRandmNum(double max) {
	double randomNum;
	randomNum = (double)rand()/RAND_MAX * max; /*float in range 0 to max */
	return randomNum;
}

cell** createEmptyBoard(int n, int m) {
	int i;
	cell** board;
	board = (cell**)malloc((n*m)*sizeof(cell*));
	if(board == NULL) fatalExit();
	for(i = 0; i < n*m; i++) {
		board[i] = (cell*)malloc((n*m)*sizeof(cell));
		if(board[i] == NULL) fatalExit();
	}
	return board;
}

cell** copyBoard(cell** board, int n, int m) {
	int i, j;
	cell** other = createEmptyBoard(n, m);
	for(i = 0; i < n*m; i++) {
		for(j = 0; j < n*m; j++) {
			other[i][j].val = board[i][j].val;
			other[i][j].isFixed = board[i][j].isFixed;
			other[i][j].err = board[i][j].err;
		}
	}
	return other;
}

sudoku* copySudoku(sudoku *source) {
	sudoku *new = (sudoku*)malloc(sizeof(sudoku));
	if(new == NULL) fatalExit();
	new -> board = copyBoard(source -> board, source -> n, source -> m);
	new -> m = source -> m;
	new -> n = source -> n;
	return new;
}

void freeSudokuCopy(sudoku *source) {
	freeBoard(source -> board, source -> n, source -> m);
	free(source);
}

void freeBoard(cell** board, int n, int m) {
	int i;
	for(i = 0; i < n*m; i++) {
		free(board[i]);
	}
	free(board);
}

int countLegalValsForCell(sudoku *puzzle, int x, int y) {
	int z;
	int N = dim(puzzle);
	int count = 0;
	for(z = 1; z <= N; z++) {
		if(validLocation(puzzle, x, y, z, false)){
			count += 1;
		}
	}
	return count;
}

int* legalValsForCell(sudoku *puzzle, int x, int y) {
	int len = countLegalValsForCell(puzzle, x, y);
	int *list;
	int z, count = 0;
	if(len == 0) { return NULL; }
	list = (int*)malloc(len*sizeof(int));
	if(list == NULL) fatalExit();
	for(z = 1; z <= dim(puzzle); z++) {
		if(validLocation(puzzle, x, y, z, false)) {
			list[count++] = z;
		}
	}
	return list;

}
int numVarsForCell(variable *vars, int len) {
	data d = vars -> d;
	int i = 0;
	while(i < len && equal(d, vars[i].d)) { i++;}
	return i;
}

void addBoardToMovesList(sudoku *puzzle, sudoku *copy) {
	int i, j, N = dim(puzzle);
	cell **old = puzzle -> board;
	cell ** new = copy -> board;
	addEmptyMove(puzzle);
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			if(old[i][j].val != new[i][j].val) {
				addChange(puzzle, i, j, new[i][j].val, old[i][j].val);
				old[i][j].val = new[i][j].val;
			}
		}
	}
}

void toSumArray(double *options, int size) {
	int i;
	for(i = 1; i < size; i++) {
			options[i] += options[i-1];
	}
}

void fatalExit() {
	printf("a fatal error occurred.\n");
	exit(0);
}

int getOffsetForCellInVarsList(int col, int row, int ***arr, int N) {
	int z;
	for(z = 0; z < N; z++) {
		if(arr[col][row][z] != -1) {
			return arr[col][row][z];
		}
	}
	return -1;
}

void setCell(sudoku* puzzle, int x, int y, int z) {
	int prevVal = puzzle -> board[x][y].val;
	bool prevErr = puzzle -> board[x][y].err == 1;
	if(!validLocation(puzzle, x, y, z, true)) {
		puzzle -> board[x][y].err = 1;
	}
	else {
		puzzle -> board[x][y].err = 0;
	}
	puzzle -> board[x][y].val = z;
	if(prevErr) {
		handlePrevErrCell(puzzle, x, y, prevVal);
	}
}

void initializeValArr(int* arr, int dim) {
	int i;
	for(i = 0; i < dim; i++) {
		arr[i] = 0;
	}
}


void findPossibleValuesCell(cell** board, int n, int m, int* values, int col, int row) {
	int i, j, rowStart = (col/n)*n, colStart = (row/m)*m;
	initializeValArr(values, n*m + 1);
	bool inCol, inRow, inBlock;
	for(i = 0; i < n*m; i++) {
		for(j = 0; j < n*m; j++) {
			inCol = j == col;
			inRow = i == row;
			inBlock = (i >= colStart && i < colStart + m) && (j >= rowStart && j < rowStart + n);
			if(inCol || inRow || inBlock) {
				values[board[j][i].val]++;
			}
		}
	}
}

int isSingleValue(int* arr, int size) {
	int i, count = 0, index = 0;
	for(i = 1; i < size; i++) {
		if(arr[i] == 0) {
			count++;
			index = i;
		}
	}
	if(count == 1) {
		return index;
	}
	return 0;
}


void handlePrevErrCell(sudoku* puzzle, int x, int y, int prevVal) {
	int i, j, rowStart = (x/puzzle -> n)*puzzle -> n, colStart = (y/puzzle -> m)*puzzle -> m, N = dim(puzzle);;
	bool inCol, inRow, inBlock;
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			inCol = j == x;
			inRow = i == y;
			inBlock = (i >= colStart && i < colStart + puzzle -> m) && (j >= rowStart && j < rowStart + puzzle -> n);
			if((inCol || inRow || inBlock) && puzzle -> board[j][i].val == prevVal) {
				if(validLocation(puzzle, j, i, prevVal, true)) {
					puzzle -> board[j][i].err = 0;
				}
			}
		}
	}
}
