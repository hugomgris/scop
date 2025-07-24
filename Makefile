# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/24 00:00:00 by student           #+#    #+#              #
#    Updated: 2025/07/24 18:39:46 by hmunoz-g         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# -=-=-=-=-    COLOURS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

DEF_COLOR   = \033[0;39m
YELLOW      = \033[0;93m
CYAN        = \033[0;96m
GREEN       = \033[0;92m
BLUE        = \033[0;94m
RED         = \033[0;91m

# -=-=-=-=-    NAME -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

NAME        := scop

# -=-=-=-=-    FLAG -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

CXX          = c++
CC           = gcc
FLAGS       = -Werror -Wall -Wextra -std=c++17 -pedantic -g -fsanitize=address
CFLAGS      = -Werror -Wall -Wextra -g -fsanitize=address
DEPFLAGS    = -MMD -MP

# -=-=-=-=-    PATH -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

RM          = rm -fr
OBJ_DIR     = .obj
DEP_DIR     = .dep
INCLUDE_DIR = include
LIB_DIR     = lib

# -=-=-=-=-    INCLUDES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

INCLUDES    = -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/glad -I$(INCLUDE_DIR)/KHR

# -=-=-=-=-    LIBRARIES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

# OpenGL libraries for different systems
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LIBS = -lGL -lglfw -ldl -lm -pthread
endif
ifeq ($(UNAME_S),Darwin)
    LIBS = -framework OpenGL -lglfw -ldl -lm -pthread
endif

# -=-=-=-=-    FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

# GLAD source file
GLAD_SRC    := include/glad/src/glad.c

# Main source files (add your actual source files here)
SRC         := src/main.cpp \

# Convert .c files to .o for glad
GLAD_OBJ    = $(addprefix $(OBJ_DIR)/, $(GLAD_SRC:.c=.o))
OBJS        = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o)) $(GLAD_OBJ)
DEPS        = $(addprefix $(DEP_DIR)/, $(SRC:.cpp=.d)) $(addprefix $(DEP_DIR)/, $(GLAD_SRC:.c=.d))

# -=-=-=-=-    TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

all: directories $(NAME)

directories:
	@mkdir -p $(OBJ_DIR)/src/app
	@mkdir -p $(OBJ_DIR)/src/obj
	@mkdir -p $(OBJ_DIR)/src/renderer
	@mkdir -p $(OBJ_DIR)/src/utils
	@mkdir -p $(OBJ_DIR)/include/glad/src
	@mkdir -p $(DEP_DIR)/src/app
	@mkdir -p $(DEP_DIR)/src/obj
	@mkdir -p $(DEP_DIR)/src/renderer
	@mkdir -p $(DEP_DIR)/src/utils
	@mkdir -p $(DEP_DIR)/include/glad/src

-include $(DEPS)

# Compile C++ files
$(OBJ_DIR)/%.o: %.cpp 
	@echo "$(YELLOW)Compiling: $< $(DEF_COLOR)"
	$(CXX) $(FLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@ -MF $(DEP_DIR)/$*.d

# Compile C files (for GLAD)
$(OBJ_DIR)/%.o: %.c 
	@echo "$(YELLOW)Compiling: $< $(DEF_COLOR)"
	$(CC) $(CFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@ -MF $(DEP_DIR)/$*.d

$(NAME): $(OBJS) Makefile
	@echo "$(GREEN)Linking $(NAME)!$(DEF_COLOR)"
	$(CXX) $(FLAGS) $(OBJS) $(LIBS) -o $(NAME)
	@echo "$(GREEN)$(NAME) compiled!$(DEF_COLOR)"
	@echo "$(CYAN)Ready to render some 3D magic!$(DEF_COLOR)"

# Run with a default model
run: $(NAME)
	@echo "$(BLUE)Running $(NAME) with teapot.obj$(DEF_COLOR)"
	./$(NAME) resources/teapot.obj

# Run with 42 model
run42: $(NAME)
	@echo "$(BLUE)Running $(NAME) with 42.obj$(DEF_COLOR)"
	./$(NAME) resources/42.obj

# Generate documentation
doxy:
	doxygen Doxyfile 2>/dev/null || echo "$(YELLOW)Doxyfile not found, skipping documentation$(DEF_COLOR)"
	@echo "$(GREEN)Documentation generated (if Doxyfile exists)$(DEF_COLOR)"

doxyclean:
	rm -rf docs/html/ docs/latex/ docs/xml/ docs/rtf/ docs/man/ docs/docbook/
	@echo "$(RED)Cleaned documentation files$(DEF_COLOR)"

clean:
	@$(RM) $(OBJ_DIR) $(DEP_DIR)
	@echo "$(RED)Cleaned object files and dependencies$(DEF_COLOR)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)Cleaned all binaries$(DEF_COLOR)"

re: fclean all

# Show available .obj files
list-models:
	@echo "$(CYAN)Available 3D models:$(DEF_COLOR)"
	@ls -1 resources/*.obj 2>/dev/null || echo "$(RED)No .obj files found in resources/$(DEF_COLOR)"

# Check dependencies
check-deps:
	@echo "$(CYAN)Checking OpenGL dependencies...$(DEF_COLOR)"
	@pkg-config --exists glfw3 && echo "$(GREEN)✓ GLFW3 found$(DEF_COLOR)" || echo "$(RED)✗ GLFW3 not found$(DEF_COLOR)"
	@pkg-config --exists gl && echo "$(GREEN)✓ OpenGL found$(DEF_COLOR)" || echo "$(RED)✗ OpenGL not found$(DEF_COLOR)"

.PHONY: all clean fclean re directories doxy doxyclean run run42 list-models check-deps