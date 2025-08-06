/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:05 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 15:18:29 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Mesh.hpp"
#include <glad/glad.h>
#include <vector>

Mesh::Mesh(Parser *parser): _parser(parser) {
	_vertexCount = parser->getVertices().size();
	_indexCount = parser->getIndices().size();
	_wireframeIndexCount = 0;
	generateWireframeIndices();
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &_IBO);
	glDeleteBuffers(1, &_wireframeIBO);
}

int Mesh::getVertexCount() const {
	return _vertexCount;
}

int Mesh::getIndexCount() const {
	return _indexCount;
}

int Mesh::getWireframeIndexCount() const {
	return _wireframeIndexCount;
}

unsigned int Mesh::getVAO() const {
	return _VAO;
}

unsigned int Mesh::getBVO() const {
	return _VBO;
}

unsigned int Mesh::getIBO() const {
	return _IBO;
}

unsigned int Mesh::getWireframeIBO() const {
	return _wireframeIBO;
}

void Mesh::bind(){
    GLCall(glGenVertexArrays(1, &_VAO));
    GLCall(glBindVertexArray(_VAO));

    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _parser->getVertices().size(), _parser->getVertices().data(), GL_STATIC_DRAW);

    glGenBuffers(1, &_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _parser->getIndices().size(), _parser->getIndices().data(), GL_STATIC_DRAW);

    // Create wireframe IBO
    glGenBuffers(1, &_wireframeIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _wireframeIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _wireframeIndices.size(), _wireframeIndices.data(), GL_STATIC_DRAW);

    // Restore original IBO binding
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}

void Mesh::generateWireframeIndices() {
    const std::vector<unsigned int>& triangleIndices = _parser->getIndices();
    _wireframeIndices.clear();
    
    for (size_t i = 0; i < triangleIndices.size(); i += 3) {
        if (i + 2 < triangleIndices.size()) {
            unsigned int v0 = triangleIndices[i];
            unsigned int v1 = triangleIndices[i + 1];
            unsigned int v2 = triangleIndices[i + 2];
            
            _wireframeIndices.push_back(v0);
            _wireframeIndices.push_back(v1);
            
            _wireframeIndices.push_back(v1);
            _wireframeIndices.push_back(v2);
            
            _wireframeIndices.push_back(v2);
            _wireframeIndices.push_back(v0);
        }
    }
    
    _wireframeIndexCount = _wireframeIndices.size();
}