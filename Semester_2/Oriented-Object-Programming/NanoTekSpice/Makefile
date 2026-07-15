##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project
##

PROJECT_NAME = nanotekspice
CXXFLAGS = -Wall -Wextra -Werror
SRC = main.cpp $(shell find src -name "*.cpp")
SRC_SEGV = main.cpp $(shell find src -name "*.cpp")
CXX = clang++
LIB_CRITERION = -lcriterion --coverage
OBJ = $(SRC:.cpp=.o)
INCL = -I include/
TEST_FAC_BINARY = test_factory
TEST_AND_BINARY = test_and
TEST_NOT_BINARY = test_not
TEST_OR_BINARY = test_or
TEST_XOR_BINARY = test_xor
TEST_BINARIES = $(TEST_AND_BINARY) $(TEST_NOT_BINARY) $(TEST_OR_BINARY) $(TEST_XOR_BINARY) $(TEST_FAC_BINARY)


all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJ)
		@$(CXX) -o $(PROJECT_NAME) $(OBJ) $(INCL) $(CXXFLAGS)

clean:
		@rm -f $(OBJ)


%.o: %.cpp
		@$(CXX) $(CXXFLAGS) $(INCL) -c $< -o $@

start :
		@mv gitignore.txt .gitignore
		@mv src/project.cpp src/$(PROJECT_NAME).cpp

fclean: clean
		@rm -f a.out
		@rm -f $(PROJECT_NAME) $(TEST_BINARIES)
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

test_factory: $(filter-out main.o, $(OBJ))
		$(CXX) -o $(TEST_FAC_BINARY) tests/factory_test.cpp $(filter-out main.o, $(OBJ)) $(INCL) $(CXXFLAGS)
		./test_factory

test_and: tests/and_logic_test.cpp $(filter-out main.o, $(OBJ))
		$(CXX) -o $(TEST_AND_BINARY) tests/and_logic_test.cpp $(filter-out main.o, $(OBJ)) $(INCL) $(CXXFLAGS)
		./$(TEST_AND_BINARY)

test_not: tests/not_logic_test.cpp $(filter-out main.o, $(OBJ))
		$(CXX) -o $(TEST_NOT_BINARY) tests/not_logic_test.cpp $(filter-out main.o, $(OBJ)) $(INCL) $(CXXFLAGS)
		./$(TEST_NOT_BINARY)

test_or: tests/or_logic_test.cpp $(filter-out main.o, $(OBJ))
		$(CXX) -o $(TEST_OR_BINARY) tests/or_logic_test.cpp $(filter-out main.o, $(OBJ)) $(INCL) $(CXXFLAGS)
		./$(TEST_OR_BINARY)

test_xor: tests/xor_logic_test.cpp $(filter-out main.o, $(OBJ))
		$(CXX) -o $(TEST_XOR_BINARY) tests/xor_logic_test.cpp $(filter-out main.o, $(OBJ)) $(INCL) $(CXXFLAGS)
		./$(TEST_XOR_BINARY)

tests_run: fclean all $(TEST_BINARIES)
		@echo "All tests completed successfully!"

segfault:
		$(CXX) -o $(PROJECT_NAME) -g $(SRC_SEGV) $(INCL) $(CXXFLAGS)

.PHONY: cs all clean fclean re test_factory test_and test_not test_or test_xor tests_run segfault
