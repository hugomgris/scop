/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:20 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/31 10:26:39 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../../include/Mesh.hpp"
#include "../../include/Shader.hpp"
#include <glad/glad.h>

class Renderer {
    public:
        Renderer();
        void draw(Mesh &mesh, int mode);
};
