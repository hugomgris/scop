/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 13:50:59 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 17:31:12 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/InputManager.hpp"

InputManager::InputManager(GLFWwindow *window, int mode, float optimalDistance, const BoundingBox &boundingBox): 
    _window(window), _mode(mode), _boundingBox(boundingBox),
    _deltaTime(0.0f), _lastFrame(0.0f), _mouseInteraction(MouseInteraction::None),
    _modelOffset(0.0f), _modelRotation(0.0f), _fov(45.0f), _zoomLevel(1.0f),
    _aspectRatio(16.0f/9.0f), _useOrthographic(false), _wireframeMode(false), 
    _showVertices(false), _autoRotation(false), _autoRotationSpeed(30.0f),
    _defaultDistance(optimalDistance) {
    
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback_wrapper);
    glfwSetCursorPosCallback(window, mouse_callback_wrapper);
    glfwSetScrollCallback(window, scroll_callback_wrapper);
    glfwSetKeyCallback(window, key_callback_wrapper);
    
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

void InputManager::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	(void)window;
	glViewport(0, 0, width, height);
}

void InputManager::mouse_button_callback_wrapper(GLFWwindow* window, int button, int action, int mods) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    inputManager->mouse_button_callback(window, button, action, mods);
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
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

void InputManager::mouse_callback_wrapper(GLFWwindow* window, double xpos, double ypos) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    inputManager->mouse_callback(window, xpos, ypos);
}

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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
        float sensitivity = 0.005f;
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

void InputManager::scroll_callback_wrapper(GLFWwindow* window, double xoffset, double yoffset) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    inputManager->scroll_callback(window, xoffset, yoffset);
}

void InputManager::key_callback_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    inputManager->key_callback(window, key, scancode, action, mods);
}

void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;
    
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    
    if (mouseX < _viewportBounds.x || mouseX > _viewportBounds.z || 
        mouseY < _viewportBounds.y || mouseY > _viewportBounds.w) {
        return;
    }
    
    if (_useOrthographic) {
        _zoomLevel += (float)yoffset * 0.1f;
        if (_zoomLevel < 0.1f) _zoomLevel = 0.1f;
        if (_zoomLevel > 5.0f) _zoomLevel = 5.0f;
    } else {
        _fov -= (float)yoffset;
        if (_fov < 1.0f) _fov = 1.0f;
        if (_fov > 45.0f) _fov = 45.0f;
    }
}

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        _wireframeMode = !_wireframeMode;
        
        if (_onWireframeToggle) {
            _onWireframeToggle(_wireframeMode);
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

void InputManager::calculateOptimalCameraPosition() {
    float boundingBoxDiagonal = _boundingBox.getDiagonal();
    float distance = boundingBoxDiagonal * 1.5f;
    
    glm::vec3 modelCenter = _boundingBox.getCenter();
    _fixedCameraPos = modelCenter + glm::vec3(0.0f, 0.0f, distance);
    _fixedCameraTarget = modelCenter;
    _fixedCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}

void InputManager::resetModelTransform() {
    _modelOffset = glm::vec3(0.0f);
    _modelRotation = glm::vec3(0.0f);
}

void InputManager::processInput() {}

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
            projection = createOrthographicProjectionForFDF(_aspectRatio, 110, 190, 10.f * _zoomLevel);
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

glm::mat4 InputManager::createOrthographicProjectionForFDF(float aspectRatio, int mapRows, int mapCols, float spacing) {
    float mapWidth = mapCols * spacing;
    float mapHeight = mapRows * spacing;
    
    float zoomFactor = 0.5f;
    
    float maxDimension = std::max(mapWidth, mapHeight);
    float orthoSize = (maxDimension * zoomFactor) / 2.0f;
    
    float orthoWidth = orthoSize * aspectRatio;
    float orthoHeight = orthoSize;
    
    return glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -50.0f, 50.0f);
}

void InputManager::resetView() {
    calculateOptimalCameraPosition();
    resetModelTransform();
    _fov = 45.0f;
    _zoomLevel = 2.0f;
    std::cout << "View reset to default" << std::endl;
}