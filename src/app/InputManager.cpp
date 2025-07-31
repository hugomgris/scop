/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 13:50:59 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/31 18:14:37 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/InputManager.hpp"

InputManager::InputManager(GLFWwindow *window): _window(window),
	_cameraPos(0.0f, 0.0f, 5.0f), _cameraFront(0.0f, 0.0f, -1.0f), _cameraUp(0.0f, 1.0f, 0.0f),
	_lastX(960.0f), _lastY(540.0f), _yaw(-90.0f), _pitch(0.0f), _firstMouse(true),
	_deltaTime(0.0f), _lastFrame(0.0f) {
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback_wrapper);
	glfwSetScrollCallback(window, scroll_callback_wrapper);
	glfwSetKeyCallback(window, key_callback_wrapper);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

InputManager::~InputManager() {}

float InputManager::getDeltaTime() const {
	return _deltaTime;
}

float InputManager::getLastFrame() const {
	return _lastFrame;
}

std::vector<glm::mat4> InputManager::getMatrices() const {
	std::vector<glm::mat4> matrices;
	matrices.push_back(_model);
	matrices.push_back(_view);
	matrices.push_back(_projection);
	return (matrices);
}

void InputManager::setDeltaTime(float currentFrame) {
	_deltaTime = currentFrame - _lastFrame;
	_lastFrame = currentFrame;
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

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    _yaw += xoffset;
    _pitch += yoffset;

    if (_pitch > 89.0f) _pitch = 89.0f;
    if (_pitch < -89.0f) _pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _cameraFront = glm::normalize(front);
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
    
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        static bool wireframe = true;
        wireframe = !wireframe;
        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		bool useOrthographic = true;

		glm::mat4 projection;
		float aspectRatio = (float)1920 / (float)1080;
		if (useOrthographic) {
			projection = createOrthographicProjection(aspectRatio);
		} else {
			projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
		}

		/* setMatrices(_model, _view, projection);// THIS FUNCTION IS IN RENDERER SO WE'LL SEE HOW TO REACH IT */
	}
}

void InputManager::processInput() {
    float cameraSpeed = 10.f * _deltaTime;
    
    if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
        _cameraPos += cameraSpeed * _cameraFront;
    if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
        _cameraPos -= cameraSpeed * _cameraFront;
    if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
        _cameraPos -= glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
    if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
        _cameraPos += glm::normalize(glm::cross(_cameraFront, _cameraUp)) * cameraSpeed;
}

void InputManager::createMatrices() {
	glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
    //glm::mat4 projection = glm::ortho(-10.f/2.0f, 10.f/2.0f, -10.f/2.0f, 10.f/2.0f, 0.1f, 100.0f);
	glm::mat4 projection = glm::perspective(glm::radians(_fov), 1920.0f / 1080.0f, 0.1f, 1000.0f);

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
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    
    return glm::ortho(left, right, bottom, top, nearPlane, farPlane);
}

glm::mat4 InputManager::createOrthographicProjectionForFDF(float aspectRatio, int mapRows, int mapCols, float spacing) {
    float mapWidth = mapCols * spacing;
    float mapHeight = mapRows * spacing;

    float orthoWidth = std::max(mapWidth, mapHeight) * 0.6f;
    float orthoHeight = orthoWidth / aspectRatio;
    
    return glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, 0.1f, 100.0f);
}