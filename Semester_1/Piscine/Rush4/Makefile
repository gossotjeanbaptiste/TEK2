##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project MyGKrellm
##

PROJECT_NAME = MyGKrellm
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror
SRC = main.cpp $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) \
$(wildcard src/*/*/*.cpp) 
SRC_SEGV = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
CXX = clang++

OBJ = $(SRC:.cpp=.o)
INCL = -I include/
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system
NCURSES_FLAGS = -lncurses

%.o: %.cpp
		$(CXX) $(CXXFLAGS) $(INCL) -c $< -o $@


all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJ)
		$(CXX) -o $(PROJECT_NAME) $(OBJ) $(INCL) $(CXXFLAGS) $(SFML_FLAGS) $(NCURSES_FLAGS) -lm

clean:
		rm -f $(OBJ)

fclean: clean
		rm -f a.out
		rm -f $(PROJECT_NAME)
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
		$(CC) -o $(PROJECT_NAME) -g $(SRC_SEGV) $(INCL) $(CFLAGS)

.PHONY: cs all clean fclean re
