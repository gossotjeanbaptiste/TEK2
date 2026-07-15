##
## EPITECH PROJECT, 2026
## Arcade
## File description:
## Makefile for Arcade core
##

PROJECT_NAME = arcade
CXXFLAGS = -Wall -Wextra -std=c++17 -fPIC
LDFLAGS = -Wl,--export-dynamic
CXX = clang++

LIBS = -ldl -lncurses
SRC = main.cpp src/LibLoader.cpp src/Core.cpp
OBJ = $(SRC:.cpp=.o)

INCL = -I include/

all: core games graphicals

core: $(PROJECT_NAME)

games: lib/arcade_minesweeper.so lib/arcade_menu.so lib/arcade_snake.so

graphicals: lib/arcade_ncurses.so lib/arcade_raylib.so lib/arcade_sdl2.so

$(PROJECT_NAME): $(OBJ)
	@$(CXX) -o $(PROJECT_NAME) $(OBJ) $(LIBS) $(CXXFLAGS) $(LDFLAGS)

lib/arcade_menu.so:
	@$(MAKE) -C src/Games/menu

lib/arcade_minesweeper.so:
	@$(MAKE) -C src/Games/minesweeper

lib/arcade_snake.so:
	@$(MAKE) -C src/Games/snake

lib/arcade_ncurses.so:
	@$(MAKE) -C src/graphics/ncurses

lib/arcade_raylib.so:
	@$(MAKE) -C src/graphics/raylib

lib/arcade_sdl2.so:
	@$(MAKE) -C src/graphics/SDL

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCL) -c $< -o $@

clean:
	@rm -f $(OBJ)
	@$(MAKE) -C src/Games/minesweeper clean
	@$(MAKE) -C src/Games/menu clean
	@$(MAKE) -C src/Games/snake clean
	@$(MAKE) -C src/graphics/ncurses clean
	@$(MAKE) -C src/graphics/raylib clean
	@$(MAKE) -C src/graphics/SDL clean

fclean: clean
	@rm -f $(PROJECT_NAME)
	@rm -f lib/*.so
	@$(MAKE) -C src/Games/minesweeper fclean
	@$(MAKE) -C src/Games/menu fclean
	@$(MAKE) -C src/Games/snake fclean
	@$(MAKE) -C src/graphics/ncurses fclean
	@$(MAKE) -C src/graphics/raylib fclean
	@$(MAKE) -C src/graphics/SDL fclean

re: fclean all

.PHONY: all core games graphicals clean fclean re
