##
## EPITECH PROJECT, 2024
## Arcade
## File description:
## Makefile for Minesweeper game library
##

GAME_LIB = arcade_minesweeper.so
CXXFLAGS = -Wall -Wextra -std=c++17 -fPIC
CXX = clang++

SRC = Minesweeper.cpp
OBJ = $(SRC:.cpp=.o)

INCL = -I ../../include/ -I ../../../include/
LIBS = -shared

all: $(GAME_LIB)

$(GAME_LIB): $(OBJ)
	@mkdir -p ../../../lib
	@$(CXX) $(LIBS) -o ../../../lib/$(GAME_LIB) $(OBJ)

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCL) -c $< -o $@

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f ../../../lib/$(GAME_LIB)

re: fclean all

.PHONY: all clean fclean re
