#include "Renderer.hpp"
#include <glad/glad.h>

Renderer::Renderer() {}

void Renderer::draw(const Mesh& mesh, const Shader& shader) {
    shader.use();
    mesh.bind();
    glDrawArrays(GL_TRIANGLES, 0, mesh.getVertexCount());
}
