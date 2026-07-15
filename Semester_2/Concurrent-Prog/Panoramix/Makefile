##
## EPITECH PROJECT, 2024
## Makefile
## File description:
## compiles libmy
##

MAKEFLAGS += --no-print-directory
PROJECT_NAME = panoramix
CFLAGS = -Wall -Wextra -Wunused -Wimplicit -O3
LDFLAGS = -lpthread -lrt
SRC = main.c $(shell find src -name "*.c")
SRC_SEGV = main.c $(shell find src -name "*.c")
OBJ = $(SRC:.c=.o)
INCL = -I include/
LIB_COMP = -L ./ $(MY_LIB)
CC = epiclang

all: $(PROJECT_NAME)

start :
		@mv gitignore.txt .gitignore
		@mv src/project.c src/$(PROJECT_NAME).c

%.o: %.c
		@$(CC) $(CXXFLAGS) $(INCL) -c $< -o $@

$(PROJECT_NAME): $(OBJ)
		@$(CC) -o $(PROJECT_NAME) $(OBJ) $(INCL) $(LIB_COMP) $(CFLAGS) $(LDFLAGS)

clean:
		@rm -f $(OBJ)

fclean: clean
		@rm -f a.out
		@rm -f $(PROJECT_NAME)
		@rm -f *~
		@rm -f src/*~
		@rm -f coding-style-reports.log
		@rm -f *.gcno
		@rm -f *.gcda
		@rm -f unit_tests
		@rm -f *.out
		@rm -f *#
		@echo "Everything has been cleaned, do make to have a \
new libmy and compilation"

re: fclean all

cs: fclean
		clear
		@echo "All none compliant file for coding style \
has been cleaned, do make for have a \
new libmy and compilation"
		coding-style . .
		cat coding-style-reports.log
		rm -f coding-style-reports.log

unit_tests: fclean
		make all
		$(CC) -o unit_tests tests/*.c -I include/ \
			-L ./ -lmy -lcriterion --coverage


tests_run: unit_tests
		./unit_tests
		gcovr --exclude tests/
		gcovr --exclude tests/ --branches
		make fclean

segfault:
		$(CC) -o $(PROJECT_NAME) -g $(SRC_SEGV) $(INCL) $(CFLAGS)

printf:
		grep -Rn printf

.PHONY: cs all clean fclean re
