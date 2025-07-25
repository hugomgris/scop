#pragma once

class Mesh {
private:
    unsigned int VAO, VBO;
    int vertexCount;

public:
    Mesh();
    ~Mesh();
    void bind() const;
    int getVertexCount() const;
};