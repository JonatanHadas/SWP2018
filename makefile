CC = gcc

COMP_FLAGS = -ansi -Wall -Wextra \
-Werror -pedantic-errors

EXEC = sudoku-console

OBJS = main.o

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(COMP_FLAGS) $^ -o $@ 
#compile all prerequisites into target

#.c file required for .o creation
main.o: $*.c 
	$(CC) $(COMP_FLAGS) -c $<

clean:
	rm -f $(EXEC) $(OBJS)
	