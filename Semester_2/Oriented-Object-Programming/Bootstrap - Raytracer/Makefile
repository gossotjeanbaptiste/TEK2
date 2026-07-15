##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project bootstrap_raytracer
##

PROJECT_NAME = bootstrap_raytracer
CXXFLAGS = -Wall -Wextra -Werror
SRC = main.cpp $(shell find src -type f -name "*.cpp")
SRC_SEGV = main.cpp $(shell find src -type f -name "*.cpp")
CXX = clang++

OBJ = $(SRC:.cpp=.o)
INCL = -I include/

%.o: %.cpp
		@$(CXX) $(CXXFLAGS) $(INCL) -c $< -o $@


all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJ)
		@$(CXX) -o $(PROJECT_NAME) $(OBJ) $(INCL) $(CXXFLAGS) -lm

clean:
		@rm -f $(OBJ)

fclean: clean
		@rm -f a.out
		@rm -f $(PROJECT_NAME)
		@rm -f *~
		@rm -f coding-style-reports.log
		@rm -f *.gcno
		@rm -f *.gcda
		@rm -f *.out
		@rm -f *#
		@echo "Everything has been cleaned, do make to have a new libmy and compilation"

re: fclean all

cs: fclean
		clear
		@echo "All none compliant file for coding style has been cleaned, do make to have a new libmy and compilation"
		coding-style . .
		cat coding-style-reports.log
		rm -f coding-style-reports.log

segfault:
		$(CXX) -o $(PROJECT_NAME) -g $(SRC_SEGV) $(INCL) $(CXXFLAGS)

.PHONY: cs all clean fclean re
