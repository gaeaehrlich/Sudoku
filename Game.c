#include "Game.h"

void play() {
	sudoku puzzle;
	command cmd;
	operation mode = INIT;
	char str[257] = "";
	while(fgets(str, 257, stdin)) {
		if(!readCommand(&cmd, str, mode, puzzle) || !executeCommand(&puzzle, cmd, mode)) {
			printf("Please try again. Enter next command: \n");
			continue;
		}
		if(cmd.op == NONE) {
			continue;
		}
		if(cmd.op == SOLVE || cmd.op == EDIT) {
			mode = cmd.op;
		}
		if(mode == SOLVE && isSolved(&puzzle)) {
			printf("BOARD COMPLETED\n");
			freeSudoku(&puzzle, puzzle.n, puzzle.m);
			mode = INIT;
			printf("Enter command to start the game:\n");
			continue;
		}
		printf("Please enter next command: \n");
	}
	exitFunc(&puzzle, mode);
}


bool executeCommand(sudoku* puzzle, command cmd, operation mode) {
	switch(cmd.op) {
	case SOLVE:
		if(solve(puzzle, cmd.params[0], mode)) {
			print(*puzzle, SOLVE);
			return true;
		}
		return false;
	case EDIT:
		return edit(puzzle, cmd.params[0], mode);
	case MARK_ERRORS:
		markErrors(puzzle, atoi(cmd.params[0]));
		return true;
	case PRINT_BOARD:
		print(*puzzle, mode);
		return true;
	case SET:
		set(puzzle, atoi(cmd.params[0]) - 1, atoi(cmd.params[1]) - 1, atoi(cmd.params[2]), mode, true);
		return true;
	case VALIDATE:
		return validate(puzzle, true) != 0;
	case GUESS:
		if(guess(puzzle, atof(cmd.params[0]))) {
			print(*puzzle, mode);
			return true;
		}
		return false;
	case GENERATE:
		if(generate(puzzle, atoi(cmd.params[0]), atoi(cmd.params[1]))) {
			print(*puzzle, mode);
			return true;
		}
		return false;
	case UNDO:
		undo(puzzle, mode, true);
		return true;
	case REDO:
		redo(puzzle, mode);
		return true;
	case SAVE:
		return save(puzzle, mode, cmd.params[0]);
	case HINT:
		return hint(puzzle, atoi(cmd.params[0]) - 1, atoi(cmd.params[1]) - 1);
	case GUESS_HINT:
		return guess_hint(puzzle, atoi(cmd.params[0]) - 1, atoi(cmd.params[1]) - 1);
	case NUM_SOLUTIONS:
		numSolutions(puzzle);
		return true;
	case AUTOFILL:
		autofill(puzzle, mode, true);
		return true;
	case RESET:
		reset(puzzle, mode);
		return true;
	case EXIT:
		exitFunc(puzzle, mode);
		return true;
	default:
		return true;
	}
}


bool solve(sudoku* puzzle, char* fileName, operation mode) {
	FILE *file = NULL;
	bool isLegal;
	file = fopen(fileName, "r");
	if(file == NULL) {
		printf("Invalid input: file path is incorrect.\n");
		return false;
	}
	isLegal = getPuzzleFromFile(puzzle, file, mode);
	if(fclose(file) == EOF) {
		printf("Error closing file. Board was not loaded.\n");
		return false;
	}
	if(isLegal) {
		initializeMovesList(puzzle);
	}
	return isLegal;
}


bool edit(sudoku* puzzle, char* fileName, operation mode) {
	int prevn = puzzle -> n, prevm = puzzle -> m;
	if(fileName != NULL) {
		if(!solve(puzzle, fileName, mode)) return false;
	}
	else {
		puzzle -> n = 3;
		puzzle -> m = 3;
		createEmptySudoku(puzzle, mode, prevn, prevm);
		initializeMovesList(puzzle);
	}
	unFix(puzzle);
	print(*puzzle, EDIT);
	return true;
}


void markErrors(sudoku* puzzle, int mark) {
	puzzle -> mark_errors = mark;
}


/* if we call this func from undo/redo then we dont add a new move so addToMovesList == false */
void set(sudoku* puzzle, int x, int y, int z, operation mode, bool addToMovesList) {
	int prevVal = puzzle -> board[x][y].val;
	if(mode == SOLVE && puzzle -> board[x][y].isFixed == true) {
		printf("Cell <%d,%d> is fixed and you are in SOLVE mode, therefore the cell value can't be changed.\n", x+1, y+1);
		return;
	}
	setCell(puzzle, x, y ,z);
	if(addToMovesList) {
		addMove(puzzle, x, y, z, prevVal);
		print(*puzzle, mode);
	}
}


/* if we call this func from reset then printMsg = false */
bool undo(sudoku* puzzle, operation mode, bool printMsg) {
	list* currChange = puzzle -> moves -> changes;
	bool term = true;
	if(puzzle -> moves -> first == true) {
		if(printMsg) printf("There are no more moves to be undone.\n");
		return false;
	}
	if(currChange == NULL) return finishUndo(puzzle, currChange, mode, printMsg);
	do {
		set(puzzle, currChange -> col, currChange -> row, currChange -> prevVal, mode, false);
		if(printMsg) printf("Changing cell <%d,%d> from %d to %d.\n", currChange -> col + 1, currChange -> row + 1, currChange -> currVal, currChange -> prevVal);
		if(currChange -> next == NULL) {
			term = false;
		}
		else {
			currChange = currChange -> next;
		}
	} while(term);
	return finishUndo(puzzle, currChange, mode, printMsg);
}


void redo(sudoku* puzzle, operation mode) {
	list* currChange;
	bool term = true;
	if(puzzle -> moves -> next == NULL) {
		printf("There are no more moves to be redone.\n");
		return;
	}
	puzzle -> moves = puzzle -> moves -> next;
	currChange = puzzle -> moves -> changes;
	if(currChange == NULL) {
		print(*puzzle, mode);
		return;
	}
	do {
		set(puzzle, currChange -> col, currChange -> row, currChange -> currVal, mode, false);
		printf("Changing cell <%d,%d> from %d to %d.\n", currChange -> col + 1, currChange -> row + 1, currChange -> prevVal, currChange -> currVal);
		if(currChange -> prev == NULL) {
			term = false;
		}
		else {
			currChange = currChange -> prev;
		}
	} while(term);
	puzzle -> moves -> changes = currChange;
	print(*puzzle, mode);
}


bool save(sudoku* puzzle, operation mode, char* fileName) {
	FILE *file;
	bool writer;
	if(mode == EDIT) {
		if(validate(puzzle,false) != 1) {
			printf("Validation failed and therefore the board can't be saved.\n");
			return false;
		}
	}
	file = fopen(fileName, "w");
	if(file == NULL) {
		printf("Invalid input: file path is incorrect.\n");
		return false;
	}
	writer = writeToFile(puzzle, file, mode);
	if(fclose(file) == EOF || !writer) {
		printf("Error writing to file.\n");
		return false;
	}
	return true;
}


void reset(sudoku* puzzle, operation mode) {
	bool canUndo;
	do {
		canUndo = undo(puzzle, mode, false);
	} while(canUndo);
	print(*puzzle, mode);
}


void exitFunc(sudoku* puzzle, operation mode) {
	if(mode != INIT) freeSudoku(puzzle, puzzle -> n, puzzle -> m);
	printf("Exiting...\n");
	exit(0);
}
