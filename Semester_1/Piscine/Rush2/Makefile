##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project pushswap_checker
##

PROJECT_NAME = pushswap_checker
HFLAGS = -Wall -O2
SRC = $(wildcard *.hs)
OBJ = $(SRC:.hs=.o)
HI = $(SRC:.hs=.hi)
CC = ghc

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(SRC)
		$(CC) $(HFLAGS) -o $(PROJECT_NAME) $(SRC)

clean:
		rm -f $(OBJ) $(HI)

fclean: clean
		rm -f $(PROJECT_NAME)
		rm -f *~
		rm -f *#
		@echo "Everything has been cleaned, do make to have a \
new compilation"

re: fclean all

cs: fclean
		clear
		@echo "All non-compliant files for coding style \
has been cleaned, do make to have a \
new compilation"
		./lambdananas .

.PHONY: cs all clean fclean re
