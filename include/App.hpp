/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   App.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:41 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/04 14:16:51 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APP_HPP
# define APP_HPP

# include "./Parser.hpp"
# include "./Renderer.hpp"
# include "./Mesh.hpp"
# include "./Shader.hpp"
# include "./InputManager.hpp"
# include "./ErrorManager.hpp"
# include <memory>
# include <iostream>
# include <sstream>
# include <fstream>
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
        
        bool _useOrthographic = false;
        bool _wireframeMode = false;

        bool _showVertices;
        void handleVertexToggle(bool showVertices);

    public:
        App(int mode, Mesh *mesh, Shader *shader, Renderer *renderer, Parser *parser);
        ~App();
        
        void run();

        void handleProjectionToggle(bool useOrthographic);
        void handleWireframeToggle(bool wireframeMode);

        glm::mat4 createProjectionMatrix();
    };

#endif