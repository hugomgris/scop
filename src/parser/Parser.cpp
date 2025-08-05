/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hmunoz-g <hmunoz-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 14:15:40 by hmunoz-g          #+#    #+#             */
/*   Updated: 2025/08/05 11:55:06 by hmunoz-g         ###   ########.fr       */
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
    bool hasNormals = false;
    bool hasTexCoords = false; // Add this flag

    _boundingBox = BoundingBox();
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            glm::vec3 position;
            iss >> position.x >> position.y >> position.z;
            updateMinMaxZ(position.z);
            updateBoundingBox(position);
            _positions.push_back(position);
        } else if (type == "vt") {
            hasTexCoords = true; // Set flag when we find texture coordinates
            glm::vec2 textCoord;
            iss >> textCoord.x >> textCoord.y;
            _texCoords.push_back(textCoord);
        } else if (type == "vn") {
            hasNormals = true;
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
                        posIndex = std::stoi(v) - 1;
                    } else {
                        posIndex = std::stoi(v.substr(0, firstSlash)) - 1;
                        size_t secondSlash = v.find('/', firstSlash + 1);

                        if (secondSlash == std::string::npos) {
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

                    FaceKey key{ posIndex, texIndex, normIndex };
                    if (_faceMap.find(key) == _faceMap.end()) {
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
    
    // Generate texture coordinates if none were found
    if (!hasTexCoords) {
        std::cout << "No texture coordinates found in OBJ file. Generating UV coordinates..." << std::endl;
        
        // Choose the UV generation method based on your needs:
        // For most cases, planar mapping works well
        generateCubicUVs();
        
        // Alternatively, you could use:
        // generateSphericalUVs(); // Good for spherical/organic objects
        // generateCubicUVs();     // Good for box-like objects
    }
    
    if (!hasNormals) {
        std::cout << "No normals found in OBJ file. Calculating normals..." << std::endl;
        calculateNormals();
    }

    std::cout << "Bounding box: Min(" << _boundingBox.min.x << ", " << _boundingBox.min.y << ", " << _boundingBox.min.z << ")" << std::endl;
    std::cout << "              Max(" << _boundingBox.max.x << ", " << _boundingBox.max.y << ", " << _boundingBox.max.z << ")" << std::endl;
    std::cout << "Optimal camera distance: " << getOptimalCameraDistance() << std::endl;
}

void Parser::parseFDF(const std::string &filePath) {
    countFDFPositions(filePath);
    calculateFDFSpacing();
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file. Check permissions.");
    }

    _boundingBox = BoundingBox();
    
    std::vector<std::vector<int>> mapLayout;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        std::string vertexStr;
        std::vector<int> mapLine;

        while (iss >> vertexStr) {
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

    for (size_t i = 0; i < mapLayout.size(); i++) {
        for (size_t j = 0; j < mapLayout[i].size(); j++) {
            glm::vec3 position;
            position.x = (j - (_cols - 1) / 2.0f) * _xSpacing;
            position.y = mapLayout[i][j] * _ySpacing;
            position.z = (i - (_rows - 1) / 2.0f) * _zSpacing;
            updateBoundingBox(position);
            _positions.push_back(position);

            Vertex vertexData;
            vertexData.position = position;
            vertexData.texCoord = glm::vec2(0.f);
            vertexData.normal = glm::vec3(0.f, 0.f, 1.f);
            _vertices.push_back(vertexData);
        }
    }

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

    std::cout << "FDF Bounding box: Min(" << _boundingBox.min.x << ", " << _boundingBox.min.y << ", " << _boundingBox.min.z << ")" << std::endl;
    std::cout << "                  Max(" << _boundingBox.max.x << ", " << _boundingBox.max.y << ", " << _boundingBox.max.z << ")" << std::endl;
    std::cout << "Optimal camera distance: " << getOptimalCameraDistance() << std::endl;
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
    _ySpacing = BASE_SPACING * scaleFactor * 0.1f;
    _zSpacing = BASE_SPACING * scaleFactor;
    
    const float MIN_SPACING = 0.1f;
    _xSpacing = std::max(_xSpacing, MIN_SPACING);
    _ySpacing = std::max(_ySpacing, MIN_SPACING * 0.1f);
    _zSpacing = std::max(_zSpacing, MIN_SPACING);
    
    if (maxDimension > 500) {
        float largeMapMultiplier = 5.0f;
        _xSpacing *= largeMapMultiplier;
        _zSpacing *= largeMapMultiplier;
    }
}

const std::vector<Vertex> &Parser::getVertices() const {
	return _vertices;
}

const std::vector<unsigned int> &Parser::getIndices() const {
	return _indices;
}

void Parser::calculateNormals() {
    std::vector<glm::vec3> vertexNormals(_vertices.size(), glm::vec3(0.0f));
    std::vector<int> normalCounts(_vertices.size(), 0);
    
    for (size_t i = 0; i < _indices.size(); i += 3) {
        if (i + 2 >= _indices.size()) break;
        
        unsigned int idx0 = _indices[i];
        unsigned int idx1 = _indices[i + 1]; 
        unsigned int idx2 = _indices[i + 2];
        
        if (idx0 >= _vertices.size() || idx1 >= _vertices.size() || idx2 >= _vertices.size()) {
            continue;
        }

        glm::vec3 v0 = _vertices[idx0].position;
        glm::vec3 v1 = _vertices[idx1].position;
        glm::vec3 v2 = _vertices[idx2].position;

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));
        
        vertexNormals[idx0] += faceNormal;
        vertexNormals[idx1] += faceNormal;
        vertexNormals[idx2] += faceNormal;
        
        normalCounts[idx0]++;
        normalCounts[idx1]++;
        normalCounts[idx2]++;
    }
    
    for (size_t i = 0; i < _vertices.size(); i++) {
        if (normalCounts[i] > 0) {
            _vertices[i].normal = glm::normalize(vertexNormals[i] / (float)normalCounts[i]);
        } else {
            _vertices[i].normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}

void Parser::updateMinMaxZ(float newZ) {
    if (newZ > _maxZ) {
        _maxZ = newZ;
    } else if (newZ < _minZ) {
        _minZ = newZ;
    }
}

float Parser::getZDifference() const {
    return std::abs(_maxZ - _minZ);
}

void Parser::updateBoundingBox(const glm::vec3& position) {
    _boundingBox.min.x = std::min(_boundingBox.min.x, position.x);
    _boundingBox.min.y = std::min(_boundingBox.min.y, position.y);
    _boundingBox.min.z = std::min(_boundingBox.min.z, position.z);
    
    _boundingBox.max.x = std::max(_boundingBox.max.x, position.x);
    _boundingBox.max.y = std::max(_boundingBox.max.y, position.y);
    _boundingBox.max.z = std::max(_boundingBox.max.z, position.z);
}

const BoundingBox &Parser::getBoundingBox() const {
    return _boundingBox;
}

float Parser::getOptimalCameraDistance() const {
    float diagonal = _boundingBox.getDiagonal();
    float fovRadians = glm::radians(45.0f);
    float distance = (diagonal * 0.5f) / tan(fovRadians * 0.5f);
    
    distance *= 1.2f;
    
    distance = std::max(distance, 2.0f);
    
    return distance;
}

void Parser::generatePlanarUVs() {
    std::cout << "Generating planar UV coordinates..." << std::endl;
    
    glm::vec3 size = _boundingBox.max - _boundingBox.min;
    
    int uAxis = (size.x >= size.y && size.x >= size.z) ? 0 : (size.y >= size.z) ? 1 : 2;
    int vAxis = (uAxis == 0) ? ((size.y >= size.z) ? 1 : 2) : (uAxis == 1) ? ((size.x >= size.z) ? 0 : 2) : 1;
    
    for (size_t i = 0; i < _vertices.size(); i++) {
        glm::vec3 pos = _vertices[i].position;
        
        float u, v;
        switch(uAxis) {
            case 0: u = pos.x; break;
            case 1: u = pos.y; break;
            case 2: u = pos.z; break;
        }
        switch(vAxis) {
            case 0: v = pos.x; break;
            case 1: v = pos.y; break;
            case 2: v = pos.z; break;
        }
        
        u = (u - _boundingBox.min[uAxis]) / size[uAxis];
        v = (v - _boundingBox.min[vAxis]) / size[vAxis];
        
        _vertices[i].texCoord = glm::vec2(u, v);
    }
}

void Parser::generateSphericalUVs() {
    std::cout << "Generating spherical UV coordinates..." << std::endl;
    
    glm::vec3 center = (_boundingBox.min + _boundingBox.max) * 0.5f;
    
    for (size_t i = 0; i < _vertices.size(); i++) {
        glm::vec3 pos = _vertices[i].position - center;
        
        float radius = glm::length(pos);
        if (radius > 0.0f) {
            pos = glm::normalize(pos);
            
            float theta = atan2(pos.z, pos.x);
            float phi = acos(pos.y);
            
            float u = (theta + M_PI) / (2.0f * M_PI);
            float v = phi / M_PI;
            
            _vertices[i].texCoord = glm::vec2(u, v);
        } else {
            _vertices[i].texCoord = glm::vec2(0.5f, 0.5f);
        }
    }
}

void Parser::generateCubicUVs() {
    std::cout << "Generating cubic UV coordinates..." << std::endl;
    
    glm::vec3 center = (_boundingBox.min + _boundingBox.max) * 0.5f;
    
    for (size_t i = 0; i < _vertices.size(); i++) {
        glm::vec3 pos = _vertices[i].position - center;
        glm::vec3 absPos = glm::abs(pos);
        
        float u, v;
        
        if (absPos.x >= absPos.y && absPos.x >= absPos.z) {
            u = (pos.z / absPos.x + 1.0f) * 0.5f;
            v = (pos.y / absPos.x + 1.0f) * 0.5f;
        } else if (absPos.y >= absPos.x && absPos.y >= absPos.z) {
            u = (pos.x / absPos.y + 1.0f) * 0.5f;
            v = (pos.z / absPos.y + 1.0f) * 0.5f;
        } else {
            u = (pos.x / absPos.z + 1.0f) * 0.5f;
            v = (pos.y / absPos.z + 1.0f) * 0.5f;
        }
        
        _vertices[i].texCoord = glm::vec2(u, v);
    }
}