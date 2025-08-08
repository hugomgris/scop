/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:08 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/08 09:39:38 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file Renderer.hpp
 * @brief Declaration of the Renderer class for OpenGL 3D rendering operations.
 *
 * The Renderer class encapsulates all OpenGL rendering functionality for the SCOP project,
 * handling mesh rendering with support for multiple visualization modes including wireframe,
 * solid, and textured rendering. It manages shader uniforms, transformation matrices,
 * and provides both single-material and multi-material rendering capabilities.
 */

#ifndef RENDERER_HPP
# define RENDERER_HPP

# include <glad/glad.h>
# include "./ErrorManager.hpp"
# include "./Colors.hpp"
# include "glm/glm.hpp"
# include "Mesh.hpp"
# include "Shader.hpp"
# include "ErrorManager.hpp"
# include "glm/gtc/type_ptr.hpp"

/**
 * @class Renderer
 * @brief Handles OpenGL rendering operations for 3D meshes.
 *
 * The Renderer class provides a high-level interface for rendering 3D geometry using OpenGL.
 * Key features include:
 * - Multiple rendering modes (wireframe, solid, textured)
 * - Transformation matrix management (Model-View-Projection)
 * - Texture mode toggle support
 * - Multi-material rendering for complex models
 * - Vertex visualization for debugging
 * - Shader uniform management
 *
 * The renderer works closely with the Mesh and Shader classes to provide efficient
 * GPU-accelerated rendering with modern OpenGL practices.
 */
class Renderer {
    private:
        Shader* _shader;

    public:
        Renderer(Shader *shader);
        
        void setMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

        void draw(Mesh &mesh, int mode, const glm::vec3 &cameraPos, bool showVertices, bool wireframeMode, bool useTexture);
        void drawMaterialGroup(Mesh &mesh, const std::vector<unsigned int>& indices, int mode, const glm::vec3 &cameraPos, bool showVertices = false);
};

#endif