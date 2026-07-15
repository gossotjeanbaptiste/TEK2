##
## EPITECH PROJECT, 2024
## Arcade
## File description:
## Makefile for Raylib graphics library
##

GRAPHICS_LIB = arcade_raylib.so
CXXFLAGS = -Wall -Wextra -std=c++17 -fPIC
CXX = clang++

SRC = Raylib_graphics.cpp
OBJ = $(SRC:.cpp=.o)

INCL = -I../../../include/
LIBS = -shared -lraylib -lm

all: $(GRAPHICS_LIB)

$(GRAPHICS_LIB): $(OBJ)
	@mkdir -p ../../../lib
	@$(CXX) $(LIBS) -o ../../../lib/$(GRAPHICS_LIB) $(OBJ)

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCL) -c $< -o $@

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f ../../../lib/$(GRAPHICS_LIB)

re: fclean all

.PHONY: all clean fclean re
