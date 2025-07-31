/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:08 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/31 16:25:18 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Renderer.hpp"
#include "glm/gtc/type_ptr.hpp"

Renderer::Renderer(Shader* shader) : _shader(shader) {}

void Renderer::setMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    _shader->use();
    
    // Get uniform locations and set matrices
    int modelLoc = glGetUniformLocation(_shader->getID(), "model");
    int viewLoc = glGetUniformLocation(_shader->getID(), "view");
    int projectionLoc = glGetUniformLocation(_shader->getID(), "projection");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Renderer::draw(Mesh &mesh, int mode) {
    int renderMode = mode == 0 ? GL_TRIANGLES : GL_LINES;

    glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear depth buffer too

    _shader->use();
    glBindVertexArray(mesh.getVAO());
    glDrawElements(renderMode, (GLsizei)mesh.getIndexCount(), GL_UNSIGNED_INT, nullptr);
}