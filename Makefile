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
IMGUIDIR    = lib/imgui

# -=-=-=-=-    INCLUDES -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

INCLUDES    = -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/glad -I$(INCLUDE_DIR)/KHR -I$(GLMDIR) -I$(LIB_DIR)/imgui -I$(LIB_DIR)/imgui/backends

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

IMGUI_SRC   := lib/imgui/imgui.cpp \
			   lib/imgui/imgui_demo.cpp \
			   lib/imgui/imgui_draw.cpp \
			   lib/imgui/imgui_tables.cpp \
			   lib/imgui/imgui_widgets.cpp \
			   lib/imgui/backends/imgui_impl_glfw.cpp \
			   lib/imgui/backends/imgui_impl_opengl3.cpp

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
			   src/ui/UIManager.cpp \

# Convert .c files to .o for glad
GLAD_OBJ    = $(addprefix $(OBJ_DIR)/, $(GLAD_SRC:.c=.o))
IMGUI_OBJ   = $(addprefix $(OBJ_DIR)/, $(IMGUI_SRC:.cpp=.o))
OBJS        = $(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o)) $(GLAD_OBJ) $(IMGUI_OBJ)
DEPS        = $(addprefix $(DEP_DIR)/, $(SRC:.cpp=.d)) $(addprefix $(DEP_DIR)/, $(GLAD_SRC:.c=.d)) $(addprefix $(DEP_DIR)/, $(IMGUI_SRC:.cpp=.d))

# -=-=-=-=-    TARGETS -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- #

all: directories glm imgui $(NAME)

directories:
	@mkdir -p $(OBJ_DIR)/src/app
	@mkdir -p $(OBJ_DIR)/src/parser
	@mkdir -p $(OBJ_DIR)/src/obj
	@mkdir -p $(OBJ_DIR)/src/renderer
	@mkdir -p $(OBJ_DIR)/src/utils
	@mkdir -p $(OBJ_DIR)/src/ui
	@mkdir -p $(OBJ_DIR)/include/glad/src
	@mkdir -p $(OBJ_DIR)/lib/imgui
	@mkdir -p $(OBJ_DIR)/lib/imgui/backends
	@mkdir -p $(DEP_DIR)/src/app
	@mkdir -p $(DEP_DIR)/src/parser
	@mkdir -p $(DEP_DIR)/src/obj
	@mkdir -p $(DEP_DIR)/src/renderer
	@mkdir -p $(DEP_DIR)/src/utils
	@mkdir -p $(DEP_DIR)/src/ui
	@mkdir -p $(DEP_DIR)/include/glad/src
	@mkdir -p $(DEP_DIR)/lib/imgui
	@mkdir -p $(DEP_DIR)/lib/imgui/backends
	@mkdir -p $(LIB_DIR)

glm:
	@if [ ! -d "$(GLMDIR)" ]; then \
		echo "$(YELLOW)GLM library not found, cloning from GitHub...$(DEF_COLOR)"; \
		git clone https://github.com/g-truc/glm.git $(GLMDIR); \
		echo "$(GREEN)GLM library cloned successfully!$(DEF_COLOR)"; \
	fi

imgui:
	@if [ ! -d "$(IMGUIDIR)" ]; then \
		echo "$(YELLOW)ImGui library not found, cloning from GitHub...$(DEF_COLOR)"; \
		git clone https://github.com/ocornut/imgui.git $(IMGUIDIR); \
		echo "$(GREEN)ImGui library cloned successfully!$(DEF_COLOR)"; \
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

# -=-=-=-=-    CLEANUP -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

clean:
	@$(RM) $(OBJ_DIR) $(DEP_DIR)
	@echo "$(RED)Cleaned object files and dependencies$(DEF_COLOR)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)Cleaned all binaries$(DEF_COLOR)"
	@$(RM) $(GLMDIR)
	@echo "$(RED)Cleaned GLM library$(DEF_COLOR)"
	@$(RM) $(IMGUIDIR)
	@echo "$(RED)Cleaned ImGui library$(DEF_COLOR)"

re: fclean all

list-models:
	@echo "$(CYAN)Available 3D models:$(DEF_COLOR)"
	@ls -1 resources/objects/*.obj 2>/dev/null || echo "$(RED)No .obj files found in resources/$(DEF_COLOR)"

check-deps:
	@echo "$(CYAN)Checking OpenGL dependencies...$(DEF_COLOR)"
	@pkg-config --exists glfw3 && echo "$(GREEN)✓ GLFW3 found$(DEF_COLOR)" || echo "$(RED)✗ GLFW3 not found$(DEF_COLOR)"
	@pkg-config --exists gl && echo "$(GREEN)✓ OpenGL found$(DEF_COLOR)" || echo "$(RED)✗ OpenGL not found$(DEF_COLOR)"
	@if [ -d "$(GLMDIR)" ]; then echo "$(GREEN)✓ GLM found$(DEF_COLOR)"; else echo "$(RED)✗ GLM not found$(DEF_COLOR)"; fi
	@if [ -d "$(IMGUIDIR)" ]; then echo "$(GREEN)✓ ImGui found$(DEF_COLOR)"; else echo "$(RED)✗ ImGui not found$(DEF_COLOR)"; fi

.PHONY: all clean fclean re directories glm doxy doxyclean run run42 list-models check-deps