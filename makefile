CC = gcc
OBJS = main.o MainAux.o GameAux.o SolverAux.o Solver.o Parser.o Game.o
EXEC = sudoku-console
COMP_FLAG = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56


$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c Game.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
MainAux.o: MainAux.c MainAux.h structures.h SPBufferset.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
GameAux.o: GameAux.c GameAux.h Parser.h MainAux.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
SolverAux.o: SolverAux.c SolverAux.h SolverAux.h MainAux.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
Solver.o: Solver.c Solver.h SolverAux.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c	
Parser.o: Parser.c Parser.h structures.h SPBufferset.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
Game.o: Game.c Game.h Solver.h Parser.h GameAux.h MainAux.h
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
all: $(EXEC)
clean:
	rm -f $(OBJS) $(EXEC)