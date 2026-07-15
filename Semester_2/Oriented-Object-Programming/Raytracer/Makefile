##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project
##

PROJECT_NAME = raytracer
CXXFLAGS = -Wall -Wextra -Werror -pthread
SRC = main.cpp $(shell find src -name "*.cpp")
SRC_SEGV = main.cpp $(shell find src -name "*.cpp")
CXX = clang++
LIB_CRITERION = -lcriterion --coverage
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


unit_tests: fclean
		make all
		$(CXX) -o unit_tests tests/*.cpp $(INCL) $(LIB_CRITERION)


tests_run: unit_tests
		./unit_tests
		gcovr --exclude tests/
		gcovr --exclude tests/ --branches
		make fclean


segfault:
		$(CXX) -o $(PROJECT_NAME) -g $(SRC_SEGV) $(INCL) $(CXXFLAGS)

.PHONY: cs all clean fclean re
