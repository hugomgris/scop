#include "Shader.hpp"
#include <glad/glad.h>
#include <iostream>

Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
    compile(vertexSrc, fragmentSrc);
}

Shader::~Shader() {
    glDeleteProgram(id);
}

unsigned int Shader::getID() const {
    return id;
}

void Shader::use() const {
    glUseProgram(id);
}

void Shader::compile(const std::string& vertexSrc, const std::string& fragmentSrc) {
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* vSrc = vertexSrc.c_str();
    glShaderSource(vertex, 1, &vSrc, nullptr);
    glCompileShader(vertex);
    // Error checking omitted for brevity

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fSrc = fragmentSrc.c_str();
    glShaderSource(fragment, 1, &fSrc, nullptr);
    glCompileShader(fragment);
    // Error checking omitted for brevity

    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    // Error checking omitted for brevity

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
