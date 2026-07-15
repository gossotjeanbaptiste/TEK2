##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## zappy_server
##

CC = epiclang
CFLAGS = -Wall -Wextra -Werror -std=c11
SRC = $(shell find . -name "*.c")
OBJ = $(SRC:.c=.o)
TARGET = zappy_server

.PHONY: all clean fclean

all: $(TARGET)

%.o: %.c
		$(CC) $(CFLAGS) $(INCL) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)
	mv $(TARGET) ../../

clean:
	-rm -f *.o

fclean: clean
	-rm -f $(TARGET)
