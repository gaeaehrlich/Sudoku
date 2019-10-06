#include "GameAux.h"

void freeMovesList(node* moves) {
	while(moves -> first == false) { /*going to the beginning*/
		moves = moves -> prev;
	}
	freeNextMoves(moves);
}


void freeSudoku(sudoku* puzzle, int prevn, int prevm) {
	freeBoard(puzzle -> board, prevn, prevm);
	freeMovesList(puzzle -> moves);
}

void initializeMovesList(sudoku* puzzle) {
	node* newMove = (node*)malloc(sizeof(node));
	if(newMove == NULL) fatalExit();
	newMove -> prev = NULL;
	newMove -> next = NULL;
	newMove -> changes = NULL;
	newMove -> first = true;
	puzzle -> moves = newMove;
}

bool isLegalFixedCell(sudoku* puzzle, int col, int row) {
	int i, j, N = dim(puzzle), n = puzzle -> n, m = puzzle -> m, rowStart = (col/n)*n, colStart = (row/m)*m, val = puzzle -> board[col][row].val;
	bool inCol, inRow, inBlock, isCurr;
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			isCurr = j == col && i == row;
			inCol = j == col;
			inRow = i == row;
			inBlock = (i >= colStart && i < colStart + m) && (j >= rowStart && j < rowStart + n);
			if(!isCurr && (inCol || inRow || inBlock)) {
				if(puzzle -> board[j][i].isFixed && puzzle -> board[j][i].val == val)  {
					return false;
				}
			}
		}
	}
	return true;
}


void fillCell(sudoku* puzzle, int val, int i, int j) {
	puzzle -> board[i][j].val = val;
	puzzle -> board[i][j].isFixed = false;
	puzzle -> board[i][j].err = validLocation(puzzle, i, j, val, true) ? 0 : 1;
}

int findCol(int i, int N) {
	if(i > 0) return i - 1;
	return N - 1;
}

int findRow(int i, int j) {
	if(i > 0) return j;
	return j - 1;
}

bool handleDot(sudoku* puzzle, char prev, int i, int j) {
	int col, row;
	if(isdigit(prev) && prev - '0' > 0) {
		col = findCol(i, dim(puzzle));
		row = findRow(i, j);
		if(!isLegalFixedCell(puzzle, col, row)) {
			printf("Invalid input: file not in format - characters are illegal. There is an error with fixed cell.\n");
			return false;
		}
		puzzle -> board[col][row].isFixed = true;
	}
	else {
		printf("Invalid input: file not in format - characters are illegal.\n");
		return false;
	}
	return true;
}


bool fillCellsFromFile(sudoku* puzzle, FILE *file) {
	char c, prev = '\0';
	int i = 0, j = 0, count = 0, N = dim(puzzle);
	while((c = fgetc(file)) != EOF) {
		if(count == N*N && !isspace(c) && !(c == '.')) { /*allowing spaces in the end*/
			printf("Invalid input: file not in format - number of characters incorrect (too many)\n");
			return false;
		}
		if(!isdigit(c) && !isspace(c) && !(c == '.')) {
			printf("Invalid input: file not in format - characters illegal.\n");
			return false;
		}
		if(isdigit(c)) {
			if(c - '0' > N) {
				printf("Invalid input: file not in format - numbers not in correct range\n");
				return false;
			}
			fillCell(puzzle, c - '0', i, j);
			count++;
			prev = c;
			j = (i + 1 == N ? j + 1 : j);
			i = (i + 1 == N ? 0 : i + 1);
		}
		if(c == '.') {
			if(!handleDot(puzzle, prev, i, j)) return false;
			prev = c;
		}
	}
	if(count < N*N) {
		printf("Invalid input: file not in format - number of characters incorrect (not enough)\n");
	}
	return count == N*N;
}

void createEmptySudoku(sudoku* puzzle, operation mode, int prevn, int prevm) {
	int i, j, N = dim(puzzle);
	puzzle -> mark_errors = 1;
	if(mode != INIT) {
		freeSudoku(puzzle, prevn, prevm);
	}
	puzzle -> board = createEmptyBoard(puzzle -> n, puzzle -> m);
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			puzzle -> board[i][j].val = 0;
			puzzle -> board[i][j].isFixed = false;
			puzzle -> board[i][j].err = 0;
		}
	}
}

bool readBoardDim(sudoku* puzzle, FILE *file) {
	int res1, res2, a, b;
	res1 = fscanf(file,"%d", &a);
	res2 = fscanf(file,"%d", &b);
	if(res1 != 1 || res2 != 1) {
		printf("Invalid input: file not in format - characters are incorrect (board dimensions)\n");
		return false;
	}
	if(a * b > 99) {
		printf("Invalid input: file not in format - board dimension is larger than 99\n");
		return false;
	}
	puzzle -> m = a;
	puzzle -> n = b;
	return true;
}

bool getPuzzleFromFile(sudoku* puzzle, FILE *file, operation mode) {
	int prevn = puzzle -> n, prevm = puzzle -> m;
	if(!readBoardDim(puzzle, file)) {
		return false;
	}
	createEmptySudoku(puzzle, mode, prevn, prevm);
	return fillCellsFromFile(puzzle, file);
}


bool writeToFile(sudoku* puzzle, FILE* file, operation mode) {
	int i, j, n = puzzle -> n, m = puzzle -> m;
	char c;
	if(fprintf(file, "%d %d\n", m, n) != 4) return false;
	for(i = 0; i < n*m; i++) {
		for(j = 0; j < n*m; j++) {
			c = puzzle -> board[j][i].val + '0';
			fputc(c, file);
			if(puzzle -> board[j][i].isFixed || (mode == EDIT && puzzle -> board[j][i].val > 0)) {
				if(fputc('.', file) == EOF) return false;
			}
			if(j != n*m - 1) {
				if(fputc(' ', file) == EOF) return false;
			}
		}
		if(!(i == n*m - 1 && j == n*m - 1)) {
			if(fputc('\n', file) == EOF) return false;
		}
	}
	return true;
}

void unFix(sudoku* puzzle) {
	int i, j , N = dim(puzzle);
	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			puzzle -> board[i][j].isFixed = false;
			validLocation(puzzle, i, j, puzzle -> board[i][j].val, true);
		}
	}
}

bool finishUndo(sudoku* puzzle, list* currChange, operation mode, bool printMsg) {
	puzzle -> moves -> changes = currChange;
	puzzle -> moves = puzzle -> moves -> prev;
	if(printMsg) print(*puzzle, mode);
	return true;
}
