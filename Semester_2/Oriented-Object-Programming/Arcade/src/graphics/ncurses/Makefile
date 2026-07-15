##
## EPITECH PROJECT, 2024
## Arcade
## File description:
## Makefile for ncurses graphics library
##

NAME = arcade_ncurses.so

CC = clang++
CFLAGS = -fPIC -Wall -Wextra -std=c++17
LDFLAGS = -shared
CPPFLAGS = -I../../../include

SRC = NcursesGraphics.cpp
OBJ = $(SRC:.cpp=.o)

LIBS = -lncurses

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) -o $(NAME) $(OBJ) $(LIBS) $(CFLAGS) $(LDFLAGS)
	@mkdir -p ../../../lib/
	@mv $(NAME) ../../../lib/$(NAME)

%.o: %.cpp
	@$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f ../../../lib/$(NAME)

re: fclean all

.PHONY: all clean fclean re
