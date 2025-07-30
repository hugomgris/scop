/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:41 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/30 17:16:42 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/App.hpp"
#include "../../include/Renderer.hpp"
#include "../../include/Mesh.hpp"
#include "../../include/Shader.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

App::App(Parser *parser) : _parser(parser), window(nullptr) {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1920, 1080, "SCOP aka FDF3D aka the renderer of worlds", nullptr, nullptr);

	if (!window) {
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		glfwTerminate();
		window = nullptr;
		return;
	}

	glViewport(0, 0, 1920, 1080);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

App::~App() {
	if (window) {
		glfwDestroyWindow(window);
	}
	
	glfwTerminate();
}

void App::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	(void)window;

	glViewport(0, 0, width, height);
}

static ShaderProgramSource ParseShader(const std::string &filepath) {
	std::ifstream stream(filepath);
	if (!stream.is_open()) {
		std::cerr << "Failed to open shader file: " << filepath << "\n";
		return {};
	}

	enum class shaderType {
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	shaderType currentShaderType = shaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				currentShaderType = shaderType::VERTEX;
			} else if (line.find("fragment") != std::string::npos) {
				currentShaderType = shaderType::FRAGMENT;
			}
		} else if (currentShaderType != shaderType::NONE) {
			ss[static_cast<int>(currentShaderType)] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string &source) {
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	//! Todo: expanded error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char *)(alloca(length * sizeof(char)));
		glGetShaderInfoLog(id, length, &length, message);
		std::cerr << "Failed to compile "
			<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader: "
			<< message << "\n";
		glDeleteShader(id);
		return 0;
	}

	return (id);
}

static unsigned int createShader(const std::string &vertexShader, const std::string &fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return (program);
}

void App::run() {
    if (!window) return;

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _parser->getVertices().size(), _parser->getVertices().data(), GL_STATIC_DRAW);

	unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _parser->getIndices().size(), _parser->getIndices().data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnable(GL_DEPTH_TEST);

	ShaderProgramSource shaderSource = ParseShader("resources/shaders/Basic.shader");

    unsigned int shader = createShader(shaderSource.VertexSource, shaderSource.FragmentSource);
    glUseProgram(shader);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Set up projection matrix
	/* glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f/1080.0f, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 model = glm::mat4(1.0f);

	// Get uniform locations and set matrices
	int projLoc = glGetUniformLocation(shader, "u_Projection");
	int viewLoc = glGetUniformLocation(shader, "u_View");
	int modelLoc = glGetUniformLocation(shader, "u_Model");

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); */

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)_parser->getIndices().size(), GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &buffer);
} 
