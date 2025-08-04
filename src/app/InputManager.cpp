/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 13:50:59 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/04 14:23:21 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/InputManager.hpp"

InputManager::InputManager(GLFWwindow *window, int mode): _window(window), _mode(mode),
	_cameraPos(0.0f, 0.0f, 5.0f), _cameraFront(0.0f, 0.0f, -1.0f), _cameraUp(0.0f, 1.0f, 0.0f),
	_lastX(960.0f), _lastY(540.0f), _yaw(-90.0f), _pitch(0.0f), _firstMouse(true),
	_deltaTime(0.0f), _lastFrame(0.0f) {
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback_wrapper);
	glfwSetScrollCallback(window, scroll_callback_wrapper);
	glfwSetKeyCallback(window, key_callback_wrapper);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    updateCameraVectors();
}

InputManager::~InputManager() {}

float InputManager::getDeltaTime() const {
	return _deltaTime;
}

float InputManager::getLastFrame() const {
	return _lastFrame;
}

glm::vec3 InputManager::getCameraPosition() const {
    return _cameraPos;
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

void InputManager::setProjectionToggleCallback(std::function<void(bool)> callback) {
    _onProjectionToggle = callback;
}

void InputManager::setWireframeToggleCallback(std::function<void(bool)> callback) {
    _onWireframeToggle = callback;
}

void InputManager::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	(void)window;
	glViewport(0, 0, width, height);
}

void InputManager::mouse_callback_wrapper(GLFWwindow* window, double xpos, double ypos) {
    InputManager *inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    inputManager->mouse_callback(window, xpos, ypos);
}

void InputManager::scroll_callback_wrapper(GLFWwindow* window, double xoffset, double yoffset) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    inputManager->scroll_callback(window, xoffset, yoffset);
}

void InputManager::key_callback_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputManager* inputManager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    inputManager->key_callback(window, key, scancode, action, mods);
}

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    
    if (_firstMouse) {
        _lastX = xpos;
        _lastY = ypos;
        _firstMouse = false;
    }

    float xoffset = xpos - _lastX;
    float yoffset = _lastY - ypos;
    _lastX = xpos;
    _lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        float sensitivity = 0.5f;
        _modelRotationX += yoffset * sensitivity;
        _modelRotationY += xoffset * sensitivity;
        
        if (_modelRotationX > 89.0f) _modelRotationX = 89.0f;
        if (_modelRotationX < -89.0f) _modelRotationX = -89.0f;
    } else {
        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        _yaw += xoffset;
        _pitch += yoffset;

        if (_pitch > 89.0f) _pitch = 89.0f;
        if (_pitch < -89.0f) _pitch = -89.0f;

        updateCameraVectors();
    }
}

void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;
    
    _fov -= (float)yoffset;
    if (_fov < 1.0f) _fov = 1.0f;
    if (_fov > 45.0f) _fov = 45.0f;
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

        resetView();
        if (_useOrthographic) {
            resetView();
            _modelRotationX = 35.265f;
            _modelRotationY = 45.0f;
        }

        
        if (_onProjectionToggle) {
            _onProjectionToggle(_useOrthographic);
        }
    }

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        _autoRotation = !_autoRotation;
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

void InputManager::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _cameraFront = glm::normalize(front);

    glm::vec3 right = glm::normalize(glm::cross(_cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    
    glm::vec3 up = glm::normalize(glm::cross(right, _cameraFront));
    
    glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(_roll), _cameraFront);
    _cameraUp = glm::vec3(rollMatrix * glm::vec4(up, 0.0f));
}

void InputManager::processInput() {
    float cameraSpeed = _movementSpeed * _deltaTime;
    float cameraRotation = _rotationSpeed * _deltaTime;
    
    // Camera movement
    if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
        _cameraPos += cameraSpeed * _cameraFront;
    if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
        _cameraPos -= cameraSpeed * _cameraFront;
    if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
        _cameraPos -= glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
    if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
        _cameraPos += glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
    if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS)
        _cameraPos += cameraSpeed * _cameraUp;
    if (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        _cameraPos -= cameraSpeed * _cameraUp;
    
    if (glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS) {
        _roll -= cameraRotation;
        updateCameraVectors();
    }
    if (glfwGetKey(_window, GLFW_KEY_E) == GLFW_PRESS) {
        _roll += cameraRotation;
        updateCameraVectors();
    }
}

void InputManager::createMatrices() {
    if (_autoRotation) {
        _modelRotationY += _autoRotationSpeed * _deltaTime;
    } 
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(_modelRotationX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(_modelRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    
    glm::mat4 view = glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);

    glm::mat4 projection;
    if (_useOrthographic) {
        if (_mode == FDF) {
            projection = createOrthographicProjectionForFDF(1920.0f / 1080.0f, 110, 190, 10.f);
        } else {
            projection = createOrthographicProjection(1920.0f / 1080.0f, 1.0f);
        }
    } else {
        projection = glm::perspective(glm::radians(_fov), 1920.0f / 1080.0f, 0.1f, 1000.0f);
    }

    _model = model;
    _view = view;
    _projection = projection;
}

glm::mat4 InputManager::createOrthographicProjection(float aspectRatio, float zoom) {
    float orthoSize = 10.0f / zoom;
    
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

    // Use a zoom factor to control how close/far the view is
    float zoomFactor = 0.5f;  // Smaller = closer view, larger = farther view
    
    float maxDimension = std::max(mapWidth, mapHeight);
    float orthoSize = (maxDimension * zoomFactor) / 2.0f;
    
    float orthoWidth = orthoSize * aspectRatio;
    float orthoHeight = orthoSize;
    
    return glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -50.0f, 50.0f);
}

void InputManager::updateCameraFront() {
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _cameraFront = glm::normalize(front);
}

void InputManager::resetView() {
    _cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
    _cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    _cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    _yaw = -90.0f;
    _pitch = 0.0f;
    _roll = 0.0f;
    _modelRotationX = 0.0f;
    _modelRotationY = 0.0f;
    _fov = 45.0f;
    updateCameraVectors();
    std::cout << "View reset to default" << std::endl;
}