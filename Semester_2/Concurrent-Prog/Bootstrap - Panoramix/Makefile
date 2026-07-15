CC = epiclang
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lpthread

# Cibles
TARGETS = step1 step2

all: $(TARGETS)

step1: step1.c
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

step2: step2.c
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

run1: step1
	@./step1

run2: step2
	@./step2

clean:
	@rm -f $(TARGETS)

re: clean all

.PHONY: all run1 run2 clean
