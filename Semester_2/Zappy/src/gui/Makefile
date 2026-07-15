##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## zappy_gui
##


CXX      = clang++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -pthread -I../../include
LDFLAGS  = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRC = $(shell find . -name "*.cpp")
OBJ    = $(SRC:.cpp=.o)
TARGET = ../../zappy_gui

.PHONY: all zappy_gui clean fclean re

all: zappy_gui

zappy_gui: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	-rm -f $(OBJ)

fclean: clean
	-rm -f $(TARGET)

re: fclean all
