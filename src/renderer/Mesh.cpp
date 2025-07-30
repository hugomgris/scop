/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:05 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/30 14:16:06 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Mesh.hpp"
#include <glad/glad.h>

Mesh::Mesh() {
	
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void Mesh::bind() const {
	glBindVertexArray(VAO);
}

int Mesh::getVertexCount() const {
	return vertexCount;
}