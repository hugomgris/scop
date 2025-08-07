/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:41 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/07 16:41:11 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file App.hpp
 * @brief Declaration of the App class - main application controller for SCOP.
 *
 * The App class serves as the central orchestrator for the SCOP 3D viewer application,
 * managing the main rendering loop, user interface, input handling, and coordination
 * between all subsystems. It handles texture loading, material management, post-processing
 * effects, and provides various visualization modes for 3D models.
 */

#ifndef APP_HPP
# define APP_HPP

# include "./Parser.hpp"
# include "./Renderer.hpp"
# include "./Mesh.hpp"
# include "./Shader.hpp"
# include "./InputManager.hpp"
# include "./Texture.hpp"
# include "./TextureLoader.hpp"
# include "./UIManager.hpp"
# include "./PostProcessor.hpp"
# include "./ErrorManager.hpp"
# include "./Colors.hpp"
# include <glad/glad.h>
# include <memory>
# include <iostream>
# include <unordered_map>
# include <sstream>
# include <fstream>
# include <algorithm>
# include "glm/glm.hpp"
# include "glm/gtc/matrix_transform.hpp"
# include "glm/gtc/type_ptr.hpp"

class Mesh;
class Shader;
class Renderer;

/**
 * @class App
 * @brief Main application controller for the SCOP 3D viewer.
 *
 * The App class coordinates all aspects of the 3D viewer application:
 * - Main rendering loop and OpenGL context management
 * - User interface (ImGui) integration and event handling
 * - Input management (keyboard, mouse) through InputManager
 * - Texture loading and material management
 * - Post-processing effects (CRT filter)
 * - Multiple visualization modes (wireframe, solid, textured)
 * - Camera projection modes (perspective, orthographic)
 * - Multi-material rendering support
 *
 * The application follows a component-based architecture where the App class
 * serves as the central hub coordinating specialized subsystems.
 */
class App {
    private:
        int _mode;
        Parser *_parser;
        Mesh *_mesh;
        Shader *_shader;
        Renderer *_renderer;
        GLFWwindow *_window;

        std::unique_ptr<InputManager> _inputManager;
        std::unique_ptr<TextureLoader> _textureLoader;
        std::unique_ptr<UIManager> _uiManager;
        std::unique_ptr<PostProcessor> _postProcessor;

        std::shared_ptr<Texture> _currentTexture;
        std::unordered_map<int, std::shared_ptr<Texture>> _materialTextures;
        
        bool _useOrthographic = false;
        bool _wireframeMode = false;
        bool _showVertices = false;
        bool _enableCRT = false; 
        bool _useTexture = false;
        
        /**
         * @brief Set up UI callback functions for ImGui controls.
         * Connects UI elements to their corresponding handler methods.
         */
        void setupUICallbacks();

    public:
        App(int mode, Mesh *mesh, Shader *shader, Renderer *renderer, Parser *parser);

        ~App();
        
        /**
         * @brief Main application loop.
         * Handles rendering, input processing, and UI updates until window close.
         */
        void run();

        void setCurrentFile(const std::string& filename);
        void handleProjectionToggle(bool useOrthographic);
        void handleWireframeToggle(bool wireframeMode);
        void handleVertexToggle(bool showVertices);
        void handleAutoRotationToggle(bool autoRotation);
        void handleCRTToggle(bool enableCRT);
        void handleTextureToggle(bool useTexture);
        void renderWithMaterials();
        glm::mat4 createProjectionMatrix();
    };

#endif