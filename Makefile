NAME		=	server

CC		=	gcc

CFLAGS		+=	-I $(INC) -Wall -Wextra

LDFLAGS		+=

INC		=	include/

SRC		=	src/main.c \
			src/utils.c

OBJ		=	$(SRC:.c=.o)

RM		=	rm -f

all:		$(NAME)

$(NAME):	$(OBJ)
		$(CC) $(OBJ) $(LDFLAGS) -o $(NAME) $(CFLAGS)

clean:		
		$(RM) $(OBJ)

fclean: 	clean
		$(RM) $(NAME)

re: 		fclean all
