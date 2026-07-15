##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## zappy_ai
##

TARGET = ../../zappy_ai

.PHONY: all zappy_ai clean fclean

all: zappy_ai

zappy_ai:
	@echo "#!/bin/bash" > $(TARGET)
	@echo 'exec python3 $$(dirname $$0)/src/ai/main.py "$$@"' >> $(TARGET)
	@chmod 755 $(TARGET)
	@echo "[OK] zappy_ai compiled (wrapper created)"

clean:
	@rm -rf __pycache__

fclean: clean
	@rm -f $(TARGET)
