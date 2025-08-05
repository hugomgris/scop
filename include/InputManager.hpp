/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputManager.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 13:47:39 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 16:50:07 by hmunoz-g         ###   ########.fr       */
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

// Mouse interaction modes for model manipulation
enum class MouseInteraction {
    None,
    PanModel,      // Left click + drag to move model
    RotateModel    // Right click + drag to rotate model
};

class InputManager {
	private:
		GLFWwindow *_window;
		int _mode;
		const BoundingBox &_boundingBox;
	
		// Fixed camera system for optimal model viewing
		glm::vec3 _fixedCameraPos;
		glm::vec3 _fixedCameraTarget{0.0f};
		glm::vec3 _fixedCameraUp{0.0f, 1.0f, 0.0f};
		float _defaultDistance;
		
		// Model manipulation system
		glm::vec3 _modelOffset{0.0f};     // Model translation
		glm::vec3 _modelRotation{0.0f};   // Model rotation (pitch, yaw, roll)
		float _modelScale = 1.0f;         // Model scale factor
		
		// Mouse interaction state
		MouseInteraction _mouseInteraction = MouseInteraction::None;
		double _lastMouseX = 0.0;
		double _lastMouseY = 0.0;
		
		// Viewport bounds for interaction detection
		glm::vec4 _viewportBounds{0.0f}; // x, y, width, height
		
		// Model auto-rotation
		bool _autoRotation = false;
		float _autoRotationSpeed = 45.0f;

		//Matrices
		glm::mat4 _model;
		glm::mat4 _view;
		glm::mat4 _projection;
		
		// Timing
		float _deltaTime;
		float _lastFrame;
		
		// Field of view
		float _fov = 45.0f;
		float _zoomLevel = 1.0f;

		std::function<void(bool)> _onProjectionToggle;
		std::function<void(bool)> _onWireframeToggle;
		std::function<void(bool)> _onVertexToggle;
		std::function<void(bool)> _onAutoRotationToggle;
		
		bool _useOrthographic = false;
		bool _wireframeMode = false;
		bool _showVertices = false;
		
		// Dynamic aspect ratio for UI rendering
		float _aspectRatio = 1920.0f / 1080.0f;
	
	public:
		InputManager(GLFWwindow *window, int mode, float optimalDistance, const BoundingBox &boundingBox);
		~InputManager();

		float getDeltaTime() const;
		float getLastFrame() const;
		glm::vec3 getCameraPosition() const;
		bool getAutorotationStatus() const;
		std::vector<glm::mat4> getMatrices() const;
		
		void setDeltaTime(float currentFrame);
		void setAspectRatio(float aspectRatio);
		void setViewportBounds(const glm::vec4& bounds);
		void setUseOrthographic(bool useOrtho);
		void setAutoRotation(bool autoRotation);

		void processInput();
		void createMatrices();
		glm::mat4 createOrthographicProjection(float aspectRatio, float zoom = 1.0f);
		glm::mat4 createOrthographicProjectionForFDF(float aspectRatio, int mapRows, int mapCols, float spacing);
		void calculateOptimalCameraPosition();
		void resetModelTransform();
		void resetView();

		// Static callbacks
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void mouse_button_callback_wrapper(GLFWwindow* window, int button, int action, int mods);
		static void mouse_callback_wrapper(GLFWwindow* window, double xpos, double ypos);
		static void scroll_callback_wrapper(GLFWwindow* window, double xoffset, double yoffset);
		static void key_callback_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
		
		// Instance methods for callbacks
		void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		void mouse_callback(GLFWwindow* window, double xpos, double ypos);
		void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

		// Event callback setters
		void setProjectionToggleCallback(std::function<void(bool)> callback);
		void setWireframeToggleCallback(std::function<void(bool)> callback);
		void setVertexToggleCallback(std::function<void(bool)> callback);
		void setAutoRotationToggleCallback(std::function<void(bool)> callback);
		
		// Getters for current state
		bool isUsingOrthographic() const { return _useOrthographic; }
		bool isWireframeMode() const { return _wireframeMode; }
};

#endif