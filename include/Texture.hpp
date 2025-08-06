/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 18:38:10 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 09:49:05 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma #once

#ifndef TEXTURE_HPP
# define TEXTURE_HPP

# include <iostream>
# include "./ErrorManager.hpp"
# include "./stb_image/stb_image.h"
# include "glm/glm.hpp"
# include "glm/gtc/matrix_transform.hpp"
# include "glm/gtc/type_ptr.hpp"
# include <glad/glad.h>

class Texture {
	private:
		unsigned int _rendererId;
		std::string _filePath;
		unsigned char *_localBufer;
		int _width, _height, _BPP;

	public:
		Texture(const std::string &path);
		~Texture();

		void Bind(unsigned int slot = 0) const;
		void Unbind() const;

		int getWidth() const;
		int getHeigth() const;
};

#endif