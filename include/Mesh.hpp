/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:25 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 15:07:14 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef MESH_HPP
# define MESH_HPP

#include "./ErrorManager.hpp"
#include "./Parser.hpp"

class Mesh {
    private:
        unsigned int _VAO, _VBO, _IBO, _wireframeIBO;
        std::vector<unsigned int> _wireframeIndices;
        int _vertexCount;
        int _indexCount;
        int _wireframeIndexCount;
        Parser *_parser;

    public:
        Mesh(Parser *parser);
        ~Mesh();

        int getVertexCount() const;
        int getIndexCount() const;
        int getWireframeIndexCount() const;
        unsigned int getVAO() const;
        unsigned int getBVO() const;
        unsigned int getIBO() const;
        unsigned int getWireframeIBO() const;

        void bind();
        void generateWireframeIndices();
};

#endif