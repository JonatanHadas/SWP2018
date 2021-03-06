CC = gcc

GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

COMP_FLAGS = -ansi -Wall -Wextra \
-Werror -pedantic-errors $(GUROBI_COMP)

EXEC = sudoku-console


# header files
HEADS = game.h solver.h parser.h game_adv.h game_main.h

# generate object file names for header files  (replace every ".h" with a ".o")
OBJS = $(patsubst %.h,%.o, $(HEADS))
# add main object file (has no header)
OBJS += main.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(COMP_FLAGS) $^ $(GUROBI_LIB) -o $@ 
#compile all prerequisites into target

#.c file and headers required for .o creation
main.o: main.c $(HEADS)
	$(CC) $(COMP_FLAGS) -c $<
game.o: game.c $(HEADS)
	$(CC) $(COMP_FLAGS) -c $<
game_adv.o: game_adv.c $(HEADS)
	$(CC) $(COMP_FLAGS) -c $<
solver.o: solver.c $(HEADS)
	$(CC) $(COMP_FLAGS) -c $<
parser.o: parser.c $(HEADS)
	$(CC) $(COMP_FLAGS) -c $<
game_main.o: game_main.c $(HEADS)
	$(CC) $(COMP_FLAGS) -c $<

clean:
	rm -f $(EXEC) $(OBJS)
	