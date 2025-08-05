/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:41 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 12:09:12 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/App.hpp"

App::App(int mode, Mesh *mesh, Shader *shader, Renderer *renderer, Parser *parser)
    : _mode(mode), _parser(parser), _mesh(mesh), _shader(shader), _renderer(renderer), 
      _window(nullptr), _wireframeMode(false), _showVertices(false) {
        
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _window = glfwCreateWindow(1920, 1080, "SCOP aka FDFGL aka the renderer of worlds", nullptr, nullptr);

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

    float optimalDistance = _parser->getOptimalCameraDistance();
    
    _inputManager = std::make_unique<InputManager>(_window, _mode, optimalDistance, _parser->getBoundingBox());
    _textureLoader = std::make_unique<TextureLoader>();

    _inputManager->setProjectionToggleCallback([this](bool useOrtho) {
        this->handleProjectionToggle(useOrtho);
    });
    
    _inputManager->setWireframeToggleCallback([this](bool wireframeMode) {
        this->handleWireframeToggle(wireframeMode);
    });
    
    _inputManager->setVertexToggleCallback([this](bool showVertices) {
        this->handleVertexToggle(showVertices);
    });
}

App::~App() {
	if (_window) {
		glfwDestroyWindow(_window);
	}
	
	glfwTerminate();
}

void App::run() {
    if (!_window) return;

    // Try to load texture from MTL materials first
    const auto& materials = _parser->getMaterials();
    
    if (!materials.empty()) {
        // Use the first material that has a texture
        for (const auto& material : materials) {
            _currentTexture = _textureLoader->loadMaterialTextures(material);
            if (_currentTexture) {
                std::cout << "Using texture from material: " << material.name << std::endl;
                break;
            }
        }
    }
    
    // Fallback to hardcoded texture if no material textures found
    if (!_currentTexture) {
        std::cout << "No material textures found, using fallback texture" << std::endl;
        _currentTexture = _textureLoader->loadTexture("resources/textures/Unicorns.png");
    }
    
    // If we still don't have a texture, we'll render without texture
    if (_currentTexture) {
        _currentTexture->Bind();
    }

    _mesh->bind();

    _shader->compile();
    _shader->use();
    _shader->setUniform("u_texture", 0);

    while (!glfwWindowShouldClose(_window)) {
        float currentFrame = glfwGetTime();
        _inputManager->setDeltaTime(currentFrame);
        
        _inputManager->processInput();
        _inputManager->createMatrices();
        
        std::vector<glm::mat4> matrices = _inputManager->getMatrices();
        
        _renderer->setMatrices(matrices[0], matrices[1], matrices[2]);
        if (_currentTexture) {
            _currentTexture->Bind(0);
        }
        _renderer->draw(*_mesh, _mode, _inputManager->getCameraPosition(), _showVertices);

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}

void App::handleProjectionToggle(bool useOrthographic) {
    _useOrthographic = useOrthographic;
    std::cout << "Switched to " << (useOrthographic ? "Orthographic" : "Perspective") 
              << " projection" << std::endl;
}

void App::handleWireframeToggle(bool wireframeMode) {
    _wireframeMode = wireframeMode;
    
    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    std::cout << "Wireframe mode " << (wireframeMode ? "ON" : "OFF") << std::endl;
}

void App::handleVertexToggle(bool showVertices) {
    _showVertices = showVertices;
    std::cout << "Vertex visualization " << (showVertices ? "ON" : "OFF") << std::endl;
}

glm::mat4 App::createProjectionMatrix() {
    float aspectRatio = (float)1920 / (float)1080;
    
    if (_useOrthographic) {
        if (_mode == OBJ) {
            return _inputManager->createOrthographicProjection(aspectRatio, 1.f);
        } else {
            return _inputManager->createOrthographicProjectionForFDF(aspectRatio, _parser->getRows(), _parser->getColumns(), 1.f);
        }
    } else {
        return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);
    }
}