/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:41 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 15:07:14 by hmunoz-g         ###   ########.fr       */
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

    const auto& materials = _parser->getMaterials();
    
    if (!materials.empty()) {
        _materialTextures = _textureLoader->loadAllMaterialTextures(materials);
        
        if (!_materialTextures.empty()) {
            _currentTexture = _materialTextures.begin()->second;
            std::cout << "Using multi-material rendering with " << _materialTextures.size() << " textures" << std::endl;
        }
    }
    
    if (!_currentTexture) {
        std::cout << "No material textures found, using fallback texture" << std::endl;
        _currentTexture = _textureLoader->loadTexture("resources/textures/Unicorns.png");
    }

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
        
        const auto& materialGroups = _parser->getMaterialGroups();
        
        if (!materialGroups.empty() && !_materialTextures.empty()) {
            renderWithMaterials();
        } else {
            if (_currentTexture) {
                _currentTexture->Bind(0);
            }
            _renderer->draw(*_mesh, _mode, _inputManager->getCameraPosition(), _showVertices, _wireframeMode);
        }

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}

void App::renderWithMaterials() {
    const auto& materialGroups = _parser->getMaterialGroups();
    
    setClearColor(Colors::BLACK_CHARCOAL_1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& group : materialGroups) {
        if (group.indices.empty()) continue;
        
        auto textureIt = _materialTextures.find(group.materialIndex);
        if (textureIt != _materialTextures.end()) {
            textureIt->second->Bind(0);
        } else if (_currentTexture) {
            _currentTexture->Bind(0);
        }
        
        _shader->use();
        
        if (_wireframeMode) {
            // In wireframe mode, use the mesh's wireframe indices for all material groups
            int lineColorLoc = glGetUniformLocation(_shader->getID(), "u_lineColor");
            int isLineModeLoc = glGetUniformLocation(_shader->getID(), "u_isLineMode");
            int isVertexModeLoc = glGetUniformLocation(_shader->getID(), "u_isVertexMode");
            
            setLineColor(lineColorLoc, Colors::OFF_WHITE);
            glLineWidth(2.0f);
            glUniform1i(isLineModeLoc, 1);
            glUniform1i(isVertexModeLoc, 0);
            
            GLCall(glBindVertexArray(_mesh->getVAO()));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->getWireframeIBO());
            GLCall(glDrawElements(GL_LINES, static_cast<GLsizei>(_mesh->getWireframeIndexCount()), GL_UNSIGNED_INT, nullptr));
            
            // Only render wireframe once for all material groups
            break;
        } else {
            int renderMode = _mode == 0 ? GL_TRIANGLES : GL_LINES;
            
            if (renderMode == GL_LINES) {
                int lineColorLoc = glGetUniformLocation(_shader->getID(), "u_lineColor");
                int isLineModeLoc = glGetUniformLocation(_shader->getID(), "u_isLineMode");
                int isVertexModeLoc = glGetUniformLocation(_shader->getID(), "u_isVertexMode");
                
                setLineColor(lineColorLoc, Colors::OFF_WHITE);
                glLineWidth(2.0f);
                glUniform1i(isLineModeLoc, 1);
                glUniform1i(isVertexModeLoc, 0);
            } else {
                int isLineModeLoc = glGetUniformLocation(_shader->getID(), "u_isLineMode");
                int isVertexModeLoc = glGetUniformLocation(_shader->getID(), "u_isVertexMode");
                glUniform1i(isLineModeLoc, 0);
                glUniform1i(isVertexModeLoc, 0);
                
                int colorLoc = glGetUniformLocation(_shader->getID(), "u_color");
                int lightPosLoc = glGetUniformLocation(_shader->getID(), "u_lightPos");
                int lightColorLoc = glGetUniformLocation(_shader->getID(), "u_lightColor");
                int viewPosLoc = glGetUniformLocation(_shader->getID(), "u_viewPos");
                int textureLoc = glGetUniformLocation(_shader->getID(), "u_texture");
                int useTextureLoc = glGetUniformLocation(_shader->getID(), "useTexture");
                
                glUniform3f(colorLoc, 0.5, 0.5, 0.9);
                
                glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
                glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
                
                glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
                glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
                
                glUniform3fv(viewPosLoc, 1, glm::value_ptr(_inputManager->getCameraPosition()));
                
                glUniform1i(textureLoc, 0);
                glUniform1i(useTextureLoc, 1);
            }
            
            unsigned int materialIBO;
            glGenBuffers(1, &materialIBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, materialIBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                         group.indices.size() * sizeof(unsigned int), 
                         group.indices.data(), 
                         GL_DYNAMIC_DRAW);
            
            GLCall(glBindVertexArray(_mesh->getVAO()));
            GLCall(glDrawElements(renderMode, static_cast<GLsizei>(group.indices.size()), GL_UNSIGNED_INT, nullptr));
            
            glDeleteBuffers(1, &materialIBO);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->getIBO());
        }
    }
    
    if (_showVertices) {
        _shader->use();
        int isVertexModeLoc = glGetUniformLocation(_shader->getID(), "u_isVertexMode");
        int isLineModeLoc = glGetUniformLocation(_shader->getID(), "u_isLineMode");
        glUniform1i(isVertexModeLoc, 1);
        glUniform1i(isLineModeLoc, 0);
        
        int vertexColorLoc = glGetUniformLocation(_shader->getID(), "u_vertexColor");
        glUniform3f(vertexColorLoc, 1.0f, 1.0f, 0.0f);
        
        glEnable(GL_PROGRAM_POINT_SIZE);
        glPointSize(10.0f);
        
        GLCall(glBindVertexArray(_mesh->getVAO()));
        GLCall(glDrawArrays(GL_POINTS, 0, _mesh->getVertexCount()));
        
        glDisable(GL_PROGRAM_POINT_SIZE);
    }
}

void App::handleProjectionToggle(bool useOrthographic) {
    _useOrthographic = useOrthographic;
    std::cout << "Switched to " << (useOrthographic ? "Orthographic" : "Perspective") 
              << " projection" << std::endl;
}

void App::handleWireframeToggle(bool wireframeMode) {
    _wireframeMode = wireframeMode;
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