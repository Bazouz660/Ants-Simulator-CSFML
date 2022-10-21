##
## EPITECH PROJECT, 2021
## Tower Defence
## File description:
## Makefile
##

SRC	=	src/*.c	\
		src/utils/*.c	\

NAME	=	ants

all:
	@gcc -o $(NAME) -g $(SRC) -I include -I C:/CSFML/include -L C:/CSFML/lib/gcc -lcsfml-graphics -lcsfml-audio -lcsfml-system -lcsfml-window -lm

clean:
	@rm -f *.o
	@rm -f *~
	@rm -f *#

fclean: clean
	@rm -f $(NAME)

re: fclean all

exe: re
	@./$(NAME)
