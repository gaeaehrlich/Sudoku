
#include "Parser.h"

int numParams(operation op) {
	switch(op) {
	case SOLVE:
		return 1;
	case EDIT:
		return 1;
	case MARK_ERRORS:
		return 1;
	case SET:
		return 3;
	case GUESS:
		return 1;
	case GENERATE:
		return 2;
	case SAVE:
		return 1;
	case HINT:
		return 2;
	case GUESS_HINT:
		return 2;
	default:
		return 0;
	}
}

bool isLegalCommand(operation mode, operation currCmd) {
	if(currCmd == ERR_COMMAND) {
		printf("Invalid command name.\n");
		return false;
	}
	if(currCmd == GENERATE) {
		if(mode ==INIT) {
			printf("Invalid command: The current command isn't available in INIT mode. It is only available from EDIT mode.\n");
			return false;
		}
		if(mode == SOLVE) {
			printf("Invalid command: The current command isn't available in SOLVE mode. It is only available from EDIT mode.\n");
			return false;
		}
	}
	if(currCmd == MARK_ERRORS || currCmd == GUESS || currCmd == HINT || currCmd == GUESS_HINT || currCmd == AUTOFILL) {
		if(mode == INIT) {
			printf("Invalid command: The current command isn't available in INIT mode. It is only available from SOLVE mode.\n");
			return false;
		}
		if(mode == EDIT) {
			printf("Invalid command: The current command isn't available in EDIT mode. It is only available from SOLVE mode.\n");
			return false;
		}
	}
	if(currCmd == PRINT_BOARD || currCmd == SET || currCmd == VALIDATE || currCmd == UNDO || currCmd == REDO || currCmd == SAVE || currCmd == NUM_SOLUTIONS || currCmd == RESET) {
		if(mode == INIT) {
			printf("Invalid command: The current command isn't available in INIT mode. It is only available from SOLVE or EDIT mode.\n");
			return false;
		}
	}
	return true;
}

bool isInteger(char* x) {
	unsigned int i;
	for(i = 0; i < strlen(x); i++) {
		if(!isdigit(x[i])) {
			return false;
		}
	}
	return true;
}

bool isFloat(char* x) {
	unsigned int i;
	int symbol = 0;
	for(i = 0; i < strlen(x); i++) {
		if(x[i] == '.') {
			symbol++;
		}
		if((x[i] != '.' && !isdigit(x[i])) || symbol > 1) {
			return false;
		}
	}
	return true;
}

bool isInRange(int x, int a, int b) {
	return (x >= a && x <= b);
}


bool isLegalParam(operation op, char* param, int numParam, sudoku puzzle) {
	int puzzleDim = puzzle.n * puzzle.m;
	char* numbers[3] = {"first", "second", "third"};
	if(op == MARK_ERRORS && (!isInteger(param) || !isInRange(atoi(param), 0, 1))) {
		printf("Invalid input: parameter for command MARK ERRORS not in format - \nparameter should be 0 or 1.\n");
		return false;
	}
	if(op == SET && (!isInteger(param) || ((numParam == 0 || numParam == 1) && !isInRange(atoi(param), 1, puzzleDim)) || (numParam == 2 && !isInRange(atoi(param), 0, puzzleDim)))) {
		printf("Invalid input: the %s parameter for command SET not in format -\nparameters should be 3 integers, the first two in range 1 to %d, and the last in range 0 to %d.\n", numbers[numParam], puzzleDim, puzzleDim);
		return false;
	}
	if(op == GUESS && !isFloat(param)) {
		printf("Invalid input: parameter for command GUESS not in format - \nparameter should be a float.\n");
		return false;
	}
	if(op == GENERATE && (!isInteger(param) || !isInRange(atoi(param), 0, puzzleDim*puzzleDim))) {
		printf("Invalid input: the %s parameter for command GENERATE not in format - \nshould be an integer in range 0 to %d.\n",  numbers[numParam], puzzleDim*puzzleDim);
		return false;
	}
	if(op == HINT && (!isInteger(param) || !isInRange(atoi(param), 1, puzzleDim))) {
		printf("Invalid input: the %s parameter for command HINT not in format - \nshould be an integer in range 1 to %d.\n",  numbers[numParam], puzzleDim);
		return false;
	}
	if(op == GUESS_HINT && (!isInteger(param) || !isInRange(atoi(param), 1, puzzleDim))) {
		printf("Invalid input: the %s parameter for command GUESS HINT not in format - \nshould be an integer in range 1 to %d.\n",  numbers[numParam], puzzleDim);
		return false;
	}
	return true;
}

bool findParams(command* cmd, const char* delim, sudoku puzzle) {
	int i, j;
	char* token;
	i = numParams(cmd -> op);
	for(j = 0; j < i; j++) {
		token = strtok(NULL, delim);
		if(token == NULL && cmd -> op != EDIT) { /* not enough tokens*/
			printf("Invalid number of parameters: not enough parameters.\n");
			return false;
		}
		else if(!isLegalParam(cmd -> op, token, j, puzzle)) {
				return false;
		}
		else {
			cmd -> params[j] = token;
		}
	}
	token = strtok(NULL, delim);
	if(token != NULL) {
		printf("Invalid number of parameters: too many parameters.\n");
		return false;
	}
	return true;
}

bool readCommand(command* cmd, char str[257], operation mode, sudoku puzzle) {
	char* token;
	const char* delim = " \t\r\n";
	int c;
	if(str[256] != '\0') {
		printf("Invalid command: Command too long.\n");
		while((c = getchar()) != '\n' && c != EOF) { } /*clearing stdin*/
		str[256] = '\0';
		return false;
	}
	token = strtok(str, delim);
	if(token == NULL) {
		cmd -> op = NONE;
	}
	else if(strcmp(token, "solve") == 0) {
		cmd -> op = SOLVE;
	}
	else if(strcmp(token, "edit") == 0) {
		cmd -> op = EDIT;
	}
	else if(strcmp(token, "mark_errors") == 0) {
		cmd -> op = MARK_ERRORS	;
	}
	else if(strcmp(token, "print_board") == 0) {
		cmd -> op = PRINT_BOARD;
	}
	else if(strcmp(token, "set") == 0) {
		cmd -> op = SET;
	}
	else if(strcmp(token, "validate") == 0) {
		cmd -> op = VALIDATE;
	}
	else if(strcmp(token, "guess") == 0) {
		cmd -> op = GUESS;
	}
	else if(strcmp(token, "generate") == 0) {
		cmd -> op = GENERATE;
	}
	else if(strcmp(token, "undo") == 0) {
		cmd -> op = UNDO;
	}
	else if(strcmp(token, "redo") == 0) {
		cmd -> op = REDO;
	}
	else if(strcmp(token, "save") == 0) {
		cmd -> op = SAVE;
	}
	else if(strcmp(token, "hint") == 0) {
		cmd -> op = HINT;
	}
	else if(strcmp(token, "guess_hint") == 0) {
		cmd -> op = GUESS_HINT;
	}
	else if(strcmp(token, "num_solutions") == 0) {
		cmd -> op = NUM_SOLUTIONS;
	}
	else if(strcmp(token, "autofill") == 0) {
		cmd -> op = AUTOFILL;
	}
	else if(strcmp(token, "reset") == 0) {
		cmd -> op = RESET;
	}
	else if(strcmp(token, "exit") == 0) {
		cmd -> op = EXIT;
	}
	else {
		cmd -> op = ERR_COMMAND;
	}
	return isLegalCommand(mode, cmd -> op) && findParams(cmd, delim, puzzle);
}
