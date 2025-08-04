/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:05 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/04 11:17:55 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Mesh.hpp"
#include <glad/glad.h>

Mesh::Mesh(Parser *parser): _parser(parser) {
	_vertexCount = parser->getVertices().size();
	_indexCount = parser->getIndices().size();
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);
}

int Mesh::getVertexCount() const {
	return _vertexCount;
}

int Mesh::getIndexCount() const {
	return _indexCount;
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

void Mesh::bind(){
    GLCall(glGenVertexArrays(1, &_VAO));
    GLCall(glBindVertexArray(_VAO));

    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _parser->getVertices().size(), _parser->getVertices().data(), GL_STATIC_DRAW);

    glGenBuffers(1, &_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _parser->getIndices().size(), _parser->getIndices().data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
}