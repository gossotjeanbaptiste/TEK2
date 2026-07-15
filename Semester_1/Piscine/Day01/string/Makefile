##
## EPITECH PROJECT, 2024
## B-CPE-101-NCY-1-1-myprintf-jean-baptiste.gossot
## File description:
## Makefile of lib/my
##

SRC 	= 	string_init.c 	\
		string_destroy.c 	\
		empty.c 	\
		length.c 	\
		at.c 	\
		print.c 	\
		data.c 	\
		clear.c 	\
		assign.c 	\
		append.c

OBJ	=	$(SRC:.c=.o)

NAME	=	libstring.a

$(NAME):	$(OBJ)
	ar rc $(NAME) $(OBJ)

clean:
		rm -f $(OBJ)

fclean: clean
		rm -f a.out
		rm -f $(NAME)
		rm -f *~
		rm -f coding-style-reports.log
		rm -f *.gcno
		rm -f *.gcda
		rm -f *.out
		rm -f *#
		@echo "Everything has been cleaned, do make to have a \
new libmy and compilation"

re: fclean all

cs: fclean
		clear
		@echo "All none compliant file for coding style \
has been cleaned, do make to have a \
new libmy and compilation"
		coding-style . .
		cat coding-style-reports.log
		rm -f coding-style-reports.log
