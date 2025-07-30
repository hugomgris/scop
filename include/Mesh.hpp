/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:25 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/30 14:16:25 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class Mesh {
private:
    unsigned int VAO, VBO;
    int vertexCount;

public:
    Mesh();
    ~Mesh();
    void bind() const;
    int getVertexCount() const;
};