##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project z
##

PROJECT_NAME = bistroMatic
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -O3
SRC = $(wildcard *.cpp) $(wildcard src/*.cpp)
SRC_SEGV = $(wildcard *.cpp)
CXX = clang++

OBJ = $(SRC:.cpp=.o)
INCL = -I include/

# Test configuration
TEST_PARSER_BIN = test_parser
TEST_PARSER_SRC = test/test_parser.cpp src/Lexer.cpp src/Exception.cpp src/OperandFactory.cpp src/Parser.cpp src/Operand.cpp src/VirtualMachine.cpp src/Stack.cpp src/RegisterBank.cpp src/InstructionExecutor.cpp src/Instruction.cpp
TEST_PARSER_OBJ = $(TEST_PARSER_SRC:.cpp=.o)

TEST_VM_BIN = test_vm
TEST_VM_SRC = test/test_vm.cpp src/Stack.cpp src/RegisterBank.cpp src/InstructionExecutor.cpp src/VirtualMachine.cpp src/Instruction.cpp src/Operand.cpp src/OperandFactory.cpp
TEST_VM_OBJ = $(TEST_VM_SRC:.cpp=.o)

TEST_OPERAND_BIN = test_operand
TEST_OPERAND_SRC = test/test_operand.cpp src/Operand.cpp src/OperandFactory.cpp
TEST_OPERAND_OBJ = $(TEST_OPERAND_SRC:.cpp=.o)

all: $(PROJECT_NAME)

test_parser: $(TEST_PARSER_BIN)

$(TEST_PARSER_BIN): $(TEST_PARSER_OBJ)
		$(CXX) -o $(TEST_PARSER_BIN) $(TEST_PARSER_OBJ) $(CXXFLAGS)

test_vm: $(TEST_VM_BIN)

$(TEST_VM_BIN): $(TEST_VM_OBJ)
		$(CXX) -o $(TEST_VM_BIN) $(TEST_VM_OBJ) $(CXXFLAGS)

test_operand: $(TEST_OPERAND_BIN)

$(TEST_OPERAND_BIN): $(TEST_OPERAND_OBJ)
		$(CXX) -o $(TEST_OPERAND_BIN) $(TEST_OPERAND_OBJ) $(CXXFLAGS)

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJ)
		$(CXX) -o $(PROJECT_NAME) $(OBJ) $(LIB_C) $(CXXFLAGS)

%.o: %.cpp
		$(CXX) $(CXXFLAGS) $(INCL) -c $< -o $@

clean:
		rm -f $(OBJ)
		rm -f $(TEST_PARSER_OBJ)
		rm -f $(TEST_VM_OBJ)
		rm -f $(TEST_OPERAND_OBJ)
		rm -f test/*.o

fclean: clean
		rm -f a.out
		rm -f $(PROJECT_NAME)
		rm -f $(TEST_PARSER_BIN)
		rm -f $(TEST_VM_BIN)
		rm -f $(TEST_OPERAND_BIN)
		rm -f *~
		rm -f coding-style-reports.log
		rm -f *.gcno
		rm -f *.gcda
		rm -f *.out
		rm -f *#
		@echo "Everything has been cleaned, do make to have a \
new libmy and compilation"

re: fclean all

test: $(TEST_OPERAND_BIN) $(TEST_PARSER_BIN) $(TEST_VM_BIN)
		./$(TEST_OPERAND_BIN)
		./$(TEST_PARSER_BIN)
		./$(TEST_VM_BIN)

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
