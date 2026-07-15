##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project z
##

CFLAGS = -Wall -Wextra -Wunused -Wimplicit -O3 -std=gnu17
SRC = $(wildcard *.c) $(wildcard G-PDG-300_rush1/*.c)
SRC_SEGV = $(wildcard *.c)
CC = epiclang
DIRECTORY = G-PDG-300_rush1

SRC_EX1 = new.c 	\
		$(DIRECTORY)/ex01.c 	\
		$(DIRECTORY)/player.c

SRC_EX2 = vertex.c 	\
		point.c 	\
		new.c 	\
		$(DIRECTORY)/ex02.c
SRC_EX3 = vertex.c 	\
		point.c 	\
		new.c 	\
		$(DIRECTORY)/ex03.c
SRC_EX4 = new.c 	\
		char.c 	\
		int.c 	\
		float.c 	\
		$(DIRECTORY)/ex04.c
SRC_EX5 = new.c 	\
		char.c 	\
		int.c 	\
		float.c 	\
		array.c 	\
		$(DIRECTORY)/ex05.c
SRC_EX6 = list.c 	\
		new.c	\
		char.c 	\
		int.c 	\
		float.c 	\
		array.c 	\
		ex06.c


OBJ_EX1 = $(SRC_EX1:.c=.o)
OBJ_EX2 = $(SRC_EX2:.c=.o)
OBJ_EX3 = $(SRC_EX3:.c=.o)
OBJ_EX4 = $(SRC_EX4:.c=.o)
OBJ_EX5 = $(SRC_EX5:.c=.o)
OBJ_EX6 = $(SRC_EX6:.c=.o)

OBJ = $(SRC:.c=.o)
INCL = -I include/
all: 
		make ex01
		make ex02
		make ex03
		make ex04
		make ex05
		make ex06

ex01: $(OBJ_EX1)
		$(CC) -o ex01 $(OBJ_EX1) $(CFLAGS)

ex02: $(OBJ_EX2)
		$(CC) -o ex02 $(OBJ_EX2) $(CFLAGS)

ex03: $(OBJ_EX3)
		$(CC) -o ex03 $(OBJ_EX3) $(CFLAGS)

ex04: $(OBJ_EX4)
		$(CC) -o ex04 $(OBJ_EX4) $(CFLAGS)

ex05: $(OBJ_EX5)
		$(CC) -o ex05 $(OBJ_EX5) $(CFLAGS)

ex06: $(OBJ_EX6)
		$(CC) -o ex06 $(OBJ_EX6) $(CFLAGS)

clean:
		rm -f $(OBJ)

fclean: clean
		rm -f a.out
		rm -f $(PROJECT_NAME) ex01 ex02 ex03 ex04 ex05 ex06
		rm -f *~
		rm -f coding-style-reports.log
		rm -f *.gcno
		rm -f *.gcda
		rm -f *.out
		rm -f *#
		@echo "Everything has been cleaned, do make to have a \
new libmy and compilation"

re: fclean all

cs: fclean
		clear
		@echo "All none compliant file for coding style \
has been cleaned, do make to have a \
new libmy and compilation"
		coding-style . .
		cat coding-style-reports.log
		rm -f coding-style-reports.log

segfault:
		epiclang -o $(PROJECT_NAME) -g $(SRC_SEGV) $(INCL) $(CFLAGS)

.PHONY: cs all clean fclean re
