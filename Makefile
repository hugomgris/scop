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
FLAGS       = -std=c++17 -pedantic -g -fsanitize=address
CFLAGS      = -g -fsanitize=address
DEPFLAGS    = -MMD -MP

# -=-=-=-=-    PATH -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

RM          = rm -fr
OBJ_DIR     = .obj
DEP_DIR     = .dep
INCLUDE_DIR = include
LIB_DIR     = lib
GLMDIR      = lib/glm

# -=-=-=-=-    INCLUDES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

INCLUDES    = -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/glad -I$(INCLUDE_DIR)/KHR -I$(GLMDIR)

# -=-=-=-=-    LIBRARIES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LIBS = -lGL -lglfw -ldl -lm -pthread
endif
ifeq ($(UNAME_S),Darwin)
    LIBS = -framework OpenGL -lglfw -ldl -lm -pthread
endif

# -=-=-=-=-    FILES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

GLAD_SRC    := include/glad/src/glad.c

SRC         := src/main.cpp \
			   src/parser/Parser.cpp \
			   src/app/App.cpp \
			   src/app/InputManager.cpp \
			   src/renderer/Renderer.cpp \
			   src/renderer/Shader.cpp \
			   src/renderer/Mesh.cpp \
			   src/renderer/Texture.cpp \
			   src/renderer/TextureLoader.cpp \
			   src/utils/ErrorManager.cpp \

# Convert .c files to .o for glad
GLAD_OBJ    = $(addprefix $(OBJ_DIR)/, $(GLAD_SRC:.c=.o))
OBJS        = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o)) $(GLAD_OBJ)
DEPS        = $(addprefix $(DEP_DIR)/, $(SRC:.cpp=.d)) $(addprefix $(DEP_DIR)/, $(GLAD_SRC:.c=.d))

# -=-=-=-=-    TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

all: directories glm $(NAME)

directories:
	@mkdir -p $(OBJ_DIR)/src/app
	@mkdir -p $(OBJ_DIR)/src/parser
	@mkdir -p $(OBJ_DIR)/src/obj
	@mkdir -p $(OBJ_DIR)/src/renderer
	@mkdir -p $(OBJ_DIR)/src/utils
	@mkdir -p $(OBJ_DIR)/include/glad/src
	@mkdir -p $(DEP_DIR)/src/app
	@mkdir -p $(DEP_DIR)/src/parser
	@mkdir -p $(DEP_DIR)/src/obj
	@mkdir -p $(DEP_DIR)/src/renderer
	@mkdir -p $(DEP_DIR)/src/utils
	@mkdir -p $(DEP_DIR)/include/glad/src
	@mkdir -p $(LIB_DIR)

glm:
	@if [ ! -d "$(GLMDIR)" ]; then \
		echo "$(YELLOW)GLM library not found, cloning from GitHub...$(DEF_COLOR)"; \
		git clone https://github.com/g-truc/glm.git $(GLMDIR); \
		echo "$(GREEN)GLM library cloned successfully!$(DEF_COLOR)"; \
	fi

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
	@echo "$(CYAN)I have become scop, the renderer of worlds!$(DEF_COLOR)"

# -=-=-=-=-    DOCUMENTATION -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

DOXYGEN_VERSION = 1.10.0
DOXYGEN_DIR = $(HOME)/doxygen-$(DOXYGEN_VERSION)
DOXYGEN_BIN = $(DOXYGEN_DIR)/bin/doxygen

# Generate project documentation using Doxygen
# NOTE: This target will work with or without Graphviz installed:
#   - WITH Graphviz: Generates full docs including dependency graphs, call graphs,
#     class collaboration diagrams, and inheritance charts
#   - WITHOUT Graphviz: Generates complete documentation but without visual graphs
#
# To enable visual graphs, install Graphviz on your system:
#   Ubuntu/Debian: sudo apt-get install graphviz
#   macOS: brew install graphviz
#   Arch: sudo pacman -S graphviz
doxy: $(DOXYGEN_BIN)
	@if command -v dot >/dev/null 2>&1; then \
		echo "$(GREEN)Generating documentation with Graphviz support$(DEF_COLOR)"; \
	else \
		echo "$(YELLOW)Generating documentation without graphs (Graphviz not found)$(DEF_COLOR)"; \
		echo "$(CYAN)Tip: Install graphviz package to enable dependency graphs$(DEF_COLOR)"; \
	fi
	@$(DOXYGEN_BIN) Doxyfile
	@echo "$(GREEN)Documentation generated in docs/html/index.html$(DEF_COLOR)"

$(DOXYGEN_BIN):
	@echo "$(CYAN)Downloading Doxygen$(DEF_COLOR)"
	@wget -q https://www.doxygen.nl/files/doxygen-$(DOXYGEN_VERSION).linux.bin.tar.gz -O /tmp/doxygen.tar.gz
	@tar -xzf /tmp/doxygen.tar.gz -C $(HOME)
	@rm /tmp/doxygen.tar.gz
	@echo "$(GREEN)Doxygen installed successfully$(DEF_COLOR)"

doxyclean:
	@rm -rf docs/html/ docs/latex/ docs/xml/ docs/rtf/ docs/man/ docs/docbook/
	@echo "$(RED)Cleaned documentation files$(DEF_COLOR)"

# -=-=-=-=-    CLEANUP -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

clean:
	@$(RM) $(OBJ_DIR) $(DEP_DIR)
	@echo "$(RED)Cleaned object files and dependencies$(DEF_COLOR)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)Cleaned all binaries$(DEF_COLOR)"
	@$(RM) $(GLMDIR)
	@echo "$(RED)Cleaned GLM library$(DEF_COLOR)"

re: fclean all

list-models:
	@echo "$(CYAN)Available 3D models:$(DEF_COLOR)"
	@ls -1 resources/objects/*.obj 2>/dev/null || echo "$(RED)No .obj files found in resources/$(DEF_COLOR)"

check-deps:
	@echo "$(CYAN)Checking OpenGL dependencies...$(DEF_COLOR)"
	@pkg-config --exists glfw3 && echo "$(GREEN)✓ GLFW3 found$(DEF_COLOR)" || echo "$(RED)✗ GLFW3 not found$(DEF_COLOR)"
	@pkg-config --exists gl && echo "$(GREEN)✓ OpenGL found$(DEF_COLOR)" || echo "$(RED)✗ OpenGL not found$(DEF_COLOR)"
	@if [ -d "$(GLMDIR)" ]; then echo "$(GREEN)✓ GLM found$(DEF_COLOR)"; else echo "$(RED)✗ GLM not found$(DEF_COLOR)"; fi

.PHONY: all clean fclean re directories glm doxy doxyclean run run42 list-models check-deps