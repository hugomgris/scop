/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:54 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 17:05:03 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include "../include/App.hpp"
#include "../include/Parser.hpp"
#include "../include/Mesh.hpp"
#include "../include/Shader.hpp"
#include "../include/Renderer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image/stb_image.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_obj_file>\n";
        return 1;
    }

    try {
        Parser parser;
        parser.checkExtension(argv[1]);

        std::string modeStr(argv[1]);
        parser.setMode(modeStr);
        parser.parse(argv[1]);
        
        Mesh mesh(&parser);
        Shader shader("resources/shaders/3D.shader");
        Renderer renderer(&shader);

        App app(parser.getMode(), &mesh, &shader, &renderer, &parser);
        
        app.setCurrentFile(argv[1]);

        app.run();

        //debug print
        /* const auto &vertices = parser.getVertices();
        const auto &indices = parser.getIndices();
        std::cout << "Parsed " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;
        for (const auto &vertex : vertices) {
            std::cout << "Vertex Position: " << vertex.position.x << ", " << vertex.position.y << ", " << vertex.position.z << std::endl;
            std::cout << "Texture Coordinate: " << vertex.texCoord.x << ", " << vertex.texCoord.y << std::endl;
            std::cout << "Normal: " << vertex.normal.x << ", " << vertex.normal.y << ", " << vertex.normal.z << std::endl;
        }
        for (const auto &index : indices) {
            std::cout << "Index: " << index << std::endl;
        } */
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
