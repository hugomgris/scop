/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:16:54 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/07 16:34:18 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/**
 * @file main.cpp
 * @brief Entry point for the SCOP 3D model renderer application.
 *
 * This file orchestrates the initialization and execution of the SCOP rendering system.
 * It handles command-line argument parsing, creates the rendering pipeline components
 * (Parser, Mesh, Shader, Renderer), and launches the main application loop.
 *
 * The application supports both OBJ and FDF file formats with real-time rendering,
 * texture management, and interactive controls for wireframe, vertex, and projection modes.
 */

#include <iostream>
#include <string>
#include "../include/App.hpp"
#include "../include/Parser.hpp"
#include "../include/Mesh.hpp"
#include "../include/Shader.hpp"
#include "../include/Renderer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image/stb_image.h"

/**
 * @brief Main entry point for the SCOP application.
 *
 * Initializes the complete rendering pipeline:
 * 1. Validates command-line arguments
 * 2. Creates and configures the Parser for file format detection
 * 3. Parses the input file (OBJ or FDF) into vertex/index data
 * 4. Builds the Mesh from parsed geometry data
 * 5. Compiles and links the 3D shader program
 * 6. Creates the Renderer with shader binding
 * 7. Launches the main App with UI, input handling, and render loop
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 * @return int Exit code (0 for success, 1 for error)
 *
 * @throws std::exception Various exceptions from file parsing, OpenGL initialization, 
 *                        or shader compilation failures
 */
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
