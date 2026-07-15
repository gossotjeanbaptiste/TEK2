##
## EPITECH PROJECT, 2026
## Makefile
## File description:
## compiles all TEK2 C/C++ projects
##

MAKEFLAGS += --no-print-directory

S1 = Semester_1

S1_PSC = $(S1)/Piscine
STRING = $(S1_PSC)/Day01/string
ZORGLUB = $(S1_PSC)/Day01/zorglub
DAY04_EX00 = $(S1_PSC)/Day04/ex00
DAY04_EX01 = $(S1_PSC)/Day04/ex01
DAY04_EX02 = $(S1_PSC)/Day04/ex02
RUSH1 = $(S1_PSC)/Rush1
RUSH2 = $(S1_PSC)/Rush2
RUSH3 = $(S1_PSC)/Rush3
RUSH4 = $(S1_PSC)/Rush4

S2 = Semester_2

S2_CCP = $(S2)/Concurrent-Prog
PANORAMIX = $(S2_CCP)/Panoramix
PLAZZA = $(S2_CCP)/Plazza

S2_OOP = $(S2)/Oriented-Object-Programming
ARCADE = $(S2_OOP)/Arcade
NANOTEKSPICE = $(S2_OOP)/NanoTekSpice
RAYTRACER = $(S2_OOP)/Raytracer

ZAPPY = $(S2)/Zappy

all: compile

compile:
		@echo "===Compiling Semester 1 Project==="
		@make -C $(STRING) && echo "Compiled libstring.a (Day01 - string)"
		@make -C $(ZORGLUB) && echo "Compiled zorglub (Day01)"
		@make -C $(DAY04_EX00) && echo "Compiled Day04 - ex00"
		@make -C $(DAY04_EX01) && echo "Compiled Day04 - ex01"
		@make -C $(DAY04_EX02) && echo "Compiled Day04 - ex02"
		@make -C $(RUSH1) && echo "Compiled Rush1"
		@make -C $(RUSH2) && echo "Compiled Rush2"
		@make -C $(RUSH3) && echo "Compiled Rush3"
		@make -C $(RUSH4) && echo "Compiled Rush4"
		@echo "===Compiled all project of S1 with Makefile==="
		@echo "===Compiling Semester 2 Project==="
		@make -C $(PANORAMIX) && echo "Compiled Panoramix"
		@make -C $(PLAZZA) && echo "Compiled Plazza"
		@make -C $(ARCADE) && echo "Compiled Arcade"
		@make -C $(NANOTEKSPICE) && echo "Compiled NanoTekSpice"
		@make -C $(RAYTRACER) && echo "Compiled Raytracer"
		@make -C $(ZAPPY) && echo "Compiled Zappy"
		@echo "===Compiled all project of S2 with Makefile==="
		@echo "You can find all the binary in their respective directory"

clean:
		@echo "===Cleaning Semester 1 Project==="
		@make -C $(STRING) clean && echo "Cleaned Day01 - string"
		@make -C $(ZORGLUB) clean && echo "Cleaned Day01 - zorglub"
		@make -C $(DAY04_EX00) clean && echo "Cleaned Day04 - ex00"
		@make -C $(DAY04_EX01) clean && echo "Cleaned Day04 - ex01"
		@make -C $(DAY04_EX02) clean && echo "Cleaned Day04 - ex02"
		@make -C $(RUSH1) clean && echo "Cleaned Rush1"
		@make -C $(RUSH2) clean && echo "Cleaned Rush2"
		@make -C $(RUSH3) clean && echo "Cleaned Rush3"
		@make -C $(RUSH4) clean && echo "Cleaned Rush4"
		@echo "===Cleaning Semester 2 Project==="
		@make -C $(PANORAMIX) clean && echo "Cleaned Panoramix"
		@make -C $(PLAZZA) clean && echo "Cleaned Plazza"
		@make -C $(ARCADE) clean && echo "Cleaned Arcade"
		@make -C $(NANOTEKSPICE) clean && echo "Cleaned NanoTekSpice"
		@make -C $(RAYTRACER) clean && echo "Cleaned Raytracer"
		@make -C $(ZAPPY) clean && echo "Cleaned Zappy"
		@echo "===All Objects Files Cleaned==="

fclean: clean
		@echo "===Cleaning Semester 1 Project Binaries==="
		@make -C $(STRING) fclean && echo "Cleaned Day01 - string"
		@make -C $(ZORGLUB) fclean && echo "Cleaned Day01 - zorglub"
		@make -C $(DAY04_EX00) fclean && echo "Cleaned Day04 - ex00"
		@make -C $(DAY04_EX01) fclean && echo "Cleaned Day04 - ex01"
		@make -C $(DAY04_EX02) fclean && echo "Cleaned Day04 - ex02"
		@make -C $(RUSH1) fclean && echo "Cleaned Rush1"
		@make -C $(RUSH2) fclean && echo "Cleaned Rush2"
		@make -C $(RUSH3) fclean && echo "Cleaned Rush3"
		@make -C $(RUSH4) fclean && echo "Cleaned Rush4"
		@echo "===Cleaning Semester 2 Project Binaries==="
		@make -C $(PANORAMIX) fclean && echo "Cleaned Panoramix"
		@make -C $(PLAZZA) fclean && echo "Cleaned Plazza"
		@make -C $(ARCADE) fclean && echo "Cleaned Arcade"
		@make -C $(NANOTEKSPICE) fclean && echo "Cleaned NanoTekSpice"
		@make -C $(RAYTRACER) fclean && echo "Cleaned Raytracer"
		@make -C $(ZAPPY) fclean && echo "Cleaned Zappy"
		@echo "===All Binaries + Unwanted Files Cleaned==="

re: fclean all

.PHONY: all compile clean fclean re
