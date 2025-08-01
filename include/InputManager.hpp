/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputManager.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 13:47:39 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/01 12:09:14 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INPUTMANAGER_HPP
# define INPUTMANAGER_HPP

# include <vector>
# include <functional>
# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class InputManager {
	private:
		GLFWwindow *_window;
	
		// Camera system
		glm::vec3 _cameraPos;
		glm::vec3 _cameraFront;
		glm::vec3 _cameraUp;

		//Matrices
		glm::mat4 _model;
		glm::mat4 _view;
		glm::mat4 _projection;
		
		// Mouse input
		float _lastX, _lastY;
		float _yaw, _pitch;
		bool _firstMouse;
		
		// Timing
		float _deltaTime;
		float _lastFrame;
		
		// Field of view
		float _fov = 45.0f;

		std::function<void(bool)> _onProjectionToggle;
		std::function<void(bool)> _onWireframeToggle;
		
		bool _useOrthographic = false;
		bool _wireframeMode = true;
	
	public:
		InputManager(GLFWwindow *window);
		~InputManager();

		float getDeltaTime() const;
		float getLastFrame() const;
		std::vector<glm::mat4> getMatrices() const;
		
		void setDeltaTime(float currentFrame);

		void processInput();
		void createMatrices();
		glm::mat4 createOrthographicProjection(float aspectRatio, float zoom = 1.0f);
		glm::mat4 createOrthographicProjectionForFDF(float aspectRatio, int mapRows, int mapCols, float spacing);

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
};

#endif