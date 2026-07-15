##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## Compile the project
##

# Répertoires des sous-projets
GUI_DIR = src/gui
SERVER_DIR = src/server
AI_DIR = src/ai
TEST_DIR = test/

.PHONY: all zappy_gui zappy_server zappy_ai clean fclean re help

all: zappy_gui zappy_server zappy_ai

zappy_gui:
	@$(MAKE) -C $(GUI_DIR) zappy_gui

zappy_server:
	@$(MAKE) -C $(SERVER_DIR) zappy_server

zappy_ai:
	@$(MAKE) -C $(AI_DIR) zappy_ai

clean:
	-@$(MAKE) -C $(GUI_DIR) clean
	-@$(MAKE) -C $(SERVER_DIR) clean
	-@$(MAKE) -C $(AI_DIR) clean
	-@$(MAKE) -C $(TEST_DIR) clean
	-@rm -f *~ a.out

fclean:
	-@$(MAKE) -C $(GUI_DIR) fclean
	-@$(MAKE) -C $(SERVER_DIR) fclean
	-@$(MAKE) -C $(AI_DIR) fclean
	-@$(MAKE) -C $(TEST_DIR) clean
	-@rm -f zappy_gui zappy_server zappy_ai
	-@rm -f *~ a.out

cs: fclean
		clear
		@echo "All none compliant file for coding style \
has been cleaned, do make for have a \
new libmy and compilation"
		coding-style . .
		cat coding-style-reports.log
		rm -f coding-style-reports.log

re: fclean all

.PHONY: all zappy_gui zappy_server zappy_ai clean fclean re
