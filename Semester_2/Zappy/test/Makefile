##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project
##

CC = clang
CFLAGS = -Wall -Wextra -Werror -std=c11 -I../include

SRC = ../src/server/server_world.c \
      ../src/server/server_resource.c \
      ../src/server/server_buffer.c \
      ../src/server/server_command_queue.c \
      test_server.c

TARGET = test_server

TEST2_SRC = ../src/server/server_command_move.c \
            ../src/server/server_command_info.c \
            ../src/server/server_command_resource.c \
            ../src/server/server_resource.c \
            ../src/server/server_buffer.c \
            ../src/server/server_world.c \
            ../src/server/server_network.c \
            ../src/server/server_game_over.c \
            ../src/server/server_command_incantation.c \
            ../src/server/server_command_fork.c \
            ../src/server/server_players.c \
            ../src/server/server_notify.c \
            test_command.c \
            test_command_lifecycle.c

TEST2_TARGET = test_command

TEST3_SRC = ../src/server/server_world.c \
            ../src/server/server_resource.c \
            ../src/server/server_buffer.c \
            ../src/server/server_player_health.c \
            ../src/server/server_network.c \
            test_advanced.c

TEST3_TARGET = test_advanced

TEST4_SRC = ../src/server/server_world.c \
            ../src/server/server_resource.c \
            ../src/server/server_buffer.c \
            ../src/server/server_game.c \
            ../src/server/server_players.c \
            ../src/server/server_network.c \
            ../src/server/server_notify.c \
            ../src/server/server_player_health.c \
            test_game.c

TEST4_TARGET = test_game

.PHONY: all clean

all: $(TARGET) $(TEST2_TARGET) $(TEST3_TARGET) $(TEST4_TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

$(TEST2_TARGET): $(TEST2_SRC)
	$(CC) $(CFLAGS) $(TEST2_SRC) -o $(TEST2_TARGET)

$(TEST3_TARGET): $(TEST3_SRC)
	$(CC) $(CFLAGS) $(TEST3_SRC) -o $(TEST3_TARGET)

$(TEST4_TARGET): $(TEST4_SRC)
	$(CC) $(CFLAGS) $(TEST4_SRC) -o $(TEST4_TARGET)

clean:
	rm -f $(TARGET) $(TEST2_TARGET) $(TEST3_TARGET) $(TEST4_TARGET)
