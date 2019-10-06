#ifndef STRUCTURES_H_
#define STRUCTURES_H_
#include "SPBufferset.h"
#include <stdbool.h>

/**
 * Type used to identify the instruction inserted by user
 */
typedef enum {
	INIT, SOLVE, EDIT, MARK_ERRORS, PRINT_BOARD, SET, VALIDATE,
	GUESS, GENERATE, UNDO, REDO, SAVE, HINT, GUESS_HINT, NUM_SOLUTIONS,
	AUTOFILL, RESET, EXIT, NONE, ERR_COMMAND
} operation;

/*
 * Structures : cell
 *              list
 *              node
 *              data
 *              variable
 *              elem
 *              stack
 *              sudoku
 *              command
 * */


typedef struct cell {
	int val;
	int err;
	bool isFixed;
} cell;

/*the nodes of the undo-redo list*/
typedef struct list {
	int col;
	int row;
	int currVal;
	int prevVal;
	struct list* next;
	struct list* prev;
} list;

/*the struct for the undo-redo list*/
typedef struct node {
	struct list* changes;
	struct node* next;
	struct node* prev;
	bool first;
} node;

/* used to pass the location of a cell */
typedef struct data {
	int col;
	int row;
} data;

/* the element of the stack */
typedef struct elem {
	data d;
	struct elem *next;
} elem;

/* used in the non recursive backtracking algorithm*/
typedef struct stack {
	int cnt;
	elem *top;
} stack;

typedef struct sudoku {
	cell** board;
	int n;
	int m;
	int mark_errors;
	node* moves;
} sudoku;


/**
 * Type used to execute the command inserted by user and saving the parameters
 */
typedef struct command {
	operation op;
	char* params[3]; /* changed to char* because can be float or file path */
} command;

/* used to pass the location of a cell and its current value */
typedef struct variable{
	data d;
	int val;
}variable;


#endif /* STRUCTURES_H_ */
