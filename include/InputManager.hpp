/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputManager.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 13:47:39 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/04 17:12:43 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INPUTMANAGER_HPP
# define INPUTMANAGER_HPP

# include <iostream>
# include <vector>
# include <functional>
# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include "./Types.hpp"
# include "./Parser.hpp"
# include "glm/glm.hpp"
# include "glm/gtc/matrix_transform.hpp"
# include "glm/gtc/type_ptr.hpp"

class InputManager {
	private:
		GLFWwindow *_window;
		int _mode;
		const BoundingBox &_boundingBox;
	
		// Camera system
		glm::vec3 _cameraPos;
		glm::vec3 _cameraFront;
		glm::vec3 _cameraUp;
		float _defaultDistance;
		float _zoomLevel = 1.0f;

		float _movementSpeed = 10.f;
		float _rotationSpeed = 90.0f;

		float _modelRotationX = 0.0f;
    	float _modelRotationY = 0.0f;

		bool _autoRotation = false;
		float _autoRotationSpeed = 45.0f;

		//Matrices
		glm::mat4 _model;
		glm::mat4 _view;
		glm::mat4 _projection;
		
		// Mouse input
		float _lastX, _lastY;
		float _yaw, _pitch, _roll = 0.f;
		bool _firstMouse;
		
		// Timing
		float _deltaTime;
		float _lastFrame;
		
		// Field of view
		float _fov = 45.0f;

		std::function<void(bool)> _onProjectionToggle;
		std::function<void(bool)> _onWireframeToggle;
		std::function<void(bool)> _onVertexToggle;
		
		bool _useOrthographic = false;
		bool _wireframeMode = false;
		bool _showVertices = false;
	
	public:
		InputManager(GLFWwindow *window, int mode, float optimalDistance, const BoundingBox &boundingBox);
		~InputManager();

		float getDeltaTime() const;
		float getLastFrame() const;
		glm::vec3 getCameraPosition() const;
		bool getAutorotationStatus() const;
		std::vector<glm::mat4> getMatrices() const;
		
		void setDeltaTime(float currentFrame);

		void processInput();
		void createMatrices();
		glm::mat4 createOrthographicProjection(float aspectRatio, float zoom = 1.0f);
		glm::mat4 createOrthographicProjectionForFDF(float aspectRatio, int mapRows, int mapCols, float spacing);
		void updateCameraFront();
		void updateCameraVectors();
		void autoRotate();
		void resetView();

		// Static callbacks
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void mouse_callback_wrapper(GLFWwindow* window, double xpos, double ypos);
		static void scroll_callback_wrapper(GLFWwindow* window, double xoffset, double yoffset);
		static void key_callback_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
		
		// Instance methods for callbacks
		void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

		// Event callback setters
		void setProjectionToggleCallback(std::function<void(bool)> callback);
		void setWireframeToggleCallback(std::function<void(bool)> callback);
		
		// Getters for current state
		bool isUsingOrthographic() const { return _useOrthographic; }
		bool isWireframeMode() const { return _wireframeMode; }
		void setVertexToggleCallback(std::function<void(bool)> callback);
};

#endif