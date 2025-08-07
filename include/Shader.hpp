/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Shader.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:18 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/07 16:49:45 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SHADER_HPP
# define SHADER_HPP
# include <string>
# include <glad/glad.h>
# include <iostream>
# include <fstream>
# include <sstream>
# include "ErrorManager.hpp"
# include "Types.hpp"

class Shader {
    private:
        unsigned int _vs;
        unsigned int _fs;
        unsigned int _id;
        ShaderProgramSource _shaderSource;

        unsigned int createShader(const std::string &vertexShader, const std::string &fragmentShader);
        unsigned int compileShader(unsigned int type, const std::string &source);
        ShaderProgramSource parseShader(const std::string &filepath);
    
    public:
        Shader(std::string shaderpath);
        ~Shader();
        void use() const;

        unsigned int getID() const;

        void compile();

        void setUniform(const std::string& name, int value);
        void setUniform(const std::string& name, float value);
        void setUniform(const std::string& name, const glm::vec2& vector);
        void setUniform(const std::string& name, const glm::vec3& vector);
        void setUniform(const std::string& name, const glm::mat4& matrix);
};

#endif
