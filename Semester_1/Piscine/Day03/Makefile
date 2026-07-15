##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## compiles libmy
##

MAKEFLAGS += --no-print-directory
PROJECT_NAME = doop
CFLAGS =
SRC = $(wildcard *.hs)
SRC_SEGV = $(wildcard *.hs) $(wildcard src/*.hs)
OBJ = $(SRC:.hs=.o)

all: $(PROJECT_NAME)

start :
		@mv src/project.c src/$(PROJECT_NAME).c

$(PROJECT_NAME):
		@ghc -o $(PROJECT_NAME) $(SRC) $(CFLAGS)

clean:
		@rm -f $(OBJ)
		@rm -f *.hi
		@rm -f *.o

fclean: @clean
		@rm -f a.out
		@rm -f $(PROJECT_NAME)
		@rm -f *~
		@rm -f src/*~
		@rm -f *.gcno
		@rm -f *.gcda
		@rm -f unit_tests
		@rm -f *.out
		@rm -f *#
		@echo "Everything has been cleaned"


re: fclean all

unit_tests: fclean
		make all
		ghc -o unit_tests tests/*.c\


tests_run: unit_tests
		./unit_tests
		gcovr --exclude tests/
		gcovr --exclude tests/ --branches
		make fclean

segfault:
		ghc -o $(PROJECT_NAME) -g $(SRC_SEGV) $(CFLAGS)

printf:
		grep -Rn printf

.PHONY: all clean fclean re