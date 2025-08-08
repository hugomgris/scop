/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:08 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/08 09:43:24 by hmunoz-g         ###   ########.fr       */
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

/**
 * Draw Mesh - Renders 3D mesh with various rendering modes and effects
 * 
 * FLOW:
 * 1. Activate shader program for rendering
 * 2. Get uniform locations for rendering mode control
 * 3. Handle wireframe rendering mode:
 *    - Set line color and enable line mode flags
 *    - Bind wireframe index buffer
 *    - Draw using GL_LINES primitive
 * 4. Handle standard rendering modes:
 *    - Configure line vs triangle rendering
 *    - Set up lighting uniforms (light position, color, camera position)
 *    - Configure texture usage flag and bind texture sampler
 *    - Draw using appropriate primitive type (triangles/lines)
 * 5. Handle vertex visualization overlay:
 *    - Enable vertex mode and set vertex color
 *    - Configure point rendering with size
 *    - Draw vertices as GL_POINTS
 */
void Renderer::draw(Mesh &mesh, int mode, const glm::vec3 &cameraPos, bool showVertices, bool wireframeMode, bool useTexture) {
    _shader->use();

    int lineColorLoc = glGetUniformLocation(_shader->getID(), "u_lineColor");
    int isLineModeLoc = glGetUniformLocation(_shader->getID(), "u_isLineMode");
    int isVertexModeLoc = glGetUniformLocation(_shader->getID(), "u_isVertexMode");
    
    if (wireframeMode) {
        setLineColor(lineColorLoc, Colors::OFF_WHITE);
        glLineWidth(1.0f);
        glUniform1i(isLineModeLoc, 1);
        glUniform1i(isVertexModeLoc, 0);
        
        GLCall(glBindVertexArray(mesh.getVAO()));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getWireframeIBO());
        GLCall(glDrawElements(GL_LINES, (GLsizei)mesh.getWireframeIndexCount(), GL_UNSIGNED_INT, nullptr));
        
    } else {
        int renderMode = mode == 0 ? GL_TRIANGLES : GL_LINES;
        
        if (renderMode == GL_LINES) {
            setLineColor(lineColorLoc, Colors::OFF_WHITE);
            glLineWidth(1.0f);
            glUniform1i(isLineModeLoc, 1);
            glUniform1i(isVertexModeLoc, 0);
        } else {
            glUniform1i(isLineModeLoc, 0);
            glUniform1i(isVertexModeLoc, 0);
            
            int colorLoc = glGetUniformLocation(_shader->getID(), "u_color");
            int lightPosLoc = glGetUniformLocation(_shader->getID(), "u_lightPos");
            int lightColorLoc = glGetUniformLocation(_shader->getID(), "u_lightColor");
            int viewPosLoc = glGetUniformLocation(_shader->getID(), "u_viewPos");
            
            int textureLoc = glGetUniformLocation(_shader->getID(), "u_texture");
            int useTextureLoc = glGetUniformLocation(_shader->getID(), "useTexture");
            
            glUniform3f(colorLoc, 0.5, 0.5, 0.9);
            
            glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
            glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
            
            glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
            
            glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
            
            glUniform1i(textureLoc, 0);
            glUniform1i(useTextureLoc, useTexture ? 1 : 0);
        }
        
        GLCall(glBindVertexArray(mesh.getVAO()));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getIBO());
        GLCall(glDrawElements(renderMode, (GLsizei)mesh.getIndexCount(), GL_UNSIGNED_INT, nullptr));
    }
    
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