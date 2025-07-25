#include "App.hpp"
#include "../renderer/Renderer.hpp"
#include "../renderer/Mesh.hpp"
#include "../renderer/Shader.hpp"
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

App::App() : window(nullptr) {
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

void App::run() {
	if (!window) return;

	float positions[6] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.0f,  0.5f 
	};

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, positions, GL_STATIC_DRAW);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
