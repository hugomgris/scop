/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:41 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/06 14:36:25 by hmunoz-g         ###   ########.fr       */
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
    _uiManager = std::make_unique<UIManager>(_window);
    _postProcessor = std::make_unique<PostProcessor>(1920, 1080);

    if (!_uiManager->initialize()) {
        std::cerr << "Failed to initialize UI\n";
    }

    // Ensure model is properly centered on startup
    _inputManager->resetView();

    setupUICallbacks();
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
        
        _uiManager->updateMeshInfo(_parser);
        _uiManager->updateCameraInfo(_inputManager.get());
        _uiManager->updatePerformanceStats(_inputManager->getDeltaTime());
        
        _uiManager->newFrame();
        
        _inputManager->processInput();
        
        const auto& layout = _uiManager->getLayout();
        
        float windowPadding = 8.0f;
        float borderSize = 2.0f;
        
        int viewportX = static_cast<int>(layout.renderAreaPos.x + windowPadding + borderSize);
        int viewportY = static_cast<int>(layout.windowHeight - layout.renderAreaPos.y - layout.renderAreaSize.y + windowPadding + borderSize);
        int viewportWidth = static_cast<int>(layout.renderAreaSize.x - (windowPadding + borderSize) * 2);
        int viewportHeight = static_cast<int>(layout.renderAreaSize.y - (windowPadding + borderSize) * 2);
        
        viewportWidth = std::max(viewportWidth, 1);
        viewportHeight = std::max(viewportHeight, 1);
        
        // Calculate aspect ratio using actual viewport dimensions
        float aspectRatio = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);
        
        _inputManager->setAspectRatio(aspectRatio);
        
        _inputManager->createMatrices();
        
        std::vector<glm::mat4> matrices = _inputManager->getMatrices();
        
        _renderer->setMatrices(matrices[0], matrices[1], matrices[2]);
        
        // DEBUG
        static bool firstRun = true;
        if (firstRun) {
            std::cout << "Render area pos: (" << layout.renderAreaPos.x << ", " << layout.renderAreaPos.y << ")" << std::endl;
            std::cout << "Render area size: (" << layout.renderAreaSize.x << ", " << layout.renderAreaSize.y << ")" << std::endl;
            std::cout << "Window size: (" << layout.windowWidth << ", " << layout.windowHeight << ")" << std::endl;
            std::cout << "Viewport: X=" << viewportX << ", Y=" << viewportY 
                      << ", W=" << viewportWidth << ", H=" << viewportHeight << std::endl;
            std::cout << "Aspect ratio: " << aspectRatio << std::endl;
            
            glm::vec3 camPos = _inputManager->getCameraPosition();
            std::cout << "Camera position: (" << camPos.x << ", " << camPos.y << ", " << camPos.z << ")" << std::endl;
            
            firstRun = false;
        }

        glm::vec4 viewportBounds(
            layout.renderAreaPos.x + windowPadding + borderSize,
            layout.renderAreaPos.y + windowPadding + borderSize,
            layout.renderAreaPos.x + layout.renderAreaSize.x - windowPadding - borderSize,
            layout.renderAreaPos.y + layout.renderAreaSize.y - windowPadding - borderSize
        );
        _inputManager->setViewportBounds(viewportBounds);
        
        // Update PostProcessor size if needed
        if (viewportWidth > 0 && viewportHeight > 0) {
            static int lastWidth = 0, lastHeight = 0;
            if (lastWidth != viewportWidth || lastHeight != viewportHeight) {
                _postProcessor->resize(viewportWidth, viewportHeight);
                lastWidth = viewportWidth;
                lastHeight = viewportHeight;
            }
        }
        
        // Update time for CRT effect
        _postProcessor->updateTime(currentFrame);
        
        // Bind PostProcessor framebuffer for off-screen 3D scene rendering
        _postProcessor->bind();
        
        // Ensure proper OpenGL state for 3D rendering
        glDisable(GL_SCISSOR_TEST);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        
        // Disable face culling for now to debug face rendering issues
        glDisable(GL_CULL_FACE);
        
        setClearColor(Colors::BLACK_CHARCOAL_1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        const auto& materialGroups = _parser->getMaterialGroups();
        
        if (!materialGroups.empty() && !_materialTextures.empty()) {
            static bool debugMaterials = true;
            if (debugMaterials) {
                std::cout << "Rendering with materials: " << materialGroups.size() << " groups" << std::endl;
                debugMaterials = false;
            }
            renderWithMaterials();
        } else {
            static bool debugFallback = true;
            if (debugFallback) {
                std::cout << "Rendering with fallback (no materials)" << std::endl;
                debugFallback = false;
            }
            if (_currentTexture) {
                _currentTexture->Bind(0);
            }
            _renderer->draw(*_mesh, _mode, _inputManager->getCameraPosition(), _showVertices, _wireframeMode);
        }
        
        // Unbind framebuffer and return to default framebuffer
        _postProcessor->unbind();
        
        // Clear the entire screen first (using full window viewport)
        glViewport(0, 0, 1920, 1080);
        setClearColor(Colors::BLACK_CHARCOAL_1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set CRT mode on the post processor
        _postProcessor->setEnableCRT(_uiManager->getState().enableCRT);
        
        // Set viewport to match the render area for correct aspect ratio
        glViewport(viewportX, 1080 - viewportY - viewportHeight, viewportWidth, viewportHeight);
        
        // Disable depth testing and face culling for post-processing quad
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        
        // Render the post-processed 3D scene to the viewport area
        _postProcessor->render();
        
        // Now render UI on top of everything (reset to full viewport)
        glViewport(0, 0, 1920, 1080);
        _uiManager->render();        // No need to render UI again or reset viewport since it's already done

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}

void App::renderWithMaterials() {
    const auto& materialGroups = _parser->getMaterialGroups();
    
    // Don't clear here - clearing is already done in the main render loop
    // setClearColor(Colors::BLACK_CHARCOAL_1);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
            
            // Use optimized static buffer for material rendering
            static unsigned int tempIBO = 0;
            if (tempIBO == 0) {
                glGenBuffers(1, &tempIBO);
            }
            
            GLCall(glBindVertexArray(_mesh->getVAO()));
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempIBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                         group.indices.size() * sizeof(unsigned int), 
                         group.indices.data(), 
                         GL_STREAM_DRAW);
            
            GLCall(glDrawElements(renderMode, static_cast<GLsizei>(group.indices.size()), GL_UNSIGNED_INT, nullptr));
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

void App::setCurrentFile(const std::string& filename) {
    if (_uiManager) {
        _uiManager->setCurrentFile(filename);
    }
}

void App::setupUICallbacks() {
    _inputManager->setProjectionToggleCallback([this](bool useOrtho) {
        this->handleProjectionToggle(useOrtho);
    });
    
    _inputManager->setWireframeToggleCallback([this](bool wireframeMode) {
        this->handleWireframeToggle(wireframeMode);
    });
    
    _inputManager->setVertexToggleCallback([this](bool showVertices) {
        this->handleVertexToggle(showVertices);
    });
    
    _inputManager->setAutoRotationToggleCallback([this](bool autoRotation) {
        this->handleAutoRotationToggle(autoRotation);
    });
    
    _inputManager->setCRTToggleCallback([this](bool enableCRT) {
        this->handleCRTToggle(enableCRT);
    });
    
    _uiManager->onWireframeModeChanged = [this](bool wireframeMode) {
        this->handleWireframeToggle(wireframeMode);
    };
    
    _uiManager->onVertexModeChanged = [this](bool showVertices) {
        this->handleVertexToggle(showVertices);
    };
    
    _uiManager->onProjectionModeChanged = [this](bool useOrtho) {
        this->handleProjectionToggle(useOrtho);
    };
    
    _uiManager->onAutoRotationChanged = [this](bool autoRotation) {
        this->handleAutoRotationToggle(autoRotation);
    };
    
    _uiManager->onCRTModeChanged = [this](bool enableCRT) {
        this->handleCRTToggle(enableCRT);
        // Also update InputManager state to keep them in sync
        if (_inputManager) {
            _inputManager->setEnableCRT(enableCRT);
        }
    };
    
    _uiManager->onResetCamera = [this]() {
        if (_inputManager) {
            _inputManager->resetView();
        }
    };
}

void App::handleProjectionToggle(bool useOrthographic) {
    _useOrthographic = useOrthographic;

    _inputManager->resetView();
    if (_useOrthographic) {
        _inputManager->setModelRotation(35.265f, 45.0f);
    } else {
        _inputManager->setModelRotation(0.0f, 0.0f);
    }
    
    if (_inputManager) {
        _inputManager->setUseOrthographic(useOrthographic);
    }
    
    if (_uiManager) {
        UIState currentState = _uiManager->getState();
        currentState.orthographicProjection = useOrthographic;
        _uiManager->updateState(currentState);
    }
    
    std::cout << "Switched to " << (useOrthographic ? "Orthographic" : "Perspective") 
              << " projection" << std::endl;
}

void App::handleWireframeToggle(bool wireframeMode) {
    _wireframeMode = wireframeMode;
    
    if (_uiManager) {
        UIState currentState = _uiManager->getState();
        currentState.wireframeMode = wireframeMode;
        _uiManager->updateState(currentState);
    }
    
    std::cout << "Wireframe mode " << (wireframeMode ? "ON" : "OFF") << std::endl;
}

void App::handleVertexToggle(bool showVertices) {
    _showVertices = showVertices;
    
    if (_uiManager) {
        UIState currentState = _uiManager->getState();
        currentState.showVertices = showVertices;
        _uiManager->updateState(currentState);
    }
    
    std::cout << "Vertex visualization " << (showVertices ? "ON" : "OFF") << std::endl;
}

void App::handleAutoRotationToggle(bool autoRotation) {
    if (_inputManager) {
        _inputManager->setAutoRotation(autoRotation);
    }
    
    if (_uiManager) {
        UIState currentState = _uiManager->getState();
        currentState.autoRotation = autoRotation;
        _uiManager->updateState(currentState);
    }
    
    std::cout << "Auto-rotation " << (autoRotation ? "ON" : "OFF") << std::endl;
}

void App::handleCRTToggle(bool enableCRT) {
    _enableCRT = enableCRT;
    
    if (_postProcessor) {
        _postProcessor->setEnableCRT(_enableCRT);
    }
    
    if (_uiManager) {
        UIState currentState = _uiManager->getState();
        currentState.enableCRT = _enableCRT;
        _uiManager->updateState(currentState);
    }
    
    std::cout << "CRT Effect " << (_enableCRT ? "ON" : "OFF") << std::endl;
}