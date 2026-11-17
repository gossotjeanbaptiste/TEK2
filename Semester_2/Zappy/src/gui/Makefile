##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## zappy_gui
##

CXX      = clang++
CXXFLAGS = -O3 -Wall -Wextra -Werror -std=c++17 -pthread -I../../include -MMD -MP
LDFLAGS  = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
SRC = $(shell find . -name "*.cpp")
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)
TARGET = ../../zappy_gui

.PHONY: all zappy_gui clean fclean re

all: zappy_gui

zappy_gui: $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

-include $(DEP)

clean:
	-rm -f $(OBJ) $(DEP)

fclean: clean
	-rm -f $(TARGET)

re: fclean all
