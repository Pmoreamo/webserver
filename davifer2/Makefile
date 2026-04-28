# Config
CC = c++
NAME = webserv
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g

# Sources
SRC = src/main.cpp
OBJ = $(SRC:.cpp=.o)

# Output colors
GREEN = \033[0;32m
RED   = \033[0;31m
RESET = \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)✓ $(NAME) built successfully$(RESET)"

%.o: %.cpp Makefile
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(GREEN)✓ Compiled: $<$(RESET)"

clean:
	@had_objs=0; \
	for o in $(OBJ); do [ -f "$$o" ] && had_objs=1 && break; done; \
	if [ $$had_objs -eq 1 ]; then \
		rm -f $(OBJ); \
		echo "$(RED)✗ Object files removed$(RESET)"; \
	else \
		echo "$(GREEN)Nothing to clean$(RESET)"; \
	fi

fclean: clean
	@if [ -f $(NAME) ]; then \
		rm -f $(NAME); \
		echo "$(RED)✗ $(NAME) removed$(RESET)"; \
	else \
		echo "$(GREEN)Nothing to remove$(RESET)"; \
	fi

re: fclean all

.PHONY: all clean fclean re