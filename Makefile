SRC_DIR :=	sources
INC_DIR :=	includes
OBJ_DIR :=	objects

SRC		:=	enki.c callback.c api.c router.c json.c request.c mongo.c mjson.c mongoose.c

SRCS	:=	$(addprefix $(SRC_DIR)/,$(SRC))
OBJS	:=	$(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC		:=	clang

CFLAGS	+=	-Wall -Wextra -Werror
MFLAGS	:=	-DMG_ENABLE_MBEDTLS=1 -I/usr/lib/x86_64-linux-gnu/include -I/usr/include
MFLAGS_	:=	-L/usr/lib/x86_64-linux-gnu/lib -lmbedtls -lmbedcrypto -lmbedx509 -lbson-1.0 -lmongoc-1.0

RM		:=	rm -rf

NAME	:=	enki

all:		obj $(NAME)

$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.c
			@$(CC) $(CFLAGS) $(MFLAGS) -c $< -o $@ -I./$(INC_DIR)

$(NAME):	$(OBJS)
			@$(CC) $(CFLAGS) $(OBJS) $(MFLAGS_) -o $(NAME)
			@echo "Build complete."

clean:
			@$(RM) $(OBJS) $(OBJ_DIR)
			@echo "Cleaned."

fclean: 	clean
			@$(RM) $(NAME)

re: 		fclean all

obj:
			@mkdir -p $(OBJ_DIR)

run:		all
			@echo "Running..."
			@./enki

.phony: 	all clean fclean re obj
