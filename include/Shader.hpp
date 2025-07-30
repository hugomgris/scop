/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:18 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/30 18:49:02 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

struct ShaderProgramSource {
	std::string vertexSource;
	std::string fragmentSource;
};

class Shader {
    public:
        Shader(std::string shaderpath);
        ~Shader();
        void use() const;

        unsigned int getID() const;

        void compile();
        
    private:
        unsigned int _vs;
        unsigned int _fs;
        unsigned int _id;
        ShaderProgramSource _shaderSource;

        unsigned int createShader(const std::string &vertexShader, const std::string &fragmentShader);
        unsigned int compileShader(unsigned int type, const std::string &source);
        ShaderProgramSource parseShader(const std::string &filepath);
};
