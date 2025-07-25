#pragma once
#include "Mesh.hpp"
#include "Shader.hpp"

class Renderer {
public:
    Renderer();
    void draw(const Mesh& mesh, const Shader& shader);
};
