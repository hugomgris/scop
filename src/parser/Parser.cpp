/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:15:40 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/07/31 18:05:25 by hmunoz-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>
#include <glm/glm.hpp>

#include "../../include/Parser.hpp"

Parser::Parser() {}

Parser::~Parser() {}

void Parser::setMode(std::string &filePath) {
	if (filePath.substr(filePath.find_last_of(".") + 1) == "obj") {
		_mode = OBJ;
	} else if (filePath.substr(filePath.find_last_of(".") + 1) == "fdf") {
		_mode = FDF;
	}
}

size_t Parser::getRows() const {
    return _rows;
}

size_t Parser::getColumns() const {
    return _cols;
}

int Parser::getMode() const {
    return _mode;
}

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
	
	if (filePath.substr(filePath.find_last_of(".") + 1) != "obj"
        && filePath.substr(filePath.find_last_of(".") + 1) != "fdf") {
		throw std::runtime_error("File must have .obj or .fdf extension");
	}
}


void Parser::parse(const std::string &filePath) {
    if (_mode == OBJ) {
        parseOBJ(filePath);
    } else if (_mode == FDF) {
        parseFDF(filePath);
    }
}

void Parser::parseOBJ(const std::string &filePath) {
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

void Parser::parseFDF(const std::string &filePath) {
    countFDFPositions(filePath);
    calculateFDFSpacing();
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file. Check permissions.");
    }
    
    std::vector<std::vector<int>> mapLayout;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string vertexStr;
        std::vector<int> mapLine;

        while (iss >> vertexStr) {
            // Handle hex colors if present (format: value,0xcolor)
            size_t commaPos = vertexStr.find(',');
            if (commaPos != std::string::npos) {
                vertexStr = vertexStr.substr(0, commaPos);
            }
            mapLine.push_back(std::stoi(vertexStr));
        }
        
        if (!mapLine.empty()) {
            mapLayout.push_back(mapLine);
        }
    }

    // Generate vertices from the 2D grid
    for (size_t i = 0; i < mapLayout.size(); i++) {
        for (size_t j = 0; j < mapLayout[i].size(); j++) {
            glm::vec3 position;
            // Center the map around origin
            position.x = (j - (_cols - 1) / 2.0f) * _xSpacing;
            position.y = ((_rows - 1) / 2.0f - i) * _ySpacing;
            position.z = mapLayout[i][j] * _zSpacing;
            _positions.push_back(position);

            Vertex vertexData;
            vertexData.position = position;
            vertexData.texCoord = glm::vec2(0.f);
            vertexData.normal = glm::vec3(0.f, 0.f, 1.f); // Default normal pointing up
            _vertices.push_back(vertexData);
        }
    }

    // Generate line indices - connect each vertex to right and down neighbors
    for (size_t i = 0; i < _rows; i++) {
        for (size_t j = 0; j < _cols; j++) {
            size_t currentIndex = i * _cols + j;

            if (j < _cols - 1) {
                _indices.push_back(currentIndex);
                _indices.push_back(currentIndex + 1);
            }

            if (i < _rows - 1) {
                _indices.push_back(currentIndex);
                _indices.push_back(currentIndex + _cols);
            }
        }
    }
}

void Parser::countFDFPositions(const std::string &filePath) {
    _rows = 0;
    _cols = 0;
    bool countedCols = false;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file. Check permissions.");
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        _rows++;

        if (!countedCols) {
            std::istringstream iss(line);
            std::string token;
            while (iss >> token) {
                _cols++;
            }
            countedCols = true;
        }
    }
}

void Parser::calculateFDFSpacing() {
    const float BASE_SPACING = 1.0f;
    
    float scaleFactor = 1.0f;
    
    if (_rows > 100 || _cols > 100) {
        scaleFactor = 0.25f;
    } else if (_rows > 50 || _cols > 50) {
        scaleFactor = 0.5f;
    }
    
    float maxDimension = static_cast<float>(std::max(_rows, _cols));
    if (maxDimension > 20) {
        scaleFactor *= (20.0f / maxDimension);
    }
    
    _xSpacing = BASE_SPACING * scaleFactor;
    _ySpacing = BASE_SPACING * scaleFactor;
    _zSpacing = BASE_SPACING * scaleFactor * 0.1f;
    
    const float MIN_SPACING = 0.1f;
    _xSpacing = std::max(_xSpacing, MIN_SPACING);
    _ySpacing = std::max(_ySpacing, MIN_SPACING);
    _zSpacing = std::max(_zSpacing, MIN_SPACING * 0.1f);
    
    if (maxDimension > 500) {
        float largeMapMultiplier = 5.0f;
        _xSpacing *= largeMapMultiplier;
        _ySpacing *= largeMapMultiplier;
    }
}

const std::vector<Vertex> &Parser::getVertices() const {
	return _vertices;
}

const std::vector<unsigned int> &Parser::getIndices() const {
	return _indices;
}