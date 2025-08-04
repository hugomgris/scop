/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:51 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/04 10:56:02 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Shader.hpp"

Shader::Shader(std::string shaderpath) {
    _shaderSource = parseShader(shaderpath);
}

Shader::~Shader() {
    glDeleteProgram(_id);
}

unsigned int Shader::getID() const {
    return _id;
}

void Shader::use() const {
    glUseProgram(_id);
}

void Shader::compile() {
    _id = createShader(_shaderSource.vertexSource, _shaderSource.fragmentSource);
    use();
}

ShaderProgramSource Shader::parseShader(const std::string &filepath) {
	std::ifstream stream(filepath);
	if (!stream.is_open()) {
		std::cerr << "Failed to open shader file: " << filepath << "\n";
		return {};
	}

	enum class shaderType {
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	shaderType currentShaderType = shaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				currentShaderType = shaderType::VERTEX;
			} else if (line.find("fragment") != std::string::npos) {
				currentShaderType = shaderType::FRAGMENT;
			}
		} else if (currentShaderType != shaderType::NONE) {
			ss[static_cast<int>(currentShaderType)] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::compileShader(unsigned int type, const std::string &source) {
	unsigned int id = glCreateShader(type);
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	//! Todo: expanded error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char *)(alloca(length * sizeof(char)));
		glGetShaderInfoLog(id, length, &length, message);
		std::cerr << "Failed to compile "
			<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader: "
			<< message << "\n";
		glDeleteShader(id);
		return 0;
	}

	return (id);
}

unsigned int Shader::createShader(const std::string &vertexShader, const std::string &fragmentShader) {
	unsigned int program = glCreateProgram();
	_vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	_fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, _vs);
	glAttachShader(program, _fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(_vs);
	glDeleteShader(_fs);

	return (program);
}
