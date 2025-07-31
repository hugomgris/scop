/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:08 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/31 10:28:35 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Renderer.hpp"

Renderer::Renderer() {}

void Renderer::draw(Mesh &mesh, int mode) {
    int renderMode = mode == 0 ? GL_TRIANGLES : GL_LINES;

    glClearColor(0.2f, 0.1f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(mesh.getVAO());
    glDrawElements(renderMode, (GLsizei)mesh.getIndexCount(), GL_UNSIGNED_INT, nullptr);
}
