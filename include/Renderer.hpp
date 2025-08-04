/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:08 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/04 09:43:43 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDERER_HPP
# define RENDERER_HPP

# include <glad/glad.h>
# include "./ErrorManager.hpp"
# include "./Colors.hpp"
# include "glm/glm.hpp"
# include "Mesh.hpp"
# include "Shader.hpp"
# include "glm/gtc/type_ptr.hpp"

class Renderer {
private:
    Shader* _shader;

public:
    Renderer(Shader* shader);
    
    void setMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);
    void draw(Mesh &mesh, int mode);
};

#endif