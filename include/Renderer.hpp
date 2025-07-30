/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:20 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/30 17:57:58 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Mesh.hpp"
#include "Shader.hpp"

class Renderer {
public:
    Renderer();
    void draw(Mesh &mesh, Shader &shader);
};
