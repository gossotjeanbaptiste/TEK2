##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## zappy_server
##

CC = clang
CFLAGS = -Wall -Wextra -Werror -std=c11
SRC = $(shell find . -name "*.c")
OBJ = $(SRC:.c=.o)
TARGET = ../../zappy_server

.PHONY: all zappy_server clean fclean

all: $(TARGET)

zappy_server: $(TARGET)

%.o: %.c
		$(CC) $(CFLAGS) $(INCL) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

clean:
	-rm -f *.o

fclean: clean
	-rm -f $(TARGET)
