/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:08 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 10:55:37 by hmunoz-g         ###   ########.fr       */
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

void Renderer::draw(Mesh &mesh, int mode, const glm::vec3 &cameraPos, bool showVertices) {
    int renderMode = mode == 0 ? GL_TRIANGLES : GL_LINES;

    setClearColor(Colors::BLACK_CHARCOAL_1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    _shader->use();

    int lineColorLoc = glGetUniformLocation(_shader->getID(), "u_lineColor");
    int isLineModeLoc = glGetUniformLocation(_shader->getID(), "u_isLineMode");
    int isVertexModeLoc = glGetUniformLocation(_shader->getID(), "u_isVertexMode");
    
    if (renderMode == GL_LINES) {
        setLineColor(lineColorLoc, Colors::OFF_WHITE);
        glLineWidth(2.0f);
        glUniform1i(isLineModeLoc, 1);
        glUniform1i(isVertexModeLoc, 0);
    } else {
        glUniform1i(isLineModeLoc, 0);
        glUniform1i(isVertexModeLoc, 0);
        
        int colorLoc = glGetUniformLocation(_shader->getID(), "u_color");
        int lightPosLoc = glGetUniformLocation(_shader->getID(), "u_lightPos");
        int lightColorLoc = glGetUniformLocation(_shader->getID(), "u_lightColor");
        int viewPosLoc = glGetUniformLocation(_shader->getID(), "u_viewPos");
        
        // Add texture uniforms here
        int textureLoc = glGetUniformLocation(_shader->getID(), "u_texture");
        int useTextureLoc = glGetUniformLocation(_shader->getID(), "useTexture");
        
        glUniform3f(colorLoc, 0.5, 0.5, 0.9);
        
        glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
        
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
        
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
        
        // Set texture uniforms
        glUniform1i(textureLoc, 0);  // Use texture unit 0
        glUniform1i(useTextureLoc, 1);  // Enable texture usage (0 = no texture, toggleable)
    }
    
    GLCall(glBindVertexArray(mesh.getVAO()));
    GLCall(glDrawElements(renderMode, (GLsizei)mesh.getIndexCount(), GL_UNSIGNED_INT, nullptr));
    
    if (showVertices) {
        glUniform1i(isVertexModeLoc, 1);
        glUniform1i(isLineModeLoc, 0);
        
        int vertexColorLoc = glGetUniformLocation(_shader->getID(), "u_vertexColor");
        glUniform3f(vertexColorLoc, 1.0f, 1.0f, 0.0f);
        
        glEnable(GL_PROGRAM_POINT_SIZE);
        glPointSize(10.0f);
        
        GLCall(glDrawArrays(GL_POINTS, 0, mesh.getVertexCount()));
        
        glDisable(GL_PROGRAM_POINT_SIZE);
    }
}