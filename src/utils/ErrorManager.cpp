/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorManager.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:29:40 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/04 10:20:38 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ErrorManager.hpp"

void ErrorManager::GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool ErrorManager::GLLogCall(const char *function, const char *file, int line) {
	while (GLenum error = glGetError()) {
		std::string errorName;

		switch (error) {
			case GL_INVALID_ENUM:      errorName = "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE:     errorName = "GL_INVALID_VALUE"; break;
			case GL_INVALID_OPERATION: errorName = "GL_INVALID_OPERATION"; break;
			case GL_OUT_OF_MEMORY:     errorName = "GL_OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: errorName = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
			default: errorName = "Unknown Error"; break;
		}

		std::cout << "[OpenGL Error] (" 
		          << static_cast<int>(error) << ")"
		          << "(0x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << error << std::dec << "): "
		          << errorName << std::endl
				  << "function [" << function << "] in file [" << file << "] at line:" << line << std::endl;

		return (false);
	}

	return (true);
}