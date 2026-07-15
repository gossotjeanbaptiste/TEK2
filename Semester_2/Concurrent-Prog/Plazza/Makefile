##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project
##

PROJECT_NAME = plazza
CXXFLAGS = -Wall -Wextra -Werror
SRC = main.cpp $(shell find src -name "*.cpp")
SRC_SEGV = main.cpp $(shell find src -name "*.cpp")
CXX = clang++
OBJ = $(SRC:.cpp=.o)
INCL = -I include/

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJ)
		@$(CXX) -o $(PROJECT_NAME) $(OBJ) $(INCL) $(CXXFLAGS)

%.o: %.cpp
		@$(CXX) $(CXXFLAGS) $(INCL) -c $< -o $@

start :
		@mv gitignore.txt .gitignore
		@mv src/project.cpp src/$(PROJECT_NAME).cpp

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
		@echo "Everything has been cleaned"

re: fclean all

segfault:
		@$(CXX) -o $(PROJECT_NAME) -g $(SRC_SEGV) $(INCL) $(CXXFLAGS)

.PHONY: all clean fclean re
