#include "Mesh.hpp"
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