/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:08 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/04 10:28:04 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Renderer.hpp"

Renderer::Renderer(Shader* shader) : _shader(shader) {}

void Renderer::setMatrices(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
    _shader->use();
    
    int modelLoc = glGetUniformLocation(_shader->getID(), "model");
    int viewLoc = glGetUniformLocation(_shader->getID(), "view");
    int projectionLoc = glGetUniformLocation(_shader->getID(), "projection");
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void Renderer::draw(Mesh &mesh, int mode) {
    int renderMode = mode == 0 ? GL_TRIANGLES : GL_LINES;

    setClearColor(Colors::BLACK_CHARCOAL_1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader->use();
    
    int lineColorLoc = glGetUniformLocation(_shader->getID(), "lineColor");
    int isLineModeLoc = glGetUniformLocation(_shader->getID(), "isLineMode");
    
    if (renderMode == GL_LINES) {
        setLineColor(lineColorLoc, Colors::OFF_WHITE);
        glLineWidth(2.0f);
        glUniform1i(isLineModeLoc, 1);
    } else {
        glUniform1i(isLineModeLoc, 0);
    }
    
    glBindVertexArray(mesh.getVAO());
    GLCall(glDrawElements(renderMode, (GLsizei)mesh.getIndexCount(), GL_UNSIGNED_INT, nullptr));
}