/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:41 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 16:50:07 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
        std::shared_ptr<Texture> _currentTexture;
        std::unordered_map<int, std::shared_ptr<Texture>> _materialTextures;
        
        bool _useOrthographic = false;
        bool _wireframeMode = false;
        bool _showVertices = false;
        
        // UI callback handlers
        void setupUICallbacks();

    public:
        App(int mode, Mesh *mesh, Shader *shader, Renderer *renderer, Parser *parser);
        ~App();
        
        void run();
        void setCurrentFile(const std::string& filename);

        void handleProjectionToggle(bool useOrthographic);
        void handleWireframeToggle(bool wireframeMode);
        void handleVertexToggle(bool showVertices);
        void handleAutoRotationToggle(bool autoRotation);
        void renderWithMaterials();

        glm::mat4 createProjectionMatrix();
    };

#endif