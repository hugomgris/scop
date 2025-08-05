/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 18:37:46 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 12:09:12 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Texture.hpp"
#include <stdexcept>
#include <iostream>

Texture::Texture(const std::string &path): _filePath(path),
	_localBufer(nullptr), _width(0), _height(0), _BPP(0)
{
	//stbi_set_flip_vertically_on_load(1);
	_localBufer = stbi_load(path.c_str(), &_width, &_height, &_BPP, 4);
	
	if (!_localBufer) {
		std::cerr << "Failed to load texture: " << path << std::endl;
		std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
		throw std::runtime_error("Failed to load texture: " + path);
	}
	
	GLCall(glGenTextures(1, &_rendererId));
	GLCall(glBindTexture(GL_TEXTURE_2D, _rendererId));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)); // GL_CLAMP?
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _localBufer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if (_localBufer) {
		stbi_image_free(_localBufer);
	}
}

Texture::~Texture() {
	GLCall(glDeleteTextures(1, &_rendererId));
}

void Texture::Bind(unsigned int slot) const {
	GLCall(glActiveTexture(GL_TEXTURE0 + slot)); // GL_TEXTURE0 = first image slot (OpenGL = STATE MACHINE)
	GLCall(glBindTexture(GL_TEXTURE_2D, _rendererId));
}

void Texture::Unbind() const {
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

int Texture::getWidth() const {
	return _width;
}

int Texture::getHeigth() const {
	return _height;
}