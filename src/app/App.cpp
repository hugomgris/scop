/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:41 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/31 18:14:41 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/App.hpp"

App::App(int mode, Mesh *mesh, Shader *shader, Renderer *renderer, Parser *parser)
    : _mode(mode), _parser(parser), _mesh(mesh), _shader(shader), _renderer(renderer), _window(nullptr) {
        
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(1920, 1080, "SCOP aka FDF3D aka the renderer of worlds", nullptr, nullptr);

	if (!_window) {
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(_window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n";
		glfwTerminate();
		_window = nullptr;
		return;
	}

	glViewport(0, 0, 1920, 1080);
	
	glEnable(GL_DEPTH_TEST);

	_inputManager = std::make_unique<InputManager>(_window);
}

App::~App() {
	if (_window) {
		glfwDestroyWindow(_window);
	}
	
	glfwTerminate();
}

void App::run() {
    if (!_window) return;

	_mesh->bind();
	_shader->compile();

    while (!glfwWindowShouldClose(_window)) {
        float currentFrame = glfwGetTime();
        _inputManager->setDeltaTime(currentFrame);
        
        _inputManager->processInput();
		_inputManager->createMatrices();
        
        // Pass matrices to renderer
		float aspectRatio = (float)1920 / (float)1080;
		glm::mat4 orthoProjection;
		
		if (_mode == OBJ) {
			orthoProjection = _inputManager->createOrthographicProjection(aspectRatio, 5.f);
		} else {
			orthoProjection = _inputManager->createOrthographicProjectionForFDF(aspectRatio, _parser->getRows(), _parser->getColumns(), 1.f); //last value should be calculated based on map size
		}

		std::vector<glm::mat4> matrices = _inputManager->getMatrices();
        _renderer->setMatrices(matrices[0], matrices[1], matrices[2]/* orthoProjection */);
        _renderer->draw(*_mesh, _mode);

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}