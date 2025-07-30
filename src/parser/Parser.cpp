/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:15:40 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/30 14:52:45 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <glm/glm.hpp>

#include "../../include/Parser.hpp"

Parser::Parser() {}

Parser::~Parser() {}

void Parser::checkExtension(const std::string &filePath) const {
	if (filePath.empty()) {
		throw std::runtime_error("File path cannot be empty");
	}

    std::ifstream testFile(filePath);
    if (!testFile.good()) {
        throw std::runtime_error("File does not exist: " + filePath);
    }

	if (filePath.find_last_of(".") == std::string::npos) {
		throw std::runtime_error("File must have an extension");
	}
	
	if (filePath.substr(filePath.find_last_of(".") + 1) != "obj") {
		throw std::runtime_error("File must have .obj extension");
	}
}

void Parser::parse(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file. Check permissions.");
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            glm::vec3 position;
            iss >> position.x >> position.y >> position.z;
            _positions.push_back(position);
        } else if (type == "vt") {
            glm::vec2 textCoord;
            iss >> textCoord.x >> textCoord.y;
            _texCoords.push_back(textCoord);
        } else if (type == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            _normals.push_back(normal);
        } else if (type == "f") {
            std::string vertexStr;
            std::vector<std::string> faceVertices;

            while (iss >> vertexStr) {
                faceVertices.push_back(vertexStr);
            }

            for (size_t i = 1; i + 1 < faceVertices.size(); ++i) {
                std::string verts[3] = { faceVertices[0], faceVertices[i], faceVertices [i + 1] };
                for (int j = 0; j < 3; ++j) {
                    const std::string &v = verts[j];

                    int posIndex = -1, texIndex = -1, normIndex = -1;
                    size_t firstSlash = v.find('/');
                    if (firstSlash == std::string::npos) {
                        // just "v" case
                        posIndex = std::stoi(v) - 1;
                    } else {
                        //manage different cases: v/vt, v//vn, v/vt/vn
                        posIndex = std::stoi(v.substr(0, firstSlash)) - 1;
                        size_t secondSlash = v.find('/', firstSlash + 1);

                        if (secondSlash == std::string::npos) {
                            //means that there's no second slash, so v/vt case
                            std::string texStr = v.substr(firstSlash + 1);
                            if (!texStr.empty()) {
                                texIndex = std::stoi(texStr) - 1;
                            }
                        } else {
                            std::string texStr = v.substr(firstSlash + 1, secondSlash - firstSlash - 1);
                            if (!texStr.empty()) {
                                texIndex = std::stoi(texStr) - 1;
                            }

                            std::string normStr = v.substr(secondSlash + 1);
                            if (!normStr.empty()) {
                                normIndex = std::stoi(normStr) - 1;
                            }
                        }
                    }

                    //Check if key is already registered to avoid dups
                    FaceKey key{ posIndex, texIndex, normIndex };
                    if (_faceMap.find(key) == _faceMap.end()) {
                        //means key is not in map
                        //fill missing data with 0
                        Vertex vertexData;
                        vertexData.position = (posIndex >= 0 && posIndex < (int)_positions.size()) ? _positions[posIndex] : glm::vec3(0.f);
                        vertexData.texCoord = (texIndex >= 0 && texIndex < (int)_texCoords.size()) ? _texCoords[texIndex] : glm::vec2(0.f);
                        vertexData.normal = (normIndex >= 0 && normIndex < (int)_normals.size()) ? _normals[normIndex] : glm::vec3(0.f);
                        _vertices.push_back(vertexData);
                        _faceMap[key] = (unsigned int)_vertices.size() - 1;
                    }
                    _indices.push_back(_faceMap[key]);
                }
            }
        }
    }
}

const std::vector<Vertex> &Parser::getVertices() const {
	return _vertices;
}

const std::vector<unsigned int> &Parser::getIndices() const {
	return _indices;
}