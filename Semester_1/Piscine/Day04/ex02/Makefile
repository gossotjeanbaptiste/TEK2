##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project z
##

PROJECT_NAME = StupidUnitConverter
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -O3
SRC = $(wildcard *.cpp)
SRC_SEGV = $(wildcard *.cpp)
CXX = clang++

OBJ = $(SRC:.cpp=.o)
INCL = -I include/

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJ)
		$(CXX) -o $(PROJECT_NAME) $(OBJ) $(LIB_C) $(CXXFLAGS)

%.o: %.cpp
		$(CXX) $(CXXFLAGS) $(INCL) -c $< -o $@

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
		$(CXX) -o $(PROJECT_NAME) -g $(SRC_SEGV) $(INCL) $(CXXFLAGS)

.PHONY: cs all clean fclean re
