/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:41 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/31 10:49:53 by hmunoz-g         ###   ########.fr       */
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



App::App(Mesh *mesh, Shader *shader, Renderer *renderer, int mode) : _mesh(mesh), _shader(shader), _renderer(renderer), _mode(mode), window(nullptr) {
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

	_mesh->bind();

	_shader->compile();

    while (!glfwWindowShouldClose(window)) {
        _renderer->draw(*_mesh, _mode);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
