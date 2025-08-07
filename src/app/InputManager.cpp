/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 13:50:59 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/07 17:25:47 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/InputManager.hpp"

/**
 * InputManager Constructor - Sets up input handling and camera system
 * 
 * FLOW:
 * 1. Initialize member variables with default values
 * 2. Set up GLFW callback system using wrapper functions
 * 3. Configure cursor mode for normal interaction
 * 4. Calculate optimal camera position based on model bounding box
 * 
 * The constructor establishes the complete input handling pipeline,
 * connecting GLFW events to class methods via static wrapper functions.
 */
InputManager::InputManager(GLFWwindow *window, int mode, float optimalDistance, const BoundingBox &boundingBox): 
    _window(window), _mode(mode), _boundingBox(boundingBox),
    _deltaTime(0.0f), _lastFrame(0.0f), _mouseInteraction(MouseInteraction::None),
    _modelOffset(0.0f), _modelRotation(0.0f), _fov(45.0f), _zoomLevel(1.0f),
    _aspectRatio(16.0f/9.0f), _useOrthographic(false), _wireframeMode(false), 
    _showVertices(false), _autoRotation(false), _autoRotationSpeed(30.0f),
    _defaultDistance(optimalDistance) {
    
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallbackWrapper);
    glfwSetCursorPosCallback(window, mouseCallbackWrapper);
    glfwSetScrollCallback(window, scrollCallbackWrapper);
    glfwSetKeyCallback(window, keyCallbackWrapper);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    calculateOptimalCameraPosition();
}

InputManager::~InputManager() {}

float InputManager::getDeltaTime() const {
	return _deltaTime;
}

float InputManager::getLastFrame() const {
	return _lastFrame;
}

glm::vec3 InputManager::getCameraPosition() const {
    return _fixedCameraPos;
}

std::vector<glm::mat4> InputManager::getMatrices() const {
	std::vector<glm::mat4> matrices;
	matrices.push_back(_model);
	matrices.push_back(_view);
	matrices.push_back(_projection);
	return (matrices);
}

bool InputManager::getAutorotationStatus() const {
    return _autoRotation;
}

void InputManager::setDeltaTime(float currentFrame) {
	_deltaTime = currentFrame - _lastFrame;
	_lastFrame = currentFrame;
}

void InputManager::setAspectRatio(float aspectRatio) {
	_aspectRatio = aspectRatio;
}

void InputManager::setViewportBounds(const glm::vec4& bounds) {
    _viewportBounds = bounds;
}

void InputManager::setUseOrthographic(bool useOrtho) {
    _useOrthographic = useOrtho;
}

void InputManager::setAutoRotation(bool autoRotation) {
    _autoRotation = autoRotation;
}

void InputManager::setEnableCRT(bool enableCRT) {
    _enableCRT = enableCRT;
}

void InputManager::setProjectionToggleCallback(std::function<void(bool)> callback) {
    _onProjectionToggle = callback;
}

void InputManager::setWireframeToggleCallback(std::function<void(bool)> callback) {
    _onWireframeToggle = callback;
}

void InputManager::setModelRotation(float x, float y) {
    _modelRotation.x = x;
    _modelRotation.y = y;
}

void InputManager::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	(void)window;
	glViewport(0, 0, width, height);
}

void InputManager::mouseButtonCallbackWrapper(GLFWwindow* window, int button, int action, int mods) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    inputManager->mouseButtonCallback(window, button, action, mods);
}

/**
 * Mouse Button Callback - Handles mouse press/release events for 3D interaction
 * 
 * FLOW:
 * 1. Get current cursor position from GLFW
 * 2. Check if cursor is within the 3D viewport bounds
 * 3. On press: Store initial position and set interaction mode
 * 4. On release: Clear interaction mode
 * 
 * Interaction modes:
 * - Left click: Pan model (translate)
 * - Right click: Rotate model
 * - Outside viewport: Ignore interaction
 */
void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    if (xpos < _viewportBounds.x || xpos > _viewportBounds.z || 
        ypos < _viewportBounds.y || ypos > _viewportBounds.w) {
        _mouseInteraction = MouseInteraction::None;
        return;
    }
    
    if (action == GLFW_PRESS) {
        _lastMouseX = xpos;
        _lastMouseY = ypos;
        
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            _mouseInteraction = MouseInteraction::PanModel;
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            _mouseInteraction = MouseInteraction::RotateModel;
        }
    } else if (action == GLFW_RELEASE) {
        _mouseInteraction = MouseInteraction::None;
    }
}

void InputManager::mouseCallbackWrapper(GLFWwindow* window, double xpos, double ypos) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    inputManager->mouseCallback(window, xpos, ypos);
}

/**
 * Mouse Movement Callback - Handles mouse drag operations for model manipulation
 * 
 * FLOW:
 * 1. Check if any mouse interaction is active
 * 2. Verify cursor remains within viewport bounds
 * 3. Calculate movement delta from last position
 * 4. Apply transformation based on interaction mode:
 *    - Pan mode: Translate model in screen space
 *    - Rotate mode: Rotate model with pitch/yaw constraints
 * 5. Update last mouse position for next frame
 */
void InputManager::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    
    if (_mouseInteraction == MouseInteraction::None) {
        return;
    }

    if (xpos < _viewportBounds.x || xpos > _viewportBounds.z || 
        ypos < _viewportBounds.y || ypos > _viewportBounds.w) {
        return;
    }
    
    float xoffset = xpos - _lastMouseX;
    float yoffset = _lastMouseY - ypos;
    
    _lastMouseX = xpos;
    _lastMouseY = ypos;
    
    if (_mouseInteraction == MouseInteraction::PanModel) {
        float sensitivity = 0.02f;
        _modelOffset.x += xoffset * sensitivity;
        _modelOffset.y += yoffset * sensitivity;
    } else if (_mouseInteraction == MouseInteraction::RotateModel) {
        float sensitivity = 0.5f;
        _modelRotation.y += xoffset * sensitivity;
        _modelRotation.x += yoffset * sensitivity;

        if (_modelRotation.x > 89.0f) _modelRotation.x = 89.0f;
        if (_modelRotation.x < -89.0f) _modelRotation.x = -89.0f;
    }
}

void InputManager::scrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    inputManager->scrollCallback(window, xoffset, yoffset);
}

void InputManager::keyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    inputManager->keyCallback(window, key, scancode, action, mods);
}

/**
 * Scroll Callback - Handles mouse wheel events for zoom control
 * 
 * FLOW:
 * 1. Get current mouse position to check viewport bounds
 * 2. Ignore scroll events outside the 3D viewport
 * 3. Apply zoom based on current projection mode:
 *    - Orthographic: Adjust zoom level (0.1x to 5.0x range)
 *    - Perspective: Adjust field of view (1° to 45° range)
 * 4. Clamp values to prevent extreme zoom levels
 */
void InputManager::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;
    
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    
    if (mouseX < _viewportBounds.x || mouseX > _viewportBounds.z || 
        mouseY < _viewportBounds.y || mouseY > _viewportBounds.w) {
        return;
    }
    
    if (_useOrthographic) {
        _zoomLevel -= (float)yoffset * 0.1f;
        if (_zoomLevel < 0.1f) _zoomLevel = 0.1f;
        if (_zoomLevel > 5.0f) _zoomLevel = 5.0f;
    } else {
        _fov -= (float)yoffset;
        if (_fov < 1.0f) _fov = 1.0f;
        if (_fov > 45.0f) _fov = 45.0f;
    }
}

/**
 * Keyboard Callback - Handles keyboard shortcuts for application control
 * 
 * FLOW:
 * 1. Check for key press events (ignore repeats and releases for most keys)
 * 2. Execute corresponding action based on key:
 *    - ESC: Close application
 *    - V: Toggle wireframe mode (OBJ only)
 *    - P: Toggle projection mode (perspective/orthographic)
 *    - 1: Toggle auto-rotation
 *    - R: Reset view to default
 *    - X: Toggle vertex visualization
 *    - C: Toggle CRT post-processing effect
 *    - T: Toggle texture rendering mode
 * 3. Trigger appropriate callback to notify other components
 */
void InputManager::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        if (_mode != FDF) {
            _wireframeMode = !_wireframeMode;
        
            if (_onWireframeToggle) {
                _onWireframeToggle(_wireframeMode);
            }
        }
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        _useOrthographic = !_useOrthographic;
        
        if (_onProjectionToggle) {
            _onProjectionToggle(_useOrthographic);
        }
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        _autoRotation = !_autoRotation;
        
        if (_onAutoRotationToggle) {
            _onAutoRotationToggle(_autoRotation);
        }
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        resetView();
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        _showVertices = !_showVertices;
        
        if (_onVertexToggle) {
            _onVertexToggle(_showVertices);
        }
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        _enableCRT = !_enableCRT;
        
        if (_onCRTToggle) {
            _onCRTToggle(_enableCRT);
        }
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        _useTexture = !_useTexture;
        
        if (_onTextureToggle) {
            _onTextureToggle(_useTexture);
        }
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        resetView();
    }
}

void InputManager::setVertexToggleCallback(std::function<void(bool)> callback) {
    _onVertexToggle = callback;
}

void InputManager::setAutoRotationToggleCallback(std::function<void(bool)> callback) {
    _onAutoRotationToggle = callback;
}

void InputManager::setCRTToggleCallback(std::function<void(bool)> callback) {
    _onCRTToggle = callback;
}

void InputManager::setTextureToggleCallback(std::function<void(bool)> callback) {
    _onTextureToggle = callback;
}

void InputManager::calculateOptimalCameraPosition() {
    float boundingBoxDiagonal = _boundingBox.getDiagonal();
    float distance = boundingBoxDiagonal * 1.5f;
    
    _fixedCameraPos = glm::vec3(0.0f, 0.0f, distance);
    _fixedCameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    _fixedCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

void InputManager::resetModelTransform() {
    glm::vec3 modelCenter = _boundingBox.getCenter();
    _modelOffset = -modelCenter;
    _modelRotation = glm::vec3(0.0f);
}

void InputManager::processInput() {}

/**
 * Create Transformation Matrices - Generates Model-View-Projection matrices
 * 
 * FLOW:
 * 1. Update auto-rotation if enabled (continuous Y-axis rotation)
 * 2. Construct Model matrix:
 *    - Apply translation (user panning)
 *    - Apply rotations (X, Y, Z axes in order)
 * 3. Construct View matrix using fixed camera (lookAt)
 * 4. Construct Projection matrix based on mode:
 *    - Orthographic: Use specialized projection for model type
 *    - Perspective: Standard perspective projection with FOV
 * 5. Store matrices for renderer access
 */
void InputManager::createMatrices() {
    if (_autoRotation) {
        _modelRotation.y += _autoRotationSpeed * _deltaTime;
    } 
    
    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model, _modelOffset);
    
    model = glm::rotate(model, glm::radians(_modelRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_modelRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_modelRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    
    glm::mat4 view = glm::lookAt(_fixedCameraPos, _fixedCameraTarget, _fixedCameraUp);

    glm::mat4 projection;
    if (_useOrthographic) {
        if (_mode == FDF) {
            projection = createOrthographicProjectionForFDF(_aspectRatio, 19, 11, 1.5f * _zoomLevel);
        } else {
            projection = createOrthographicProjection(_aspectRatio, _zoomLevel);
        }
    } else {
        projection = glm::perspective(glm::radians(_fov), _aspectRatio, 0.1f, 1000.0f);
    }

    _model = model;
    _view = view;
    _projection = projection;
}

/**
 * Create Orthographic Projection - Generates orthographic projection matrix for OBJ models
 * 
 * FLOW:
 * 1. Calculate orthographic view size based on model bounding box diagonal
 * 2. Apply zoom factor to scale the view volume
 * 3. Calculate view bounds:
 *    - Width adjusted for aspect ratio (left/right)
 *    - Height maintains square proportions (top/bottom)
 *    - Fixed near/far planes for general 3D models
 * 4. Return GLM orthographic projection matrix
 */
glm::mat4 InputManager::createOrthographicProjection(float aspectRatio, float zoom) {
    float orthoSize = _boundingBox.getDiagonal() / zoom;
    
    float left = -orthoSize * aspectRatio;
    float right = orthoSize * aspectRatio;
    float bottom = -orthoSize;
    float top = orthoSize;
    float nearPlane = -50.0f;
    float farPlane = 50.0f;
    
    return glm::ortho(left, right, bottom, top, nearPlane, farPlane);
}

/**
 * Create Orthographic Projection for FDF - Specialized projection for heightmap visualization
 * 
 * FLOW:
 * 1. Calculate actual map dimensions in world space
 *    - Map width = columns × grid spacing
 *    - Map height = rows × grid spacing
 * 2. Apply fixed zoom factor optimized for heightmap viewing
 * 3. Determine orthographic view bounds:
 *    - Use maximum dimension to maintain proportions
 *    - Adjust for aspect ratio to prevent distortion
 *    - Set wide near/far planes for large height variations
 * 4. Return projection matrix suited for top-down heightmap view
 */
glm::mat4 InputManager::createOrthographicProjectionForFDF(float aspectRatio, int mapRows, int mapCols, float spacing) {
    float mapWidth = mapCols * spacing;
    float mapHeight = mapRows * spacing;
    
    float zoomFactor = 0.5f;
    
    float maxDimension = std::max(mapWidth, mapHeight);
    float orthoSize = (maxDimension * zoomFactor) / 2.0f;
    
    float orthoWidth = orthoSize * aspectRatio;
    float orthoHeight = orthoSize;
    
    return glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -5000.0f, 10000.0f);
}

void InputManager::resetView() {
    calculateOptimalCameraPosition();
    resetModelTransform();
    _fov = 45.0f;
    _zoomLevel = 2.0f;
    std::cout << "View reset to default" << std::endl;
}