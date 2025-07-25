#pragma once
#include <string>

class Shader {
public:
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~Shader();
    void use() const;

	unsigned int getID() const;
private:
    unsigned int id;
    void compile(const std::string& vertexSrc, const std::string& fragmentSrc);
};
